#include "onkyo_iscp.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome::onkyo_iscp {

static const char *const TAG = "onkyo_iscp";

void OnkyoIscp::setup() {
  rx_buffer_.reserve(MAX_FRAME_LENGTH);
  if (connected_binary_sensor_ != nullptr) {
    connected_binary_sensor_->publish_state(false);
}

  if (last_frame_sensor_ != nullptr) {
  last_frame_sensor_->publish_state("none");
  }

  if (last_unknown_frame_sensor_ != nullptr) {
  last_unknown_frame_sensor_->publish_state("none");
  }

  ESP_LOGI(TAG, "Onkyo ISCP UART component started");
  this->enqueue_command_("PWRQSTN");
}

void OnkyoIscp::dump_config() {
  ESP_LOGCONFIG(TAG, "Onkyo ISCP:");
  LOG_UPDATE_INTERVAL(this);
}

void OnkyoIscp::loop() {
  this->read_uart_();
  this->process_queue_();
  this->check_receiver_timeout_();
}

void OnkyoIscp::update() { 
  ESP_LOGV(TAG, "Sending receiver heartbeat");
  this->enqueue_command_("PWRQSTN"); 
}

void OnkyoIscp::mark_receiver_online_() {
  last_valid_frame_ms_ = millis();

  if (receiver_online_) {
    return;
  }

  receiver_online_ = true;

  ESP_LOGI(TAG, "Receiver communication established");

  if (connected_binary_sensor_ != nullptr) {
    connected_binary_sensor_->publish_state(true);
  }
  this->query_all();
}

void OnkyoIscp::check_receiver_timeout_() {
  if (!receiver_online_) {
    return;
  }

  const uint32_t now = millis();

  if (now - last_valid_frame_ms_ <= RECEIVER_TIMEOUT_MS) {
    return;
  }

  receiver_online_ = false;

  ESP_LOGW(
      TAG,
      "Receiver communication timeout after %u ms",
      RECEIVER_TIMEOUT_MS
  );

  if (connected_binary_sensor_ != nullptr) {
    connected_binary_sensor_->publish_state(false);
  }
}

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
  this->enqueue_command_("LMDQSTN");
  this->enqueue_command_("TFRQSTN");
}

std::string OnkyoIscp::tone_value_to_code_(float value) {
  int rounded =
      static_cast<int>(std::lround(value / 2.0f)) * 2;
  rounded = std::max(-10, std::min(10,rounded));
  if (rounded == 0) {
    return "00";
  }
  const char sign = rounded > 0 ? '+' : '-';
  const int magnitude = std::abs(rounded);
  char magnitude_character;
  if (magnitude == 10) {
    magnitude_character = 'A';
  } else {
    magnitude_character =
        static_cast<char>('0' + magnitude);
  }
  std::string result;
  result.push_back(sign);
  result.push_back(magnitude_character);
  return result;
}

bool OnkyoIscp::tone_code_to_value_(const std::string &code, float &value) {
  if (code == "00") {
    value = 0.0f;
    return true;
  }
  if (code.size() != 2) {
    return false;
  }
  const char sign = code[0];
  const char magnitude_character = code[1];
  if (sign != '+' && sign != '-') {
    return false;
  }
  int magnitude;
  if (magnitude_character == 'A') {
    magnitude = 10;
  } else if (magnitude_character >= '0' && magnitude_character <= '9') {
    magnitude = magnitude_character - '0';
  } else {
    return false;
  }
  if (
      magnitude < 0 ||
      magnitude > 10 ||
      magnitude % 2 != 0
  ) {
    return false;
  }
  value = static_cast<float>(
      sign == '-' ? -magnitude : magnitude
  );
  return true;
}

void OnkyoIscp::set_power(bool state) { this->send_command(state ? "PWR01" : "PWR00"); }
void OnkyoIscp::set_mute(bool state) { this->send_command(state ? "AMT01" : "AMT00"); }
void OnkyoIscp::set_front_bass(float value) {this->send_command("TFRB" + tone_value_to_code_(value)); }
void OnkyoIscp::set_front_treble(float value) {this->send_command("TFRT" + tone_value_to_code_(value)); }
void OnkyoIscp::set_volume(float raw_value) {
  int value = std::max(0, std::min(100, static_cast<int>(raw_value + 0.5f)));
  char command[8];
  std::snprintf(command, sizeof(command), "MVL%02X", value);
  this->send_command(command);
}
void OnkyoIscp::process_front_tone_(
    const std::string &value)
  {
  size_t position = 0;
  while (position < value.size()) {
    const char tone_type = value[position];
    if (
        tone_type != '*' &&
        tone_type != 'T'
    ) {
      ESP_LOGW(
          TAG,
          "Invalid TFR response: %s",
          value.c_str()
      );
      return;
    }
    if (position + 3 > value.size()) {
      ESP_LOGW( TAG, "Incomplete TFR response: %s", value.c_str());
      return;
    }
    const std::string code =
        value.substr(position + 1, 2);
    float tone_value;
    if (!tone_code_to_value_(code, tone_value)) {
      ESP_LOGW( TAG, "Invalid TFR tone value: %s", code.c_str());
      return;
    }
    if (
        tone_type == 'B' &&
        front_bass_number_ != nullptr
    ) {
      front_bass_number_->publish_state(tone_value);
    }
     if (
        tone_type == 'T' &&
        front_treble_number_ != nullptr
    ) {
      front_treble_number_->publish_state(tone_value);
    }
    position += 3;
  }
}

