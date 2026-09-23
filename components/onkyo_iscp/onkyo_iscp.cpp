// SPDX-License-Identifier: GPL-3.0-only
// Copyright (c) 2026 Dominic Hitschel

#include "onkyo_iscp.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "esphome/core/hal.h"
#include "esphome/core/log.h"

namespace esphome::onkyo_iscp {

static const char* const TAG = "onkyo_iscp";
namespace {

struct ValueMap {
  const char *code;
  const char *name;
};

template<size_t N>
std::string find_name(const ValueMap (&map)[N], const std::string &code) {
  for (const auto &entry : map) {
    if (code == entry.code) return entry.name;
  }
  return {};
}

template<size_t N>
std::string find_code(const ValueMap (&map)[N], const std::string &name) {
  for (const auto &entry : map) {
    if (name == entry.name) return entry.code;
  }
  return {};
}

const ValueMap DYNAMIC_VOLUME_MAP[] = {
    {"00", "Off"}, {"01", "Light"}, {"02", "Medium"}, {"03", "Heavy"},
};

const ValueMap AUDIO_SELECTOR_MAP[] = {
    {"02", "Analog"}, {"04", "HDMI"}, {"05", "Coax/Optical"}, {"07", "ARC"},
};

const ValueMap SPEAKER_LAYOUT_MAP[] = {
    {"SB", "Surround Back"}, {"FH", "Front High"}, {"FW", "Front Wide"},
};

const ValueMap LATE_NIGHT_MAP[] = {
    {"00", "Off"}, {"01", "Low"}, {"02", "High"}, {"03", "Auto"},
};

const ValueMap DIMMER_MAP[] = {
    {"00", "Bright"}, {"01", "Dim"}, {"02", "Dark"},
};

const ValueMap HDMI_AUDIO_OUT_MAP[] = {
    {"00", "Off"}, {"01", "On"}, {"02", "Auto"},
};

const ValueMap MONITOR_RESOLUTION_MAP[] = {
    {"00", "Through"}, {"01", "Auto"}, {"02", "480p"},
    {"03", "720p"}, {"04", "1080i"}, {"05", "1080p"},
};

const ValueMap VIDEO_WIDE_MODE_MAP[] = {
    {"00", "Auto"}, {"01", "4:3"}, {"02", "Full"},
    {"03", "Zoom"}, {"04", "Wide Zoom"},
};

const ValueMap PICTURE_MODE_MAP[] = {
    {"00", "Through"}, {"01", "Custom"}, {"02", "Cinema"}, {"03", "Game"},
};

const ValueMap DISPLAY_MODE_MAP[] = {
    {"00", "Selector + Volume"},
    {"01", "Selector + Listening Mode"},
};

const ValueMap INPUT_MAP[] = {
    {"00", "VCR/DVR"}, {"01", "CBL/SAT"}, {"02", "GAME/TV"},
    {"03", "AUX1"}, {"04", "AUX2"}, {"05", "PC"},
    {"10", "BD/DVD"}, {"20", "TAPE"}, {"22", "PHONO"},
    {"23", "TV/CD"}, {"24", "FM"}, {"25", "AM"},
    {"26", "TUNER"}, {"30", "MULTI CH"}, {"40", "UNIVERSAL PORT"},
    {"55", "HDMI 5"}, {"56", "HDMI 6"}, {"57", "HDMI 7"},
};

const ValueMap LISTENING_MODE_MAP[] = {
    {"00", "Stereo"}, {"01", "Direct"}, {"02", "Surround"},
    {"03", "Game RPG"}, {"05", "Game Action"}, {"06", "Game Rock"},
    {"08", "Orchestra"}, {"09", "Unplugged"}, {"0A", "Studio-Mix"},
    {"0B", "TV Logic"}, {"0C", "All Ch Stereo"}, {"0D", "Theater-Dimensional"},
    {"0E", "Game Sports"}, {"0F", "Mono"}, {"13", "Full Mono"},
    {"16", "Audyssey DSX"}, {"40", "Straight Decode"}, {"42", "THX Cinema"},
    {"43", "THX Surround EX"}, {"44", "THX Music"}, {"45", "THX Games"},
    {"50", "THX Cinema 2"}, {"51", "THX Music Mode"}, {"52", "THX Games Mode"},
    {"80", "PLII/PLIIx Movie"}, {"81", "PLII/PLIIx Music"}, {"82", "Neo:6 Cinema"},
    {"83", "Neo:6 Music"}, {"84", "PLII/PLIIx THX Cinema"},
    {"85", "Neo:6 THX Cinema"}, {"86", "PLII/PLIIx Game"},
    {"89", "PLII/PLIIx THX Games"}, {"8A", "Neo:6 THX Games"},
    {"8B", "PLII/PLIIx THX Music"}, {"8C", "Neo:6 THX Music"},
    {"90", "PLIIz Height"}, {"94", "PLIIz Height + THX Cinema"},
    {"95", "PLIIz Height + THX Music"}, {"96", "PLIIz Height + THX Games"},
    {"A0", "PLII/PLIIx Movie + Audyssey DSX"},
    {"A1", "PLII/PLIIx Music + Audyssey DSX"},
    {"A2", "PLII/PLIIx Game + Audyssey DSX"},
    {"A3", "Neo:6 Cinema + Audyssey DSX"},
    {"A4", "Neo:6 Music + Audyssey DSX"},
};

const char *const PTY_NAMES[] = {
    "None", "News", "Affairs", "Info", "Sport", "Educate", "Drama",
    "Culture", "Science", "Varied", "Pop M", "Rock M", "Easy M",
    "Light M", "Classics", "Other M", "Weather", "Finance", "Children",
    "Social", "Religion", "Phone In", "Travel", "Leisure", "Jazz",
    "Country", "Nation M", "Oldies", "Folk M", "Document", "TEST", "Alarm",
};

}  // namespace

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

