#include "onkyo_iscp.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "esphome/core/hal.h"
#include "esphome/core/log.h"

namespace esphome::onkyo_iscp {

static const char* const TAG = "onkyo_iscp";

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

  ESP_LOGW(TAG, "Receiver communication timeout after %u ms",
           RECEIVER_TIMEOUT_MS);

  if (connected_binary_sensor_ != nullptr) {
    connected_binary_sensor_->publish_state(false);
  }
}

void OnkyoIscp::send_command(const std::string& command) {
  if (command.empty()) return;
  std::string frame = command;
  if (frame.rfind("!1", 0) != 0) frame.insert(0, "!1");
  if (frame.back() != '\r') frame.push_back('\r');
  this->write_str(frame.c_str());
  ESP_LOGD(TAG, "TX: %s", frame.c_str());
}

void OnkyoIscp::enqueue_command_(const std::string& command) {
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
  this->enqueue_command_("SWLQSTN");
  this->enqueue_command_("CTLQSTN");
  this->enqueue_command_("ADYQSTN");
  this->enqueue_command_("ADQQSTN");
  this->enqueue_command_("ADVQSTN");
  this->enqueue_command_("LTNQSTN");
  this->enqueue_command_("RASQSTN");
  this->enqueue_command_("MOTQSTN");
  this->enqueue_command_("DIMQSTN");
  this->enqueue_command_("SLPQSTN");
  this->enqueue_command_("SLAQSTN");
  this->enqueue_command_("SPLQSTN");
  this->enqueue_command_("TUNQSTN");
  this->enqueue_command_("PRSQSTN");
}


void OnkyoIscp::set_audyssey(bool state) {
  this->send_command(state ? "ADY01" : "ADY00");

  this->enqueue_command_("ADYQSTN");
}

void OnkyoIscp::set_dynamic_eq(bool state) {
  this->send_command(state ? "ADQ01" : "ADQ00");

  this->enqueue_command_("ADQQSTN");
}

void OnkyoIscp::set_re_eq(bool state) {
  this->send_command(state ? "RAS01" : "RAS00");

  this->enqueue_command_("RASQSTN");
}

void OnkyoIscp::set_late_night(const std::string& mode) {
  const std::string code = late_night_name_to_code_(mode);

  if (code.empty()) {
    ESP_LOGW(TAG, "Unknown Late Night option: %s", mode.c_str());
    return;
  }

  this->send_command("LTN" + code);
  this->enqueue_command_("LTNQSTN");
}

void OnkyoIscp::set_music_optimizer(bool state) {
  this->send_command(state ? "MOT01" : "MOT00");

  this->enqueue_command_("MOTQSTN");
}

void OnkyoIscp::set_dynamic_volume(const std::string& mode) {
  const std::string code = dynamic_volume_name_to_code_(mode);

  if (code.empty()) {
    ESP_LOGW(TAG, "Unknown Dynamic Volume option: %s", mode.c_str());
    return;
  }

  this->send_command("ADV" + code);
  this->enqueue_command_("ADVQSTN");
}

void OnkyoIscp::set_dimmer(const std::string& mode) {
  const std::string code = dimmer_name_to_code_(mode);

  if (code.empty()) {
    ESP_LOGW(TAG, "Unknown dimmer option: %s", mode.c_str());
    return;
  }

  this->send_command("DIM" + code);
  this->enqueue_command_("DIMQSTN");
}

void OnkyoIscp::set_sleep_timer(float minutes) {
  int rounded = static_cast<int>(std::lround(minutes));

  rounded = std::max(0, std::min(90, rounded));

  if (rounded == 0) {
    this->send_command("SLPOFF");
    this->enqueue_command_("SLPQSTN");
    return;
  }

  char code[3];

  std::snprintf(code, sizeof(code), "%02X", rounded);

  this->send_command("SLP" + std::string(code));

  this->enqueue_command_("SLPQSTN");
}