void OnkyoIscp::set_input(const std::string &input) {
  const std::string code = input_name_to_code_(input);
  if (code.empty()) {
    ESP_LOGW(TAG, "Unknown input option: %s", input.c_str());
    return;
  }
  this->send_command("SLI" + code);
}

void OnkyoIscp::set_listening_mode(const std::string &mode) {
  const std::string code =  listening_mode_name_to_code_(mode);
  if (code.empty()) {
    ESP_LOGW(TAG, "Unknown listening mode option: %s", mode.c_str());
    return;
  }
  this->send_command("LMD" + code);
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
        if (value == '!') {
      rx_buffer_.clear();
      rx_buffer_.push_back('!');
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

bool OnkyoIscp::is_valid_ascii_frame_(
    const std::string &frame
) {
  if (frame.empty()) {
    return false;
  }

  for (const unsigned char character : frame) {
    if (character < 0x20 || character > 0x7E) {
      return false;
    }
  }

  return true;
}

std::string OnkyoIscp::normalize_frame_(std::string frame) {
  const size_t start = frame.rfind("!1");
  if (start == std::string::npos) {
    return {};
  }
  frame.erase(0, start + 2);
  return frame;
}

void OnkyoIscp::process_frame_(std::string frame) {
  if (!is_valid_ascii_frame_(frame)) {
    ESP_LOGW(TAG, "Discarding non-ASCII UART frame (%u bytes)", static_cast<unsigned int>(frame.size()));
    return;
  }
  frame = normalize_frame_(std::move(frame));
  if (frame.size() < 3) {
    ESP_LOGV(TAG, "Discarding invalid ISCP frame");
    return;
  }
  const std::string command = frame.substr(0, 3);
  for (const unsigned char character : command) {
    const bool valid =
        (character >= 'A' && character <= 'Z') ||
        (character >= '0' && character <= '9');

    if (!valid) {
      ESP_LOGW(
          TAG,
          "Discarding frame with invalid command"
      );
      return;
    }
  }

  this->mark_receiver_online_();

  ESP_LOGD(TAG, "RX: %s", frame.c_str());

  this->status_clear_warning();

  if (last_frame_sensor_ != nullptr) {
    last_frame_sensor_->publish_state(frame);
  }

  this->process_command_(
      command,
      frame.substr(3)
  );
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
  } else if (command == "LMD" && listening_mode_select_ != nullptr) {
    const std::string name = listening_mode_code_to_name_(value);
    if (!name.empty()) {listening_mode_select_->publish_state(name);
    } else {ESP_LOGW(TAG, "Unknown listening mode code: %s", value.c_str());
    }
  } else if (command == "TFR") {
    this->process_front_tone_(value);
  } else if (command == "FLD" && display_sensor_ != nullptr) {
    display_sensor_->publish_state(value);
  } else {
    const std::string unknown_frame = command + value;
    ESP_LOGD(TAG, "Unhandled ISCP frame: %s", unknown_frame.c_str());
    if (last_unknown_frame_sensor_ != nullptr) {
      last_unknown_frame_sensor_->publish_state(unknown_frame);
    }
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

std::string OnkyoIscp::listening_mode_code_to_name_(const std::string &code) {
  if (code == "00") return "Stereo";
  if (code == "01") return "Direct";
  if (code == "02") return "Surround";
  if (code == "03") return "Game RPG";
  if (code == "05") return "Game Action";
  if (code == "06") return "Game Rock";
  if (code == "08") return "Orchestra";
  if (code == "09") return "Unplugged";
  if (code == "0A") return "Studio-Mix";
  if (code == "0B") return "TV Logic";
  if (code == "0C") return "All Ch Stereo";
  if (code == "0D") return "Theater-Dimensional";
  if (code == "0E") return "Game Sports";
  if (code == "0F") return "Mono";
  if (code == "13") return "Full Mono";
  if (code == "16") return "Audyssey DSX";
  if (code == "40") return "Straight Decode";
  if (code == "42") return "THX Cinema";
  if (code == "43") return "THX Surround EX";
  if (code == "44") return "THX Music";
  if (code == "45") return "THX Games";
  if (code == "50") return "THX Cinema 2";
  if (code == "51") return "THX Music Mode";
  if (code == "52") return "THX Games Mode";
  if (code == "80") return "PLII/PLIIx Movie";
  if (code == "81") return "PLII/PLIIx Music";
  if (code == "82") return "Neo:6 Cinema";
  if (code == "83") return "Neo:6 Music";
  if (code == "84") return "PLII/PLIIx THX Cinema";
  if (code == "85") return "Neo:6 THX Cinema";
  if (code == "86") return "PLII/PLIIx Game";
  if (code == "89") return "PLII/PLIIx THX Games";
  if (code == "8A") return "Neo:6 THX Games";
  if (code == "8B") return "PLII/PLIIx THX Music";
  if (code == "8C") return "Neo:6 THX Music";
  if (code == "90") return "PLIIz Height";
  if (code == "94") return "PLIIz Height + THX Cinema";
  if (code == "95") return "PLIIz Height + THX Music";
  if (code == "96") return "PLIIz Height + THX Games";
  if (code == "A0") return "PLII/PLIIx Movie + Audyssey DSX";
  if (code == "A1") return "PLII/PLIIx Music + Audyssey DSX";
  if (code == "A2") return "PLII/PLIIx Game + Audyssey DSX";
  if (code == "A3") return "Neo:6 Cinema + Audyssey DSX";
  if (code == "A4") return "Neo:6 Music + Audyssey DSX";
  return {};
}

std::string OnkyoIscp::listening_mode_name_to_code_(const std::string &name) {
  static const char *const options[][2] = {
      {"Stereo", "00"},
      {"Direct", "01"},
      {"Surround", "02"},
      {"Game RPG", "03"},
      {"Game Action", "05"},
      {"Game Rock", "06"},
      {"Orchestra", "08"},
      {"Unplugged", "09"},
      {"Studio-Mix", "0A"},
      {"TV Logic", "0B"},
      {"All Ch Stereo", "0C"},
      {"Theater-Dimensional", "0D"},
      {"Game Sports", "0E"},
      {"Mono", "0F"},
      {"Full Mono", "13"},
      {"Audyssey DSX", "16"},
      {"Straight Decode", "40"},
      {"THX Cinema", "42"},
      {"THX Surround EX", "43"},
      {"THX Music", "44"},
      {"THX Games", "45"},
      {"THX Cinema 2", "50"},
      {"THX Music Mode", "51"},
      {"THX Games Mode", "52"},
      {"PLII/PLIIx Movie", "80"},
      {"PLII/PLIIx Music", "81"},
      {"Neo:6 Cinema", "82"},
      {"Neo:6 Music", "83"},
      {"PLII/PLIIx THX Cinema", "84"},
      {"Neo:6 THX Cinema", "85"},
      {"PLII/PLIIx Game", "86"},
      {"PLII/PLIIx THX Games", "89"},
      {"Neo:6 THX Games", "8A"},
      {"PLII/PLIIx THX Music", "8B"},
      {"Neo:6 THX Music", "8C"},
      {"PLIIz Height", "90"},
      {"PLIIz Height + THX Cinema", "94"},
      {"PLIIz Height + THX Music", "95"},
      {"PLIIz Height + THX Games", "96"},
      {"PLII/PLIIx Movie + Audyssey DSX", "A0"},
      {"PLII/PLIIx Music + Audyssey DSX", "A1"},     
      {"PLII/PLIIx Game + Audyssey DSX", "A2"},
      {"Neo:6 Cinema + Audyssey DSX", "A3"},
      {"Neo:6 Music + Audyssey DSX", "A4"},
  };
  for (const auto &option : options) if (name == option[0]) return option[1];
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
void OnkyoFrontBassNumber::control(float value) { if (parent_ != nullptr) { parent_->set_front_bass(value); } }
void OnkyoFrontTrebleNumber::control(float value) { if (parent_ != nullptr) { parent_->set_front_treble(value); } }
void OnkyoInputSelect::control(const std::string &value) { if (parent_) parent_->set_input(value); }
void OnkyoVolumeUpButton::press_action() { if (parent_) parent_->send_command("MVLUP"); }
void OnkyoVolumeDownButton::press_action() { if (parent_) parent_->send_command("MVLDOWN"); }
void OnkyoQueryAllButton::press_action() { if (parent_) parent_->query_all(); }
void OnkyoListeningModeSelect::control(const std::string &value) { if (parent_) parent_->set_listening_mode(value); }

}  // namespace esphome::onkyo_iscp