  if (audio_information_sensor_ != nullptr) {
    audio_information_sensor_->publish_state("unknown");
  }

  if (video_information_sensor_ != nullptr) {
    video_information_sensor_->publish_state("unknown");
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
void OnkyoIscp::send_command(const std::string &command) {
  this->enqueue_command_(command);
}

void OnkyoIscp::transmit_command_(const std::string &command) {
  if (command.empty()) return;
  std::string frame = command;
  if (frame.rfind("!1", 0) != 0) frame.insert(0, "!1");
  if (frame.back() != '\r') frame.push_back('\r');
  this->write_str(frame.c_str());
  ESP_LOGD(TAG, "TX: %s", frame.c_str());
}

void OnkyoIscp::enqueue_command_(const std::string &command) {
  if (command.empty()) {
    ESP_LOGW(TAG, "Ignoring empty ISCP command");
    return;
  }
  if (command_queue_.size() >= MAX_COMMAND_QUEUE_SIZE) {
    ESP_LOGW(TAG, "ISCP command queue full, dropping command: %s", command.c_str());
    return;
  }
  command_queue_.push_back(command);
}

void OnkyoIscp::process_queue_() {
  if (command_queue_.empty()) return;
  const uint32_t now = millis();
  if (now - last_command_ms_ < COMMAND_GAP_MS) return;
  this->transmit_command_(command_queue_.front());
  command_queue_.pop_front();
  last_command_ms_ = now;
}
void OnkyoIscp::query_all() {
  last_full_query_ms_ = millis();
  this->query_core_();
  this->query_audio_();
  this->query_display_();
  this->query_routing_();
  this->query_video_information();
}

void OnkyoIscp::query_core_() {
  this->enqueue_command_("PWRQSTN");
  this->enqueue_command_("MVLQSTN");
  this->enqueue_command_("AMTQSTN");
  this->enqueue_command_("SLIQSTN");
}

void OnkyoIscp::query_audio_() {
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
}

void OnkyoIscp::query_display_() {
  this->enqueue_command_("DIFQSTN");
  this->enqueue_command_("DIMQSTN");
  this->enqueue_command_("SLPQSTN");
}

void OnkyoIscp::query_routing_() {
  this->enqueue_command_("SLAQSTN");
  this->enqueue_command_("SPLQSTN");
}
void OnkyoIscp::query_tuner() {
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
  if (tuner_band_ == TunerBand::UNKNOWN) {
    ESP_LOGW(TAG, "Cannot select preset while no tuner input is active");
    return;
  }

  const int value = static_cast<int>(std::lround(preset));

  if (value == 0) {
    ESP_LOGD(TAG, "Preset 0 represents manual tuning and cannot be selected");
    this->enqueue_command_("PRSQSTN");
    return;
  }

  if (value < 1 || value > 40) {
    ESP_LOGW(TAG, "Tuner preset out of range: %d", value);
    this->enqueue_command_("PRSQSTN");
    return;
  }

  current_tuner_preset_ = static_cast<uint8_t>(value);

  char command[8];
  std::snprintf(command, sizeof(command), "PRS%02X", value);

  this->enqueue_command_(command);
  this->enqueue_command_("PRSQSTN");
  this->enqueue_command_("TUNQSTN");
}
void OnkyoIscp::tuner_preset_up() {
  if (tuner_band_ == TunerBand::UNKNOWN) {
    ESP_LOGW(TAG, "Cannot select next preset while no tuner band is active");
    return;
  }

  this->enqueue_command_("PRSUP");
  this->enqueue_command_("PRSQSTN");
  this->enqueue_command_("TUNQSTN");
}
void OnkyoIscp::tuner_preset_down() {
  if (tuner_band_ == TunerBand::UNKNOWN) {
    ESP_LOGW(TAG, "Cannot select previous preset while no tuner band is active");
    return;
  }

  this->enqueue_command_("PRSDOWN");
  this->enqueue_command_("PRSQSTN");
  this->enqueue_command_("TUNQSTN");
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

  if (end == value.c_str() || *end != '\0' || preset < 0 || preset > 40) {
    ESP_LOGW(TAG, "Invalid tuner preset: %s", value.c_str());
    return;
  }

  current_tuner_preset_ = static_cast<uint8_t>(preset);

  if (preset == 0) {
    ESP_LOGD(TAG, "No tuner preset active, receiver is manually tuned");

    if (tuner_preset_number_ != nullptr) {
      tuner_preset_number_->publish_state(0.0f);
    }

    return;
  }

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
  const int raw =
      std::max(0, std::min(100, static_cast<int>(std::lround(raw_value))));
  char command[8];
  std::snprintf(command, sizeof(command), "MVL%02X", raw);
  this->send_command(command);
}
void OnkyoIscp::set_relative_volume(float db_value) {
  const int db =
      std::max(-82, std::min(18, static_cast<int>(std::lround(db_value))));
  this->set_volume(static_cast<float>(db + 82));
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
void OnkyoIscp::add_input_source(const std::string &code, const std::string &name) {
  input_sources_.push_back({code, name});
}
void OnkyoIscp::set_input(const std::string& input) {
  const std::string code = configured_input_name_to_code_(input);
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
void OnkyoIscp::set_pty(const std::string &pty) {
  if (tuner_band_ != TunerBand::FM) {
    ESP_LOGW(TAG, "PTY selection is only available on FM");
    return;
  }

  const std::string code = pty_name_to_code_(pty);

  if (code.empty()) {
    ESP_LOGW(TAG, "Unknown PTY option: %s", pty.c_str());
    return;
  }

  this->send_command("PTS" + code);
}
void OnkyoIscp::start_pty_scan() {
  if (tuner_band_ != TunerBand::FM) {
    ESP_LOGW(TAG, "PTY scan is only available on FM");
    return;
  }

  this->send_command("PTSSCAN");
}
void OnkyoIscp::start_tp_scan() {
  if (tuner_band_ != TunerBand::FM) {
    ESP_LOGW(TAG, "TP scan is only available on FM");
    return;
  }

  this->send_command("TPSSCAN");
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
void OnkyoIscp::query_video_information() {
  this->enqueue_command_("HAOQSTN");
  this->enqueue_command_("RESQSTN");
  this->enqueue_command_("VWMQSTN");
  this->enqueue_command_("VPMQSTN");
  this->enqueue_command_("IFAQSTN");
  this->enqueue_command_("IFVQSTN");
}
void OnkyoIscp::set_hdmi_audio_out(const std::string &mode) {
  const std::string code = hdmi_audio_out_name_to_code_(mode);

  if (code.empty()) {
    ESP_LOGW(TAG, "Unknown HDMI Audio Out option: %s", mode.c_str());
    return;
  }

  this->enqueue_command_("HAO" + code);
  this->enqueue_command_("HAOQSTN");
}
void OnkyoIscp::set_monitor_resolution(const std::string &resolution) {
  const std::string code = monitor_resolution_name_to_code_(resolution);

  if (code.empty()) {
    ESP_LOGW(TAG, "Unknown monitor resolution option: %s", resolution.c_str());
    return;
  }

  this->enqueue_command_("RES" + code);
  this->enqueue_command_("RESQSTN");
}
void OnkyoIscp::set_video_wide_mode(const std::string &mode) {
  const std::string code = video_wide_mode_name_to_code_(mode);

  if (code.empty()) {
    ESP_LOGW(TAG, "Unknown Video Wide Mode option: %s", mode.c_str());
    return;
  }

  this->enqueue_command_("VWM" + code);
  this->enqueue_command_("VWMQSTN");
}
void OnkyoIscp::set_picture_mode(const std::string &mode) {
  const std::string code = picture_mode_name_to_code_(mode);

  if (code.empty()) {
    ESP_LOGW(TAG, "Unknown Picture Mode option: %s", mode.c_str());
    return;
  }

  this->enqueue_command_("VPM" + code);
  this->enqueue_command_("VPMQSTN");
}
void OnkyoIscp::process_audio_information_(const std::string &value) {
  if (value.empty()) {
    ESP_LOGW(TAG, "Empty audio information response");
    return;
  }
  const std::vector<std::string> fields = split_information_fields_(value);
  ESP_LOGD(TAG, "Parsed IFA response with %u fields", static_cast<unsigned int>(fields.size()));
  if (audio_source_sensor_ != nullptr) audio_source_sensor_->publish_state(information_field_(fields, 0));
  if (audio_input_format_sensor_ != nullptr) audio_input_format_sensor_->publish_state(information_field_(fields, 1));
  if (audio_sample_rate_sensor_ != nullptr) audio_sample_rate_sensor_->publish_state(information_field_(fields, 2));
  if (audio_input_channels_sensor_ != nullptr) audio_input_channels_sensor_->publish_state(information_field_(fields, 3));
  if (audio_output_format_sensor_ != nullptr) audio_output_format_sensor_->publish_state(information_field_(fields, 4));
}
void OnkyoIscp::process_video_information_(const std::string &value) {
  if (value.empty()) {
    ESP_LOGW(TAG, "Empty video information response");
    return;
  }
  const std::vector<std::string> fields = split_information_fields_(value);
  ESP_LOGD(TAG, "Parsed IFV response with %u fields", static_cast<unsigned int>(fields.size()));
  if (video_input_sensor_ != nullptr) video_input_sensor_->publish_state(information_field_(fields, 0));
  if (video_input_resolution_sensor_ != nullptr) video_input_resolution_sensor_->publish_state(information_field_(fields, 1));
  if (video_input_color_space_sensor_ != nullptr) video_input_color_space_sensor_->publish_state(information_field_(fields, 2));
  if (video_input_color_depth_sensor_ != nullptr) video_input_color_depth_sensor_->publish_state(information_field_(fields, 3));
  if (video_output_sensor_ != nullptr) video_output_sensor_->publish_state(information_field_(fields, 4));
  if (video_output_resolution_sensor_ != nullptr) video_output_resolution_sensor_->publish_state(information_field_(fields, 5));
  if (video_output_color_space_sensor_ != nullptr) video_output_color_space_sensor_->publish_state(information_field_(fields, 6));
  if (video_output_color_depth_sensor_ != nullptr) video_output_color_depth_sensor_->publish_state(information_field_(fields, 7));
}
void OnkyoIscp::send_osd_command(const std::string &command) {
  if (!receiver_online_) {
    ESP_LOGW(TAG, "Cannot send OSD command while receiver is offline");
    return;
  }

  static const char *const valid_commands[] = {
      "MENU",
      "UP",
      "DOWN",
      "LEFT",
      "RIGHT",
      "ENTER",
      "EXIT",
      "HOME",
      "QUICK",
      "AUDIO",
      "VIDEO",
  };

  bool valid = false;

  for (const char *valid_command : valid_commands) {
    if (command == valid_command) {
      valid = true;
      break;
    }
  }

  if (!valid) {
    ESP_LOGW(TAG, "Invalid OSD command: %s", command.c_str());
    return;
  }

  ESP_LOGD(TAG, "Sending OSD command: %s", command.c_str());
  this->enqueue_command_("OSD" + command);
}
void OnkyoIscp::set_display_mode(const std::string &mode) {
  const std::string code = display_mode_name_to_code_(mode);

  if (code.empty()) {
    ESP_LOGW(TAG, "Unknown display mode option: %s", mode.c_str());
    return;
  }

  this->enqueue_command_("DIF" + code);
  this->enqueue_command_("DIFQSTN");
}
void OnkyoIscp::display_audio_format() {
  if (!receiver_online_) {
    ESP_LOGW(TAG, "Cannot display audio format while receiver is offline");
    return;
  }

  this->enqueue_command_("DIF02");
  this->enqueue_command_("IFAQSTN");
}
void OnkyoIscp::display_video_format() {
  if (!receiver_online_) {
    ESP_LOGW(TAG, "Cannot display video format while receiver is offline");
    return;
  }

  this->enqueue_command_("DIF03");
  this->enqueue_command_("IFVQSTN");
}
void OnkyoIscp::display_mode_next() {
  if (!receiver_online_) {
    ESP_LOGW(TAG, "Cannot change display mode while receiver is offline");
    return;
  }

  this->enqueue_command_("DIFTG");
  this->enqueue_command_("DIFQSTN");
}
void OnkyoIscp::send_speaker_level_calibration_command(const std::string &command) {
  if (!receiver_online_) {
    ESP_LOGW(TAG, "Cannot send speaker level calibration command while receiver is offline");
    return;
  }
  if (command != "TEST" && command != "CHSEL" && command != "UP" &&
      command != "DOWN") {
    ESP_LOGW(TAG, "Invalid speaker level calibration command: %s", command.c_str());
    return;
  }
  this->enqueue_command_("SLC" + command);
}
// Dispatcher ##########################################################################################
void OnkyoIscp::process_command_(const std::string &command, const std::string &value) {
  if (this->process_core_command_(command, value)) return;
  if (this->process_audio_command_(command, value)) return;
  if (this->process_tuner_command_(command, value)) return;
  if (this->process_video_command_(command, value)) return;
  if (this->process_auxiliary_command_(command, value)) return;
  this->process_unknown_command_(command, value);
}

bool OnkyoIscp::process_core_command_(const std::string &command, const std::string &value) {
  if (command == "PWR") {
    if (value == "01") {
      const bool power_changed = !receiver_power_on_;
      receiver_power_on_ = true;
      if (power_switch_ != nullptr) power_switch_->publish_state(true);
      if (power_changed) {
        ESP_LOGI(TAG, "Receiver powered on");
        const uint32_t now = millis();
        if (now - last_full_query_ms_ > 1000) {
          ESP_LOGI(TAG, "Starting full state synchronization after power on");
          this->query_all();
        }
      }
    } else if (value == "00") {
      if (receiver_power_on_) ESP_LOGI(TAG, "Receiver entered standby");
      receiver_power_on_ = false;
      if (power_switch_ != nullptr) power_switch_->publish_state(false);
    } else {
      ESP_LOGW(TAG, "Unknown power state: %s", value.c_str());
    }
    return true;
  }

  if (command == "AMT") {
    if (value == "01") {
      if (mute_switch_ != nullptr) mute_switch_->publish_state(true);
    } else if (value == "00") {
      if (mute_switch_ != nullptr) mute_switch_->publish_state(false);
    } else {
      ESP_LOGW(TAG, "Unknown mute state: %s", value.c_str());
    }
    return true;
  }

  if (command == "MVL") {
    char *end = nullptr;
    const long raw = std::strtol(value.c_str(), &end, 16);
    if (end == value.c_str() || *end != '\0' || raw < 0 || raw > 100) {
      ESP_LOGW(TAG, "Invalid master volume response: %s", value.c_str());
      return true;
    }
    if (volume_number_ != nullptr) volume_number_->publish_state(static_cast<float>(raw));
    if (relative_volume_number_ != nullptr)
      relative_volume_number_->publish_state(static_cast<float>(raw - 82));
    return true;
  }

  if (command == "SLI") {
    if (value == "24") tuner_band_ = TunerBand::FM;
    else if (value == "25") tuner_band_ = TunerBand::AM;
    else tuner_band_ = TunerBand::UNKNOWN;

    if (input_select_ != nullptr) {
      const std::string name = configured_input_code_to_name_(value);
      if (!name.empty()) input_select_->publish_state(name);
      else if (!input_code_to_name_(value).empty())
        ESP_LOGD(TAG, "Input code %s is known but not enabled", value.c_str());
      else ESP_LOGW(TAG, "Unknown input code: %s", value.c_str());
    }
    if (value == "24" || value == "25" || value == "26") this->query_tuner();
    return true;
  }

  if (command == "DIM") {
    const std::string name = dimmer_code_to_name_(value);
    if (!name.empty()) {
      if (dimmer_select_ != nullptr) dimmer_select_->publish_state(name);
    } else {
      ESP_LOGW(TAG, "Unknown dimmer state: %s", value.c_str());
    }
    return true;
  }

  if (command == "SLP") {
    this->process_sleep_timer_(value);
    return true;
  }

  if (command == "DIF") {
    if (value == "02") ESP_LOGD(TAG, "Temporary audio format display requested");
    else if (value == "03") ESP_LOGD(TAG, "Temporary video format display requested");
    else if (value == "TG") ESP_LOGD(TAG, "Display mode advanced");
    else if (value == "N/A") ESP_LOGD(TAG, "Display mode is not available in the current context");
    else {
      const std::string name = display_mode_code_to_name_(value);
      if (!name.empty()) {
        if (display_mode_select_ != nullptr) display_mode_select_->publish_state(name);
      } else {
        ESP_LOGW(TAG, "Unknown display mode response: %s", value.c_str());
      }
    }
    return true;
  }

  if (command == "FLD") {
    if (display_sensor_ != nullptr) display_sensor_->publish_state(value);
    return true;
  }

  return false;
}

bool OnkyoIscp::process_audio_command_(const std::string &command, const std::string &value) {
  if (command == "LMD") {
    const std::string name = listening_mode_code_to_name_(value);
    if (!name.empty()) {
      if (listening_mode_select_ != nullptr) listening_mode_select_->publish_state(name);
    } else ESP_LOGW(TAG, "Unknown listening mode code: %s", value.c_str());
    return true;
  }
  if (command == "TFR") { this->process_front_tone_(value); return true; }
  if (command == "SWL") { this->process_subwoofer_level_(value); return true; }
  if (command == "CTL") { this->process_center_level_(value); return true; }

  if (command == "ADY") {
    if (value == "00") { if (audyssey_switch_ != nullptr) audyssey_switch_->publish_state(false); }
    else if (value == "01") { if (audyssey_switch_ != nullptr) audyssey_switch_->publish_state(true); }
    else ESP_LOGW(TAG, "Unknown Audyssey state: %s", value.c_str());
    return true;
  }

  if (command == "ADQ") {
    if (value == "00") { if (dynamic_eq_switch_ != nullptr) dynamic_eq_switch_->publish_state(false); }
    else if (value == "01") { if (dynamic_eq_switch_ != nullptr) dynamic_eq_switch_->publish_state(true); }
    else ESP_LOGW(TAG, "Unknown Dynamic EQ state: %s", value.c_str());
    return true;
  }

  if (command == "ADV") {
    const std::string name = dynamic_volume_code_to_name_(value);
    if (!name.empty()) { if (dynamic_volume_select_ != nullptr) dynamic_volume_select_->publish_state(name); }
    else ESP_LOGW(TAG, "Unknown Dynamic Volume state: %s", value.c_str());
    return true;
  }

  if (command == "RAS") {
    if (value == "00") { if (re_eq_switch_ != nullptr) re_eq_switch_->publish_state(false); }
    else if (value == "01") { if (re_eq_switch_ != nullptr) re_eq_switch_->publish_state(true); }
    else ESP_LOGW(TAG, "Unknown Re-EQ state: %s", value.c_str());
    return true;
  }

  if (command == "MOT") {
    if (value == "00") { if (music_optimizer_switch_ != nullptr) music_optimizer_switch_->publish_state(false); }
    else if (value == "01") { if (music_optimizer_switch_ != nullptr) music_optimizer_switch_->publish_state(true); }
    else ESP_LOGW(TAG, "Unknown Music Optimizer state: %s", value.c_str());
    return true;
  }

  if (command == "LTN") {
    const std::string name = late_night_code_to_name_(value);
    if (!name.empty()) { if (late_night_select_ != nullptr) late_night_select_->publish_state(name); }
    else ESP_LOGW(TAG, "Unknown Late Night state: %s", value.c_str());
    return true;
  }

  if (command == "SLA") {
    const std::string name = audio_selector_code_to_name_(value);
    if (!name.empty()) { if (audio_selector_select_ != nullptr) audio_selector_select_->publish_state(name); }
    else ESP_LOGW(TAG, "Unknown audio selector state: %s", value.c_str());
    return true;
  }

  if (command == "SPL") {
    const std::string name = speaker_layout_code_to_name_(value);
    if (!name.empty()) { if (speaker_layout_select_ != nullptr) speaker_layout_select_->publish_state(name); }
    else ESP_LOGW(TAG, "Unknown speaker layout state: %s", value.c_str());
    return true;
  }

  return false;
}

bool OnkyoIscp::process_tuner_command_(const std::string &command, const std::string &value) {
  if (command == "TUN" || command == "TUZ") { this->process_tuner_frequency_(value); return true; }
  if (command == "PRS" || command == "PRZ") { this->process_tuner_preset_(value); return true; }

  if (command == "RDS") {
    if (value == "00") ESP_LOGD(TAG, "RDS display mode: Radio Text");
    else if (value == "01") ESP_LOGD(TAG, "RDS display mode: PTY");
    else if (value == "02") ESP_LOGD(TAG, "RDS display mode: TP");
    else if (value == "UP") ESP_LOGD(TAG, "RDS display mode advanced");
    else ESP_LOGW(TAG, "Unknown RDS response: %s", value.c_str());
    return true;
  }

  if (command == "PTS") {
    if (value == "N/A") ESP_LOGD(TAG, "PTY scan is not available for the current station");
    else if (value == "SCAN") ESP_LOGD(TAG, "PTY scan started");
    else {
      const std::string name = pty_code_to_name_(value);
      if (!name.empty()) {
        if (pty_select_ != nullptr) pty_select_->publish_state(name);
        ESP_LOGD(TAG, "PTY state: %s", name.c_str());
      } else ESP_LOGW(TAG, "Unknown PTY response: %s", value.c_str());
    }
    return true;
  }

  if (command == "TPS") {
    if (value == "N/A") ESP_LOGD(TAG, "TP scan is not available for the current station");
    else if (value == "SCAN") ESP_LOGD(TAG, "TP scan started");
    else ESP_LOGD(TAG, "TP scan response: %s", value.c_str());
    return true;
  }

  return false;
}

bool OnkyoIscp::process_video_command_(const std::string &command, const std::string &value) {
  if (command == "HAO") {
    const std::string name = hdmi_audio_out_code_to_name_(value);
    if (!name.empty()) { if (hdmi_audio_out_select_ != nullptr) hdmi_audio_out_select_->publish_state(name); }
    else ESP_LOGW(TAG, "Unknown HDMI Audio Out state: %s", value.c_str());
    return true;
  }

  if (command == "RES") {
    const std::string name = monitor_resolution_code_to_name_(value);
    if (!name.empty()) { if (monitor_resolution_select_ != nullptr) monitor_resolution_select_->publish_state(name); }
    else ESP_LOGW(TAG, "Unknown monitor resolution state: %s", value.c_str());
    return true;
  }

  if (command == "VWM") {
    const std::string name = video_wide_mode_code_to_name_(value);
    if (!name.empty()) { if (video_wide_mode_select_ != nullptr) video_wide_mode_select_->publish_state(name); }
    else ESP_LOGW(TAG, "Unknown Video Wide Mode state: %s", value.c_str());
    return true;
  }

  if (command == "VPM") {
    const std::string name = picture_mode_code_to_name_(value);
    if (!name.empty()) { if (picture_mode_select_ != nullptr) picture_mode_select_->publish_state(name); }
    else ESP_LOGW(TAG, "Unknown Picture Mode state: %s", value.c_str());
    return true;
  }

  if (command == "IFA") {
    if (value.empty()) ESP_LOGW(TAG, "Empty audio information response");
    else {
      if (audio_information_sensor_ != nullptr) audio_information_sensor_->publish_state(value);
      this->process_audio_information_(value);
    }
    return true;
  }

  if (command == "IFV") {
    if (value.empty()) ESP_LOGW(TAG, "Empty video information response");
    else {
      if (video_information_sensor_ != nullptr) video_information_sensor_->publish_state(value);
      this->process_video_information_(value);
    }
    return true;
  }

  return false;
}

bool OnkyoIscp::process_auxiliary_command_(const std::string &command, const std::string &value) {
  if (command == "TST") {
    if (value == "N/A") ESP_LOGD(TAG, "TST function is not available");
    else ESP_LOGD(TAG, "TST response: %s", value.c_str());
    return true;
  }
  if (command == "SLZ") {
    ESP_LOGD(TAG, "Zone 2 input selector notification: %s", value.c_str());
    return true;
  }
  if (command == "OSD") {
    if (value == "N/A") ESP_LOGD(TAG, "OSD command is not available in the current context");
    else ESP_LOGD(TAG, "OSD response: %s", value.c_str());
    return true;
  }
  return false;
}

void OnkyoIscp::process_unknown_command_(const std::string &command, const std::string &value) {
  const std::string unknown_frame = command + value;
  ESP_LOGD(TAG, "Unhandled ISCP frame: %s", unknown_frame.c_str());
  if (last_unknown_frame_sensor_ != nullptr) last_unknown_frame_sensor_->publish_state(unknown_frame);
}
// Mappings ###############################################################################################################################
std::string OnkyoIscp::dynamic_volume_code_to_name_(const std::string &code) {
  return find_name(DYNAMIC_VOLUME_MAP, code);
}
std::string OnkyoIscp::dynamic_volume_name_to_code_(const std::string &name) {
  return find_code(DYNAMIC_VOLUME_MAP, name);
}
std::string OnkyoIscp::audio_selector_code_to_name_(const std::string &code) {
  return find_name(AUDIO_SELECTOR_MAP, code);
}
std::string OnkyoIscp::audio_selector_name_to_code_(const std::string &name) {
  return find_code(AUDIO_SELECTOR_MAP, name);
}
std::string OnkyoIscp::speaker_layout_code_to_name_(const std::string &code) {
  return find_name(SPEAKER_LAYOUT_MAP, code);
}
std::string OnkyoIscp::speaker_layout_name_to_code_(const std::string &name) {
  return find_code(SPEAKER_LAYOUT_MAP, name);
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
std::string OnkyoIscp::input_code_to_name_(const std::string &code) {
  return find_name(INPUT_MAP, code);
}
std::string OnkyoIscp::configured_input_code_to_name_(const std::string &code) const {
  for (const auto &source : input_sources_) {
    if (source.code == code) return source.name;
  }
  return {};
}
std::string OnkyoIscp::configured_input_name_to_code_(const std::string &name) const {
  for (const auto &source : input_sources_) {
    if (source.name == name) return source.code;
  }
  return {};
}
std::string OnkyoIscp::listening_mode_code_to_name_(const std::string &code) {
  return find_name(LISTENING_MODE_MAP, code);
}
std::string OnkyoIscp::listening_mode_name_to_code_(const std::string &name) {
  return find_code(LISTENING_MODE_MAP, name);
}
std::string OnkyoIscp::normalize_frame_(std::string frame) {
  const size_t start = frame.rfind("!1");
  if (start == std::string::npos) {
    return {};
  }
  frame.erase(0, start + 2);
  return frame;
}
std::string OnkyoIscp::pty_code_to_name_(const std::string &code) {
  char *end = nullptr;
  const long value = std::strtol(code.c_str(), &end, 16);
  if (end == code.c_str() || *end != '\0' || value < 0 || value > 31) return {};
  return PTY_NAMES[value];
}
std::string OnkyoIscp::pty_name_to_code_(const std::string &name) {
  for (size_t index = 0; index < 32; index++) {
    if (name != PTY_NAMES[index]) continue;
    char code[3];
    std::snprintf(code, sizeof(code), "%02X", static_cast<unsigned int>(index));
    return code;
  }
  return {};
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
std::string OnkyoIscp::late_night_code_to_name_(const std::string &code) {
  return find_name(LATE_NIGHT_MAP, code);
}
std::string OnkyoIscp::late_night_name_to_code_(const std::string &name) {
  return find_code(LATE_NIGHT_MAP, name);
}
std::string OnkyoIscp::level_value_to_code_(float value, int minimum, int maximum) {
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
std::string OnkyoIscp::dimmer_code_to_name_(const std::string &code) {
  return find_name(DIMMER_MAP, code);
}
std::string OnkyoIscp::dimmer_name_to_code_(const std::string &name) {
  return find_code(DIMMER_MAP, name);
}
std::string OnkyoIscp::hdmi_audio_out_code_to_name_(const std::string &code) {
  return find_name(HDMI_AUDIO_OUT_MAP, code);
}
std::string OnkyoIscp::hdmi_audio_out_name_to_code_(const std::string &name) {
  return find_code(HDMI_AUDIO_OUT_MAP, name);
}
std::string OnkyoIscp::monitor_resolution_code_to_name_(const std::string &code) {
  return find_name(MONITOR_RESOLUTION_MAP, code);
}
std::string OnkyoIscp::monitor_resolution_name_to_code_(const std::string &name) {
  return find_code(MONITOR_RESOLUTION_MAP, name);
}
std::string OnkyoIscp::video_wide_mode_code_to_name_(const std::string &code) {
  return find_name(VIDEO_WIDE_MODE_MAP, code);
}
std::string OnkyoIscp::video_wide_mode_name_to_code_(const std::string &name) {
  return find_code(VIDEO_WIDE_MODE_MAP, name);
}
std::string OnkyoIscp::picture_mode_code_to_name_(const std::string &code) {
  return find_name(PICTURE_MODE_MAP, code);
}
std::string OnkyoIscp::picture_mode_name_to_code_(const std::string &name) {
  return find_code(PICTURE_MODE_MAP, name);
}
std::vector<std::string> OnkyoIscp::split_information_fields_(const std::string& value) {
  std::vector<std::string> fields;
  size_t start = 0;

  while (start <= value.size()) {
    const size_t separator = value.find(',', start);

    if (separator == std::string::npos) {
      fields.push_back(value.substr(start));
      break;
    }

    fields.push_back(value.substr(start, separator - start));
    start = separator + 1;
  }

  return fields;
}
std::string OnkyoIscp::information_field_(const std::vector<std::string> &fields, size_t index) {
  if (index >= fields.size()) {
    return "N/A";
  }

  std::string value = fields[index];

  const size_t first = value.find_first_not_of(" \t");
  if (first == std::string::npos) {
    return "N/A";
  }

  const size_t last = value.find_last_not_of(" \t");
  value = value.substr(first, last - first + 1);

  return value.empty() ? "N/A" : value;
}
std::string OnkyoIscp::display_mode_code_to_name_(const std::string &code) {
  return find_name(DISPLAY_MODE_MAP, code);
}
std::string OnkyoIscp::display_mode_name_to_code_(const std::string &name) {
  return find_code(DISPLAY_MODE_MAP, name);
}
// Control classes  ###############################################################################################################################
void OnkyoPowerSwitch::write_state(bool state) {
  if (parent_) parent_->set_power(state);
}
void OnkyoMuteSwitch::write_state(bool state) {
  if (parent_) parent_->set_mute(state);
}
void OnkyoVolumeNumber::control(float value) {
  if (parent_) parent_->set_volume(value);
}
void OnkyoRelativeVolumeNumber::control(float value) {
  if (parent_) parent_->set_relative_volume(value);
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
    parent_->tuner_preset_up();
  }
}
void OnkyoPresetDownButton::press_action() {
  if (parent_ != nullptr) {
    parent_->tuner_preset_down();
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
void OnkyoPtySelect::control(const std::string &value) {
  if (parent_ != nullptr) {
    parent_->set_pty(value);
  }
}
void OnkyoPtyScanButton::press_action() {
  if (parent_ != nullptr) {
    parent_->start_pty_scan();
  }
}
void OnkyoTpScanButton::press_action() {
  if (parent_ != nullptr) {
    parent_->start_tp_scan();
  }
}
void OnkyoHdmiAudioOutSelect::control(const std::string &value) {
  if (parent_ != nullptr) {
    parent_->set_hdmi_audio_out(value);
  }
}
void OnkyoMonitorResolutionSelect::control(const std::string &value) {
  if (parent_ != nullptr) {
    parent_->set_monitor_resolution(value);
  }
}
void OnkyoVideoWideModeSelect::control(const std::string &value) {
  if (parent_ != nullptr) {
    parent_->set_video_wide_mode(value);
  }
}
void OnkyoPictureModeSelect::control(const std::string &value) {
  if (parent_ != nullptr) {
    parent_->set_picture_mode(value);
  }
}
void OnkyoOsdMenuButton::press_action() {
  if (parent_ != nullptr) parent_->send_osd_command("MENU");
}
void OnkyoOsdUpButton::press_action() {
  if (parent_ != nullptr) parent_->send_osd_command("UP");
}
void OnkyoOsdDownButton::press_action() {
  if (parent_ != nullptr) parent_->send_osd_command("DOWN");
}
void OnkyoOsdLeftButton::press_action() {
  if (parent_ != nullptr) parent_->send_osd_command("LEFT");
}
void OnkyoOsdRightButton::press_action() {
  if (parent_ != nullptr) parent_->send_osd_command("RIGHT");
}
void OnkyoOsdEnterButton::press_action() {
  if (parent_ != nullptr) parent_->send_osd_command("ENTER");
}
void OnkyoOsdExitButton::press_action() {
  if (parent_ != nullptr) parent_->send_osd_command("EXIT");
}
void OnkyoOsdHomeButton::press_action() {
  if (parent_ != nullptr) parent_->send_osd_command("HOME");
}
void OnkyoOsdQuickButton::press_action() {
  if (parent_ != nullptr) parent_->send_osd_command("QUICK");
}
void OnkyoOsdAudioButton::press_action() {
  if (parent_ != nullptr) parent_->send_osd_command("AUDIO");
}
void OnkyoOsdVideoButton::press_action() {
  if (parent_ != nullptr) parent_->send_osd_command("VIDEO");
}
void OnkyoDisplayModeSelect::control(const std::string &value) {
  if (parent_ != nullptr) {
    parent_->set_display_mode(value);
  }
}
void OnkyoDisplayAudioFormatButton::press_action() {
  if (parent_ != nullptr) {
    parent_->display_audio_format();
  }
}
void OnkyoDisplayVideoFormatButton::press_action() {
  if (parent_ != nullptr) {
    parent_->display_video_format();
  }
}
void OnkyoDisplayModeNextButton::press_action() {
  if (parent_ != nullptr) {
    parent_->display_mode_next();
  }
}
void OnkyoSpeakerLevelTestButton::press_action() {
  if (parent_ != nullptr)
    parent_->send_speaker_level_calibration_command("TEST");
}
void OnkyoSpeakerLevelNextButton::press_action() {
  if (parent_ != nullptr)
    parent_->send_speaker_level_calibration_command("CHSEL");
}
void OnkyoSpeakerLevelUpButton::press_action() {
  if (parent_ != nullptr)
    parent_->send_speaker_level_calibration_command("UP");
}
void OnkyoSpeakerLevelDownButton::press_action() {
  if (parent_ != nullptr)
    parent_->send_speaker_level_calibration_command("DOWN");
}
}  // namespace esphome::onkyo_iscp