void OnkyoIscp::set_fm_frequency(float frequency) {
  if (tuner_band_ != TunerBand::FM) {
    ESP_LOGW(TAG, "Cannot set FM frequency while FM input is not active");
    return;
  }

  const int encoded = static_cast<int>(std::lround(frequency * 100.0f));

  if (encoded < 8750 || encoded > 10800) {
    ESP_LOGW(TAG, "FM frequency out of range: %.2f MHz", frequency);
    return;
  }

  char command[9];
  std::snprintf(command, sizeof(command), "TUN%05d", encoded);

  this->send_command(command);
  this->enqueue_command_("TUNQSTN");
}

void OnkyoIscp::set_am_frequency(float frequency) {
  if (tuner_band_ != TunerBand::AM) {
    ESP_LOGW(TAG, "Cannot set AM frequency while AM input is not active");
    return;
  }

  const int encoded = static_cast<int>(std::lround(frequency));

  if (encoded < 522 || encoded > 1611) {
    ESP_LOGW(TAG, "AM frequency out of range: %d kHz", encoded);
    return;
  }

  char command[9];
  std::snprintf(command, sizeof(command), "TUN%05d", encoded);

  this->send_command(command);
  this->enqueue_command_("TUNQSTN");
}

void OnkyoIscp::set_tuner_preset(float preset) {
  int value = static_cast<int>(std::lround(preset));
  value = std::max(1, std::min(40, value));

  current_tuner_preset_ = static_cast<uint8_t>(value);

  char command[8];
  std::snprintf(command, sizeof(command), "PRS%02X", value);

  this->send_command(command);
  this->enqueue_command_("PRSQSTN");
}

void OnkyoIscp::process_tuner_frequency_(const std::string &value) {
  if (value.empty()) {
    ESP_LOGW(TAG, "Empty tuner frequency response");
    return;
  }

  char *end = nullptr;
  const long raw = std::strtol(value.c_str(), &end, 10);

  if (end == value.c_str() || *end != '\0') {
    ESP_LOGW(TAG, "Invalid tuner frequency: %s", value.c_str());
    return;
  }

  TunerBand detected_band = tuner_band_;

  if (detected_band == TunerBand::UNKNOWN) {
    if (raw >= 8750 && raw <= 10800) {
      detected_band = TunerBand::FM;
    } else if (raw >= 522 && raw <= 1611) {
      detected_band = TunerBand::AM;
    }
  }

  if (detected_band == TunerBand::FM) {
    const float frequency = static_cast<float>(raw) / 100.0f;

    if (frequency < 87.5f || frequency > 108.0f) {
      ESP_LOGW(TAG, "FM frequency out of range: %.2f", frequency);
      return;
    }

    tuner_band_ = TunerBand::FM;

    if (fm_frequency_number_ != nullptr) {
      fm_frequency_number_->publish_state(frequency);
    }

    return;
  }

  if (detected_band == TunerBand::AM) {
    if (raw < 522 || raw > 1611) {
      ESP_LOGW(TAG, "AM frequency out of range: %ld", raw);
      return;
    }

    tuner_band_ = TunerBand::AM;

    if (am_frequency_number_ != nullptr) {
      am_frequency_number_->publish_state(static_cast<float>(raw));
    }

    return;
  }

  ESP_LOGW(
      TAG,
      "Unable to determine tuner band for frequency: %s",
      value.c_str()
  );
}

void OnkyoIscp::process_tuner_preset_(const std::string &value) {
  char *end = nullptr;
  const long preset = std::strtol(value.c_str(), &end, 16);

  if (
      end == value.c_str() ||
      *end != '\0' ||
      preset < 1 ||
      preset > 40
  ) {
    ESP_LOGW(TAG, "Invalid tuner preset: %s", value.c_str());
    return;
  }

  current_tuner_preset_ = static_cast<uint8_t>(preset);

  if (tuner_preset_number_ != nullptr) {
    tuner_preset_number_->publish_state(static_cast<float>(preset));
  }
}

