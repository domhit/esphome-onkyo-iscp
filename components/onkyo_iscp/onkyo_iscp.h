#pragma once

#include <deque>
#include <string>

#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/button/button.h"
#include "esphome/components/number/number.h"
#include "esphome/components/select/select.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/uart/uart.h"


namespace esphome::onkyo_iscp {

class OnkyoIscp;

class OnkyoPowerSwitch : public switch_::Switch {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }
 protected:
  void write_state(bool state) override;
  OnkyoIscp *parent_{nullptr};
};

class OnkyoMuteSwitch : public switch_::Switch {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }
 protected:
  void write_state(bool state) override;
  OnkyoIscp *parent_{nullptr};
};

class OnkyoAudysseySwitch : public switch_::Switch {
 public:
  void set_parent(OnkyoIscp *parent) {
    parent_ = parent;
  }

 protected:
  void write_state(bool state) override;

  OnkyoIscp *parent_{nullptr};
};


class OnkyoDynamicEqSwitch : public switch_::Switch {
 public:
  void set_parent(OnkyoIscp *parent) {
    parent_ = parent;
  }

 protected:
  void write_state(bool state) override;

  OnkyoIscp *parent_{nullptr};
};

class OnkyoVolumeNumber : public number::Number {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }
 protected:
  void control(float value) override;
  OnkyoIscp *parent_{nullptr};
};

class OnkyoFrontBassNumber : public number::Number {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }
 protected:
  void control(float value) override;
  OnkyoIscp *parent_{nullptr};
};


class OnkyoFrontTrebleNumber : public number::Number {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }
 protected:
  void control(float value) override;
  OnkyoIscp *parent_{nullptr};
};

class OnkyoSubwooferLevelNumber : public number::Number {
 public:
  void set_parent(OnkyoIscp *parent) {
    parent_ = parent;
  }

 protected:
  void control(float value) override;

  OnkyoIscp *parent_{nullptr};
};


class OnkyoCenterLevelNumber : public number::Number {
 public:
  void set_parent(OnkyoIscp *parent) {
    parent_ = parent;
  }

 protected:
  void control(float value) override;

  OnkyoIscp *parent_{nullptr};
};

class OnkyoInputSelect : public select::Select {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }
 protected:
  void control(const std::string &value) override;
  OnkyoIscp *parent_{nullptr};
};

class OnkyoListeningModeSelect : public select::Select {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }
 protected:
  void control(const std::string &value) override;
  OnkyoIscp *parent_{nullptr};
};

class OnkyoDynamicVolumeSelect : public select::Select {
 public:
  void set_parent(OnkyoIscp *parent) {
    parent_ = parent;
  }

 protected:
  void control(const std::string &value) override;

  OnkyoIscp *parent_{nullptr};
};

class OnkyoVolumeUpButton : public button::Button {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }
 protected:
  void press_action() override;
  OnkyoIscp *parent_{nullptr};
};

class OnkyoVolumeDownButton : public button::Button {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }
 protected:
  void press_action() override;
  OnkyoIscp *parent_{nullptr};
};

class OnkyoQueryAllButton : public button::Button {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }
 protected:
  void press_action() override;
  OnkyoIscp *parent_{nullptr};
};

class OnkyoIscp : public PollingComponent, public uart::UARTDevice {
 public:
  void setup() override;
  void loop() override;
  void update() override;
  void dump_config() override;

