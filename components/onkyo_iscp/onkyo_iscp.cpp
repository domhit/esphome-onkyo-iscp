#include "onkyo_iscp.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>

#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome::onkyo_iscp {

static const char *const TAG = "onkyo_iscp";

void OnkyoIscp::setup() {
  rx_buffer_.reserve(MAX_FRAME_LENGTH);
  ESP_LOGI(TAG, "Onkyo ISCP UART component started");
  this->query_all();
}

void OnkyoIscp::dump_config() {
  ESP_LOGCONFIG(TAG, "Onkyo ISCP:");
  LOG_UPDATE_INTERVAL(this);
}

void OnkyoIscp::loop() {
  this->read_uart_();
  this->process_queue_();
}

void OnkyoIscp::update() { this->query_all(); }

void OnkyoIscp::send_command(const std::string &command) {
  if (command.empty()) return;
  std::string frame = command;
  if (frame.rfind("!1", 0) != 0) frame.insert(0, "!1");
  if (frame.back() != '\r') frame.push_back('\r');
  this->write_str(frame.c_str());
  ESP_LOGD(TAG, "TX: %s", frame.c_str());
}

void OnkyoIscp::enqueue_command_(const std::string &command) {
  command_queue_.push_back(command);
}

void OnkyoIscp::process_queue_() {
  if (command_queue_.empty()) return;
  const uint32_t now = millis();
  if (now - last_command_ms_ < COMMAND_GAP_MS) return;
  this->send_command(command_queue_.front());
  command_queue_.pop_front();
  last_command_ms_ = now;
}

void OnkyoIscp::query_all() {
  this->enqueue_command_("PWRQSTN");
  this->enqueue_command_("MVLQSTN");
  this->enqueue_command_("AMTQSTN");
  this->enqueue_command_("SLIQSTN");
}

void OnkyoIscp::set_power(bool state) { this->send_command(state ? "PWR01" : "PWR00"); }
void OnkyoIscp::set_mute(bool state) { this->send_command(state ? "AMT01" : "AMT00"); }

void OnkyoIscp::set_volume(float raw_value) {
  int value = std::max(0, std::min(100, static_cast<int>(raw_value + 0.5f)));
  char command[8];
  std::snprintf(command, sizeof(command), "MVL%02X", value);
  this->send_command(command);
}

void OnkyoIscp::set_input(const std::string &input) {
  const std::string code = input_name_to_code_(input);
  if (code.empty()) {
    ESP_LOGW(TAG, "Unknown input option: %s", input.c_str());
    return;
  }
  this->send_command("SLI" + code);
}

void OnkyoIscp::read_uart_() {
  while (this->available()) {
    uint8_t value;
    if (!this->read_byte(&value)) return;
    if (value == '\r' || value == '\n' || value == 0x1A) {
      if (!rx_buffer_.empty()) {
        this->process_frame_(rx_buffer_);
        rx_buffer_.clear();
      }
      continue;
    }
    if (rx_buffer_.size() >= MAX_FRAME_LENGTH) {
      ESP_LOGW(TAG, "UART frame overflow, buffer cleared");
      rx_buffer_.clear();
      this->status_set_warning();
      continue;
    }
    rx_buffer_.push_back(static_cast<char>(value));
  }
}

std::string OnkyoIscp::normalize_frame_(std::string frame) {
  while (!frame.empty() && static_cast<uint8_t>(frame.front()) < 0x20) frame.erase(frame.begin());
  if (frame.rfind("!1", 0) == 0) frame.erase(0, 2);
  return frame;
}

void OnkyoIscp::process_frame_(std::string frame) {
  frame = normalize_frame_(std::move(frame));
  if (frame.size() < 3) return;
  ESP_LOGD(TAG, "RX: %s", frame.c_str());
  this->status_clear_warning();
  if (last_frame_sensor_ != nullptr) last_frame_sensor_->publish_state(frame);
  this->process_command_(frame.substr(0, 3), frame.substr(3));
}

void OnkyoIscp::process_command_(const std::string &command, const std::string &value) {
  if (command == "PWR" && power_switch_ != nullptr) {
    if (value == "01") power_switch_->publish_state(true);
    else if (value == "00") power_switch_->publish_state(false);
  } else if (command == "AMT" && mute_switch_ != nullptr) {
    if (value == "01") mute_switch_->publish_state(true);
    else if (value == "00") mute_switch_->publish_state(false);
  } else if (command == "MVL" && volume_number_ != nullptr) {
    char *end = nullptr;
    const long raw = std::strtol(value.c_str(), &end, 16);
    if (end != value.c_str()) volume_number_->publish_state(static_cast<float>(raw));
  } else if (command == "SLI" && input_select_ != nullptr) {
    const std::string name = input_code_to_name_(value);
    if (!name.empty()) input_select_->publish_state(name);
  } else if (command == "FLD" && display_sensor_ != nullptr) {
    display_sensor_->publish_state(value);
  } else {
    ESP_LOGV(TAG, "Unhandled command %s value %s", command.c_str(), value.c_str());
  }
}

std::string OnkyoIscp::input_code_to_name_(const std::string &code) {
  if (code == "00") return "VCR/DVR";
  if (code == "01") return "CBL/SAT";
  if (code == "02") return "GAME/TV";
  if (code == "03") return "AUX1";
  if (code == "04") return "AUX2";
  if (code == "05") return "PC";
  if (code == "10") return "BD/DVD";
  if (code == "20") return "TAPE";
  if (code == "22") return "PHONO";
  if (code == "23") return "TV/CD";
  if (code == "24") return "FM";
  if (code == "25") return "AM";
  if (code == "26") return "TUNER";
  if (code == "30") return "MULTI CH";
  if (code == "40") return "UNIVERSAL PORT";
  if (code == "55") return "HDMI 5";
  if (code == "56") return "HDMI 6";
  if (code == "57") return "HDMI 7";
  return {};
}

std::string OnkyoIscp::input_name_to_code_(const std::string &name) {
  static const char *const options[][2] = {
      {"VCR/DVR", "00"}, {"CBL/SAT", "01"}, {"GAME/TV", "02"}, {"AUX1", "03"},
      {"AUX2", "04"}, {"PC", "05"}, {"BD/DVD", "10"}, {"TAPE", "20"},
      {"PHONO", "22"}, {"TV/CD", "23"}, {"FM", "24"}, {"AM", "25"},
      {"TUNER", "26"}, {"MULTI CH", "30"}, {"UNIVERSAL PORT", "40"},
      {"HDMI 5", "55"}, {"HDMI 6", "56"}, {"HDMI 7", "57"},
  };
  for (const auto &option : options) if (name == option[0]) return option[1];
  return {};
}

void OnkyoPowerSwitch::write_state(bool state) { if (parent_) parent_->set_power(state); }
void OnkyoMuteSwitch::write_state(bool state) { if (parent_) parent_->set_mute(state); }
void OnkyoVolumeNumber::control(float value) { if (parent_) parent_->set_volume(value); }
void OnkyoInputSelect::control(const std::string &value) { if (parent_) parent_->set_input(value); }
void OnkyoVolumeUpButton::press_action() { if (parent_) parent_->send_command("MVLUP"); }
void OnkyoVolumeDownButton::press_action() { if (parent_) parent_->send_command("MVLDOWN"); }
void OnkyoQueryAllButton::press_action() { if (parent_) parent_->query_all(); }

}  // namespace esphome::onkyo_iscp