void OnkyoIscp::process_sleep_timer_(const std::string& value) {
  if (sleep_timer_number_ == nullptr) {
    return;
  }

  if (value == "OFF") {
    sleep_timer_number_->publish_state(0.0f);
    return;
  }

  if (value.size() != 2) {
    ESP_LOGW(TAG, "Invalid sleep timer response: %s", value.c_str());
    return;
  }

  char* end = nullptr;

  const long minutes = std::strtol(value.c_str(), &end, 16);

  if (end == value.c_str() || *end != '\0' || minutes < 0 || minutes > 90) {
    ESP_LOGW(TAG, "Invalid sleep timer value: %s", value.c_str());
    return;
  }

  sleep_timer_number_->publish_state(static_cast<float>(minutes));
}

void OnkyoIscp::set_audio_selector(const std::string& selector) {
  const std::string code = audio_selector_name_to_code_(selector);

  if (code.empty()) {
    ESP_LOGW(TAG, "Unknown audio selector option: %s", selector.c_str());
    return;
  }

  this->send_command("SLA" + code);
  this->enqueue_command_("SLAQSTN");
}

void OnkyoIscp::set_speaker_layout(const std::string& layout) {
  const std::string code = speaker_layout_name_to_code_(layout);

  if (code.empty()) {
    ESP_LOGW(TAG, "Unknown speaker layout option: %s", layout.c_str());
    return;
  }

  this->send_command("SPL" + code);
  this->enqueue_command_("SPLQSTN");
}

std::string OnkyoIscp::dynamic_volume_code_to_name_(const std::string& code) {
  if (code == "00") {
    return "Off";
  }

  if (code == "01") {
    return "Light";
  }

  if (code == "02") {
    return "Medium";
  }

  if (code == "03") {
    return "Heavy";
  }

  return {};
}

std::string OnkyoIscp::dynamic_volume_name_to_code_(const std::string& name) {
  if (name == "Off") {
    return "00";
  }

  if (name == "Light") {
    return "01";
  }

  if (name == "Medium") {
    return "02";
  }

  if (name == "Heavy") {
    return "03";
  }

  return {};
}

std::string OnkyoIscp::audio_selector_code_to_name_(const std::string& code) {
  if (code == "02") {
    return "Analog";
  }

  if (code == "04") {
    return "HDMI";
  }

  if (code == "05") {
    return "Coax/Optical";
  }

  if (code == "07") {
    return "ARC";
  }

  return {};
}

std::string OnkyoIscp::audio_selector_name_to_code_(const std::string& name) {
  if (name == "Analog") {
    return "02";
  }

  if (name == "HDMI") {
    return "04";
  }

  if (name == "Coax/Optical") {
    return "05";
  }

  if (name == "ARC") {
    return "07";
  }

  return {};
}

std::string OnkyoIscp::speaker_layout_code_to_name_(const std::string& code) {
  if (code == "SB") {
    return "Surround Back";
  }

  if (code == "FH") {
    return "Front High";
  }

  if (code == "FW") {
    return "Front Wide";
  }

  return {};
}

std::string OnkyoIscp::speaker_layout_name_to_code_(const std::string& name) {
  if (name == "Surround Back") {
    return "SB";
  }

  if (name == "Front High") {
    return "FH";
  }

  if (name == "Front Wide") {
    return "FW";
  }

  return {};
}

std::string OnkyoIscp::tone_value_to_code_(float value) {
  int rounded = static_cast<int>(std::lround(value / 2.0f)) * 2;
  rounded = std::max(-10, std::min(10, rounded));
  if (rounded == 0) {
    return "00";
  }
  const char sign = rounded > 0 ? '+' : '-';
  const int magnitude = std::abs(rounded);
  char magnitude_character;
  if (magnitude == 10) {
    magnitude_character = 'A';
  } else {
    magnitude_character = static_cast<char>('0' + magnitude);
  }
  std::string result;
  result.push_back(sign);
  result.push_back(magnitude_character);
  return result;
}

bool OnkyoIscp::tone_code_to_value_(const std::string& code, float& value) {
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
  if (magnitude < 0 || magnitude > 10 || magnitude % 2 != 0) {
    return false;
  }
  value = static_cast<float>(sign == '-' ? -magnitude : magnitude);
  return true;
}