  void send_command(const std::string &command);
  void query_all();
  void set_power(bool state);
  void set_mute(bool state);
  void set_volume(float raw_value);
  void set_front_bass(float value);
  void set_front_treble(float value);
  void set_subwoofer_level(float value);
  void set_center_level(float value);
  void set_audyssey(bool state);
  void set_dynamic_eq(bool state);
  void set_dynamic_volume(const std::string &mode);
  void set_input(const std::string &input);
  void set_listening_mode(const std::string &listening_mode);
  void set_connected_binary_sensor(binary_sensor::BinarySensor *entity) { connected_binary_sensor_ = entity;}
  void set_power_switch(OnkyoPowerSwitch *entity) { power_switch_ = entity; }
  void set_mute_switch(OnkyoMuteSwitch *entity) { mute_switch_ = entity; }
  void set_audyssey_switch(OnkyoAudysseySwitch *entity) { audyssey_switch_ = entity; }
  void set_dynamic_eq_switch(OnkyoDynamicEqSwitch *entity) { dynamic_eq_switch_ = entity; }
  void set_dynamic_volume_select(OnkyoDynamicVolumeSelect *entity) { dynamic_volume_select_ = entity; }
  void set_volume_number(OnkyoVolumeNumber *entity) { volume_number_ = entity; }
  void set_front_bass_number(OnkyoFrontBassNumber *entity) { front_bass_number_ = entity; }
  void set_front_treble_number(OnkyoFrontTrebleNumber *entity) { front_treble_number_ = entity; }
  void set_subwoofer_level_number(OnkyoSubwooferLevelNumber *entity) { subwoofer_level_number_ = entity; }
  void set_center_level_number(OnkyoCenterLevelNumber *entity) { center_level_number_ = entity; }
  void set_input_select(OnkyoInputSelect *entity) { input_select_ = entity; }
  void set_last_frame_sensor(text_sensor::TextSensor *entity) { last_frame_sensor_ = entity; }
  void set_last_unknown_frame_sensor(text_sensor::TextSensor *entity) { last_unknown_frame_sensor_ = entity; }
  void set_display_sensor(text_sensor::TextSensor *entity) { display_sensor_ = entity; }
  void set_listening_mode_select(OnkyoListeningModeSelect *entity) { listening_mode_select_ = entity; }

 protected:
  void read_uart_();
  void process_frame_(std::string frame);
  void process_command_(const std::string &command, const std::string &value);
  void enqueue_command_(const std::string &command);
  void process_queue_();
  void mark_receiver_online_();
  void check_receiver_timeout_();
  void process_front_tone_(const std::string &value);
  void process_subwoofer_level_(const std::string &value);
  void process_center_level_(const std::string &value);

  static std::string normalize_frame_(std::string frame);
  static std::string input_code_to_name_(const std::string &code);
  static std::string input_name_to_code_(const std::string &name);
  static std::string listening_mode_code_to_name_(const std::string &code);
  static std::string listening_mode_name_to_code_(const std::string &name);
  static std::string tone_value_to_code_(float value);
  static std::string dynamic_volume_code_to_name_(const std::string &code);
  static std::string dynamic_volume_name_to_code_(const std::string &name);
  static bool tone_code_to_value_(const std::string &code, float &value);

  static std::string level_value_to_code_(float value, int minimum, int maximum);
  static bool level_code_to_value_(const std::string &code, float &value);

  std::string rx_buffer_;
  std::deque<std::string> command_queue_;
  uint32_t last_command_ms_{0};
  static constexpr uint32_t COMMAND_GAP_MS = 100;
  static constexpr size_t MAX_FRAME_LENGTH = 160;
  uint32_t last_valid_frame_ms_{0};
  bool receiver_online_{false};
  static constexpr uint32_t RECEIVER_TIMEOUT_MS = 75000;
  static bool is_valid_ascii_frame_(const std::string &frame);

  OnkyoPowerSwitch *power_switch_{nullptr};
  OnkyoMuteSwitch *mute_switch_{nullptr};
  OnkyoAudysseySwitch *audyssey_switch_{nullptr};
  OnkyoDynamicEqSwitch *dynamic_eq_switch_{nullptr};
  OnkyoDynamicVolumeSelect *dynamic_volume_select_{nullptr};
  OnkyoVolumeNumber *volume_number_{nullptr};
  OnkyoFrontBassNumber *front_bass_number_{nullptr};
  OnkyoFrontTrebleNumber *front_treble_number_{nullptr};
  OnkyoSubwooferLevelNumber *subwoofer_level_number_{nullptr};
  OnkyoCenterLevelNumber *center_level_number_{nullptr};
  OnkyoInputSelect *input_select_{nullptr};
  OnkyoListeningModeSelect *listening_mode_select_{nullptr};
  text_sensor::TextSensor *last_frame_sensor_{nullptr};
  text_sensor::TextSensor *last_unknown_frame_sensor_{nullptr};
  text_sensor::TextSensor *display_sensor_{nullptr};
  binary_sensor::BinarySensor *connected_binary_sensor_{nullptr};

};

template<typename... Ts> class SendCommandAction final : public Action<Ts...> {
 public:
  explicit SendCommandAction(OnkyoIscp *parent) : parent_(parent) {}
  TEMPLATABLE_VALUE(std::string, command)
  void play(Ts... x) override { parent_->send_command(this->command_.value(x...)); }
 protected:
  OnkyoIscp *parent_;
};

}  // namespace esphome::onkyo_iscp