std::string OnkyoIscp::late_night_code_to_name_(const std::string& code) {
  if (code == "00") {
    return "Off";
  }

  if (code == "01") {
    return "Low";
  }

  if (code == "02") {
    return "High";
  }

  if (code == "03") {
    return "Auto";
  }

  return {};
}

std::string OnkyoIscp::late_night_name_to_code_(const std::string& name) {
  if (name == "Off") {
    return "00";
  }

  if (name == "Low") {
    return "01";
  }

  if (name == "High") {
    return "02";
  }

  if (name == "Auto") {
    return "03";
  }

  return {};
}

std::string OnkyoIscp::level_value_to_code_(float value, int minimum,
                                            int maximum) {
  int rounded = static_cast<int>(std::lround(value));

  rounded = std::max(minimum, std::min(maximum, rounded));

  if (rounded == 0) {
    return "00";
  }

  const char sign = rounded > 0 ? '+' : '-';
  const int magnitude = std::abs(rounded);

  char magnitude_character;

  if (magnitude <= 9) {
    magnitude_character = static_cast<char>('0' + magnitude);
  } else {
    magnitude_character = static_cast<char>('A' + magnitude - 10);
  }

  std::string result;
  result.push_back(sign);
  result.push_back(magnitude_character);

  return result;
}

bool OnkyoIscp::level_code_to_value_(const std::string& code, float& value) {
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

  if (magnitude_character >= '0' && magnitude_character <= '9') {
    magnitude = magnitude_character - '0';
  } else if (magnitude_character >= 'A' && magnitude_character <= 'F') {
    magnitude = 10 + magnitude_character - 'A';
  } else {
    return false;
  }

  value = static_cast<float>(sign == '-' ? -magnitude : magnitude);

  return true;
}

std::string OnkyoIscp::dimmer_code_to_name_(const std::string& code) {
  if (code == "00") {
    return "Bright";
  }

  if (code == "01") {
    return "Dim";
  }

  if (code == "02") {
    return "Dark";
  }

  return {};
}

std::string OnkyoIscp::dimmer_name_to_code_(const std::string& name) {
  if (name == "Bright") {
    return "00";
  }

  if (name == "Dim") {
    return "01";
  }

  if (name == "Dark") {
    return "02";
  }

  return {};
}

void OnkyoIscp::set_power(bool state) {
  this->send_command(state ? "PWR01" : "PWR00");
}
void OnkyoIscp::set_mute(bool state) {
  this->send_command(state ? "AMT01" : "AMT00");
}
void OnkyoIscp::set_front_bass(float value) {
  this->send_command("TFRB" + tone_value_to_code_(value));
}
void OnkyoIscp::set_front_treble(float value) {
  this->send_command("TFRT" + tone_value_to_code_(value));
}
void OnkyoIscp::set_subwoofer_level(float value) {
  const std::string code = level_value_to_code_(value, -15, 12);

  this->send_command("SWL" + code);
  this->enqueue_command_("SWLQSTN");
}

void OnkyoIscp::set_center_level(float value) {
  const std::string code = level_value_to_code_(value, -12, 12);

  this->send_command("CTL" + code);
  this->enqueue_command_("CTLQSTN");
}

void OnkyoIscp::set_volume(float raw_value) {
  int value = std::max(0, std::min(100, static_cast<int>(raw_value + 0.5f)));
  char command[8];
  std::snprintf(command, sizeof(command), "MVL%02X", value);
  this->send_command(command);
}
void OnkyoIscp::process_front_tone_(const std::string& value) {
  bool parsed_any_value = false;

  const size_t bass_position = value.find('B');

  if (bass_position != std::string::npos && bass_position + 3 <= value.size()) {
    const std::string bass_code = value.substr(bass_position + 1, 2);

    float bass_value;

    if (tone_code_to_value_(bass_code, bass_value)) {
      parsed_any_value = true;

      if (front_bass_number_ != nullptr) {
        front_bass_number_->publish_state(bass_value);
      }
    } else {
      ESP_LOGW(TAG, "Invalid front bass value: %s", bass_code.c_str());
    }
  }

  const size_t treble_position = value.find('T');

  if (treble_position != std::string::npos &&
      treble_position + 3 <= value.size()) {
    const std::string treble_code = value.substr(treble_position + 1, 2);

    float treble_value;

    if (tone_code_to_value_(treble_code, treble_value)) {
      parsed_any_value = true;

      if (front_treble_number_ != nullptr) {
        front_treble_number_->publish_state(treble_value);
      }
    } else {
      ESP_LOGW(TAG, "Invalid front treble value: %s", treble_code.c_str());
    }
  }

  if (!parsed_any_value) {
    ESP_LOGW(TAG, "Invalid TFR response: %s", value.c_str());
  }
}

void OnkyoIscp::process_subwoofer_level_(const std::string& value) {
  float level;

  if (!level_code_to_value_(value, level)) {
    ESP_LOGW(TAG, "Invalid subwoofer level response: %s", value.c_str());
    return;
  }

  if (level < -15.0f || level > 12.0f) {
    ESP_LOGW(TAG, "Subwoofer level is out of range: %.0f", level);
    return;
  }

  if (subwoofer_level_number_ != nullptr) {
    subwoofer_level_number_->publish_state(level);
  }
}

void OnkyoIscp::process_center_level_(const std::string& value) {
  float level;

  if (!level_code_to_value_(value, level)) {
    ESP_LOGW(TAG, "Invalid center level response: %s", value.c_str());
    return;
  }

  if (level < -12.0f || level > 12.0f) {
    ESP_LOGW(TAG, "Center level is out of range: %.0f", level);
    return;
  }

  if (center_level_number_ != nullptr) {
    center_level_number_->publish_state(level);
  }
}

void OnkyoIscp::set_input(const std::string& input) {
  const std::string code = input_name_to_code_(input);
  if (code.empty()) {
    ESP_LOGW(TAG, "Unknown input option: %s", input.c_str());
    return;
  }
  this->send_command("SLI" + code);
}

void OnkyoIscp::set_listening_mode(const std::string& mode) {
  const std::string code = listening_mode_name_to_code_(mode);
  if (code.empty()) {
    ESP_LOGW(TAG, "Unknown listening mode option: %s", mode.c_str());
    return;
  }
  this->send_command("LMD" + code);
}

void OnkyoIscp::store_current_preset() {
  if (tuner_band_ == TunerBand::UNKNOWN) {
    ESP_LOGW(TAG, "Cannot store preset while no tuner input is active");
    return;
  }

  if (current_tuner_preset_ < 1 || current_tuner_preset_ > 40) {
    ESP_LOGW(TAG, "Cannot store preset because no valid preset is selected");
    return;
  }

  char command[8];
  std::snprintf(
      command,
      sizeof(command),
      "PRM%02X",
      current_tuner_preset_
  );

  ESP_LOGI(
      TAG,
      "Storing current tuner frequency in preset %u",
      current_tuner_preset_
  );

  this->send_command(command);
  this->enqueue_command_("PRSQSTN");
  this->enqueue_command_("TUNQSTN");
}

void OnkyoIscp::show_rds_radio_text() {
  if (tuner_band_ != TunerBand::FM) {
    ESP_LOGW(TAG, "RDS Radio Text is only available on FM");
    return;
  }

  this->send_command("RDS00");
}

void OnkyoIscp::show_rds_pty() {
  if (tuner_band_ != TunerBand::FM) {
    ESP_LOGW(TAG, "RDS PTY information is only available on FM");
    return;
  }

  this->send_command("RDS01");
}

void OnkyoIscp::show_rds_tp() {
  if (tuner_band_ != TunerBand::FM) {
    ESP_LOGW(TAG, "RDS TP information is only available on FM");
    return;
  }

  this->send_command("RDS02");
}

void OnkyoIscp::show_next_rds_information() {
  if (tuner_band_ != TunerBand::FM) {
    ESP_LOGW(TAG, "RDS information is only available on FM");
    return;
  }

  this->send_command("RDSUP");
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

bool OnkyoIscp::is_valid_ascii_frame_(const std::string& frame) {
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
    ESP_LOGW(TAG, "Discarding non-ASCII UART frame (%u bytes)",
             static_cast<unsigned int>(frame.size()));
    return;
  }
  frame = normalize_frame_(std::move(frame));
  if (frame.size() < 3) {
    ESP_LOGV(TAG, "Discarding invalid ISCP frame");
    return;
  }
  const std::string command = frame.substr(0, 3);
  for (const unsigned char character : command) {
    const bool valid = (character >= 'A' && character <= 'Z') ||
                       (character >= '0' && character <= '9');

    if (!valid) {
      ESP_LOGW(TAG, "Discarding frame with invalid command");
      return;
    }
  }
  this->mark_receiver_online_();
  ESP_LOGD(TAG, "RX: %s", frame.c_str());
  this->status_clear_warning();
  if (last_frame_sensor_ != nullptr) {
    last_frame_sensor_->publish_state(frame);
  }
  this->process_command_(command, frame.substr(3));
}

void OnkyoIscp::process_command_(const std::string& command,
                                 const std::string& value) {
  if (command == "PWR" && power_switch_ != nullptr) {
    if (value == "01")
      power_switch_->publish_state(true);
    else if (value == "00")
      power_switch_->publish_state(false);
  } else if (command == "AMT" && mute_switch_ != nullptr) {
    if (value == "01")
      mute_switch_->publish_state(true);
    else if (value == "00")
      mute_switch_->publish_state(false);
  } else if (command == "MVL" && volume_number_ != nullptr) {
    char* end = nullptr;
    const long raw = std::strtol(value.c_str(), &end, 16);
    if (end != value.c_str())
      volume_number_->publish_state(static_cast<float>(raw));
  } else if (command == "SLI") {
    if (value == "24") {
      tuner_band_ = TunerBand::FM;
    } else if (value == "25") {
      tuner_band_ = TunerBand::AM;
    } else {
      tuner_band_ = TunerBand::UNKNOWN;
    }

    if (input_select_ != nullptr) {
      const std::string name = input_code_to_name_(value);

      if (!name.empty()) {
        input_select_->publish_state(name);
      }
    }

    if (value == "24" || value == "25" || value == "26") {
      this->enqueue_command_("TUNQSTN");
      this->enqueue_command_("PRSQSTN");
    }
  } else if (command == "LMD" && listening_mode_select_ != nullptr) {
    const std::string name = listening_mode_code_to_name_(value);
    if (!name.empty()) {
      listening_mode_select_->publish_state(name);
    } else {
      ESP_LOGW(TAG, "Unknown listening mode code: %s", value.c_str());
    }
  } else if (command == "TFR") {
    this->process_front_tone_(value);
  } else if (command == "SWL") {
    this->process_subwoofer_level_(value);

  } else if (command == "CTL") {
    this->process_center_level_(value);
  } else if (command == "ADY" && audyssey_switch_ != nullptr) {
    if (value == "00") {
      audyssey_switch_->publish_state(false);
    } else if (value == "01") {
      audyssey_switch_->publish_state(true);
    } else {
      ESP_LOGW(TAG, "Unknown Audyssey state: %s", value.c_str());
    }

  } else if (command == "ADQ" && dynamic_eq_switch_ != nullptr) {
    if (value == "00") {
      dynamic_eq_switch_->publish_state(false);
    } else if (value == "01") {
      dynamic_eq_switch_->publish_state(true);
    } else {
      ESP_LOGW(TAG, "Unknown Dynamic EQ state: %s", value.c_str());
    }

  } else if (command == "ADV" && dynamic_volume_select_ != nullptr) {
    const std::string name = dynamic_volume_code_to_name_(value);

    if (!name.empty()) {
      dynamic_volume_select_->publish_state(name);
    } else {
      ESP_LOGW(TAG, "Unknown Dynamic Volume state: %s", value.c_str());
    }
  } else if (command == "RAS" && re_eq_switch_ != nullptr) {
    if (value == "00") {
      re_eq_switch_->publish_state(false);
    } else if (value == "01") {
      re_eq_switch_->publish_state(true);
    } else {
      ESP_LOGW(TAG, "Unknown Re-EQ state: %s", value.c_str());
    }

  } else if (command == "MOT" && music_optimizer_switch_ != nullptr) {
    if (value == "00") {
      music_optimizer_switch_->publish_state(false);
    } else if (value == "01") {
      music_optimizer_switch_->publish_state(true);
    } else {
      ESP_LOGW(TAG, "Unknown Music Optimizer state: %s", value.c_str());
    }

  } else if (command == "LTN" && late_night_select_ != nullptr) {
    const std::string name = late_night_code_to_name_(value);

    if (!name.empty()) {
      late_night_select_->publish_state(name);
    } else {
      ESP_LOGW(TAG, "Unknown Late Night state: %s", value.c_str());
    }

  } else if (command == "DIM" && dimmer_select_ != nullptr) {
    const std::string name = dimmer_code_to_name_(value);

    if (!name.empty()) {
      dimmer_select_->publish_state(name);
    } else {
      ESP_LOGW(TAG, "Unknown dimmer state: %s", value.c_str());
    }

  } else if (command == "SLP") {
    this->process_sleep_timer_(value);

  } else if (command == "SLA" && audio_selector_select_ != nullptr) {
    const std::string name = audio_selector_code_to_name_(value);

    if (!name.empty()) {
      audio_selector_select_->publish_state(name);
    } else {
      ESP_LOGW(TAG, "Unknown audio selector state: %s", value.c_str());
    }

  } else if (command == "SPL" && speaker_layout_select_ != nullptr) {
    const std::string name = speaker_layout_code_to_name_(value);

    if (!name.empty()) {
      speaker_layout_select_->publish_state(name);
    } else {
      ESP_LOGW(TAG, "Unknown speaker layout state: %s", value.c_str());
    }
  } else if (command == "TUN" || command == "TUZ") {
  this->process_tuner_frequency_(value);

  } else if (command == "PRS" || command == "PRZ") {
  this->process_tuner_preset_(value);
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

std::string OnkyoIscp::input_code_to_name_(const std::string& code) {
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

std::string OnkyoIscp::input_name_to_code_(const std::string& name) {
  static const char* const options[][2] = {
      {"VCR/DVR", "00"}, {"CBL/SAT", "01"},  {"GAME/TV", "02"},
      {"AUX1", "03"},    {"AUX2", "04"},     {"PC", "05"},
      {"BD/DVD", "10"},  {"TAPE", "20"},     {"PHONO", "22"},
      {"TV/CD", "23"},   {"FM", "24"},       {"AM", "25"},
      {"TUNER", "26"},   {"MULTI CH", "30"}, {"UNIVERSAL PORT", "40"},
      {"HDMI 5", "55"},  {"HDMI 6", "56"},   {"HDMI 7", "57"},
  };
  for (const auto& option : options)
    if (name == option[0]) return option[1];
  return {};
}

std::string OnkyoIscp::listening_mode_code_to_name_(const std::string& code) {
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

std::string OnkyoIscp::listening_mode_name_to_code_(const std::string& name) {
  static const char* const options[][2] = {
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
  for (const auto& option : options)
    if (name == option[0]) return option[1];
  return {};
}

void OnkyoPowerSwitch::write_state(bool state) {
  if (parent_) parent_->set_power(state);
}
void OnkyoMuteSwitch::write_state(bool state) {
  if (parent_) parent_->set_mute(state);
}
void OnkyoVolumeNumber::control(float value) {
  if (parent_) parent_->set_volume(value);
}
void OnkyoFrontBassNumber::control(float value) {
  if (parent_ != nullptr) {
    parent_->set_front_bass(value);
  }
}
void OnkyoFrontTrebleNumber::control(float value) {
  if (parent_ != nullptr) {
    parent_->set_front_treble(value);
  }
}
void OnkyoSubwooferLevelNumber::control(float value) {
  if (parent_ != nullptr) {
    parent_->set_subwoofer_level(value);
  }
}
void OnkyoCenterLevelNumber::control(float value) {
  if (parent_ != nullptr) {
    parent_->set_center_level(value);
  }
}
void OnkyoInputSelect::control(const std::string& value) {
  if (parent_) parent_->set_input(value);
}
void OnkyoVolumeUpButton::press_action() {
  if (parent_) parent_->send_command("MVLUP");
}
void OnkyoVolumeDownButton::press_action() {
  if (parent_) parent_->send_command("MVLDOWN");
}
void OnkyoQueryAllButton::press_action() {
  if (parent_) parent_->query_all();
}
void OnkyoListeningModeSelect::control(const std::string& value) {
  if (parent_) parent_->set_listening_mode(value);
}
void OnkyoAudysseySwitch::write_state(bool state) {
  if (parent_ != nullptr) {
    parent_->set_audyssey(state);
  }
}
void OnkyoDynamicEqSwitch::write_state(bool state) {
  if (parent_ != nullptr) {
    parent_->set_dynamic_eq(state);
  }
}
void OnkyoDynamicVolumeSelect::control(const std::string& value) {
  if (parent_ != nullptr) {
    parent_->set_dynamic_volume(value);
  }
}
void OnkyoReEqSwitch::write_state(bool state) {
  if (parent_ != nullptr) {
    parent_->set_re_eq(state);
  }
}
void OnkyoMusicOptimizerSwitch::write_state(bool state) {
  if (parent_ != nullptr) {
    parent_->set_music_optimizer(state);
  }
}
void OnkyoLateNightSelect::control(const std::string& value) {
  if (parent_ != nullptr) {
    parent_->set_late_night(value);
  }
}
void OnkyoDimmerSelect::control(const std::string& value) {
  if (parent_ != nullptr) {
    parent_->set_dimmer(value);
  }
}
void OnkyoSleepTimerNumber::control(float value) {
  if (parent_ != nullptr) {
    parent_->set_sleep_timer(value);
  }
}
void OnkyoAudioSelectorSelect::control(const std::string& value) {
  if (parent_ != nullptr) {
    parent_->set_audio_selector(value);
  }
}
void OnkyoSpeakerLayoutSelect::control(const std::string& value) {
  if (parent_ != nullptr) {
    parent_->set_speaker_layout(value);
  }
}
void OnkyoFmFrequencyNumber::control(float value) {
  if (parent_ != nullptr) {
    parent_->set_fm_frequency(value);
  }
}

void OnkyoAmFrequencyNumber::control(float value) {
  if (parent_ != nullptr) {
    parent_->set_am_frequency(value);
  }
}

void OnkyoTunerPresetNumber::control(float value) {
  if (parent_ != nullptr) {
    parent_->set_tuner_preset(value);
  }
}

void OnkyoPresetUpButton::press_action() {
  if (parent_ != nullptr) {
    parent_->send_command("PRSUP");
  }
}

void OnkyoPresetDownButton::press_action() {
  if (parent_ != nullptr) {
    parent_->send_command("PRSDOWN");
  }
}

void OnkyoPresetStoreButton::press_action() {
  if (parent_ != nullptr) {
    parent_->store_current_preset();
  }
}

void OnkyoRdsRadioTextButton::press_action() {
  if (parent_ != nullptr) {
    parent_->show_rds_radio_text();
  }
}

void OnkyoRdsPtyButton::press_action() {
  if (parent_ != nullptr) {
    parent_->show_rds_pty();
  }
}

void OnkyoRdsTpButton::press_action() {
  if (parent_ != nullptr) {
    parent_->show_rds_tp();
  }
}

void OnkyoRdsNextButton::press_action() {
  if (parent_ != nullptr) {
    parent_->show_next_rds_information();
  }
}
}  // namespace esphome::onkyo_iscp
