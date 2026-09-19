#pragma once

#include <deque>
#include <string>
#include <vector>

#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/button/button.h"
#include "esphome/components/number/number.h"
#include "esphome/components/select/select.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/automation.h"
#include "esphome/core/component.h"

namespace esphome::onkyo_iscp {

class OnkyoIscp;

//Switches ####################################################################################
class OnkyoPowerSwitch : public switch_::Switch {
 public:
  void set_parent(OnkyoIscp* parent) { parent_ = parent; }

 protected:
  void write_state(bool state) override;
  OnkyoIscp* parent_{nullptr};
};
class OnkyoMuteSwitch : public switch_::Switch {
 public:
  void set_parent(OnkyoIscp* parent) { parent_ = parent; }

 protected:
  void write_state(bool state) override;
  OnkyoIscp* parent_{nullptr};
};
class OnkyoAudysseySwitch : public switch_::Switch {
 public:
  void set_parent(OnkyoIscp* parent) { parent_ = parent; }

 protected:
  void write_state(bool state) override;
  OnkyoIscp* parent_{nullptr};
};
class OnkyoDynamicEqSwitch : public switch_::Switch {
 public:
  void set_parent(OnkyoIscp* parent) { parent_ = parent; }

 protected:
  void write_state(bool state) override;
  OnkyoIscp* parent_{nullptr};
};
class OnkyoReEqSwitch : public switch_::Switch {
 public:
  void set_parent(OnkyoIscp* parent) { parent_ = parent; }

 protected:
  void write_state(bool state) override;
  OnkyoIscp* parent_{nullptr};
};
class OnkyoMusicOptimizerSwitch : public switch_::Switch {
 public:
  void set_parent(OnkyoIscp* parent) { parent_ = parent; }

 protected:
  void write_state(bool state) override;
  OnkyoIscp* parent_{nullptr};
};
//Numbers ####################################################################################
class OnkyoVolumeNumber : public number::Number {
 public:
  void set_parent(OnkyoIscp* parent) { parent_ = parent; }

 protected:
  void control(float value) override;
  OnkyoIscp* parent_{nullptr};
};
class OnkyoFrontBassNumber : public number::Number {
 public:
  void set_parent(OnkyoIscp* parent) { parent_ = parent; }

 protected:
  void control(float value) override;
  OnkyoIscp* parent_{nullptr};
};
class OnkyoFrontTrebleNumber : public number::Number {
 public:
  void set_parent(OnkyoIscp* parent) { parent_ = parent; }

 protected:
  void control(float value) override;
  OnkyoIscp* parent_{nullptr};
};
class OnkyoSubwooferLevelNumber : public number::Number {
 public:
  void set_parent(OnkyoIscp* parent) { parent_ = parent; }

 protected:
  void control(float value) override;
  OnkyoIscp* parent_{nullptr};
};
class OnkyoCenterLevelNumber : public number::Number {
 public:
  void set_parent(OnkyoIscp* parent) { parent_ = parent; }

 protected:
  void control(float value) override;
  OnkyoIscp* parent_{nullptr};
};
class OnkyoSleepTimerNumber : public number::Number {
 public:
  void set_parent(OnkyoIscp* parent) { parent_ = parent; }

 protected:
  void control(float value) override;
  OnkyoIscp* parent_{nullptr};
};
class OnkyoFmFrequencyNumber : public number::Number {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void control(float value) override;
  OnkyoIscp *parent_{nullptr};
};
class OnkyoAmFrequencyNumber : public number::Number {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void control(float value) override;
  OnkyoIscp *parent_{nullptr};
};
class OnkyoTunerPresetNumber : public number::Number {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void control(float value) override;
  OnkyoIscp *parent_{nullptr};
};
//Selects ####################################################################################
class OnkyoInputSelect : public select::Select {
 public:
  void set_parent(OnkyoIscp* parent) { parent_ = parent; }

 protected:
  void control(const std::string& value) override;
  OnkyoIscp* parent_{nullptr};
};
class OnkyoListeningModeSelect : public select::Select {
 public:
  void set_parent(OnkyoIscp* parent) { parent_ = parent; }

 protected:
  void control(const std::string& value) override;
  OnkyoIscp* parent_{nullptr};
};
class OnkyoDynamicVolumeSelect : public select::Select {
 public:
  void set_parent(OnkyoIscp* parent) { parent_ = parent; }

 protected:
  void control(const std::string& value) override;
  OnkyoIscp* parent_{nullptr};
};
class OnkyoLateNightSelect : public select::Select {
 public:
  void set_parent(OnkyoIscp* parent) { parent_ = parent; }

 protected:
  void control(const std::string& value) override;
  OnkyoIscp* parent_{nullptr};
};
class OnkyoDimmerSelect : public select::Select {
 public:
  void set_parent(OnkyoIscp* parent) { parent_ = parent; }

 protected:
  void control(const std::string& value) override;
  OnkyoIscp* parent_{nullptr};
};
class OnkyoAudioSelectorSelect : public select::Select {
 public:
  void set_parent(OnkyoIscp* parent) { parent_ = parent; }

 protected:
  void control(const std::string& value) override;
  OnkyoIscp* parent_{nullptr};
};
class OnkyoSpeakerLayoutSelect : public select::Select {
 public:
  void set_parent(OnkyoIscp* parent) { parent_ = parent; }

 protected:
  void control(const std::string& value) override;
  OnkyoIscp* parent_{nullptr};
};
class OnkyoPtySelect : public select::Select {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void control(const std::string &value) override;
  OnkyoIscp *parent_{nullptr};
};
class OnkyoHdmiAudioOutSelect : public select::Select {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void control(const std::string &value) override;
  OnkyoIscp *parent_{nullptr};
};
class OnkyoMonitorResolutionSelect : public select::Select {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void control(const std::string &value) override;
  OnkyoIscp *parent_{nullptr};
};
class OnkyoVideoWideModeSelect : public select::Select {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void control(const std::string &value) override;
  OnkyoIscp *parent_{nullptr};
};
class OnkyoPictureModeSelect : public select::Select {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void control(const std::string &value) override;
  OnkyoIscp *parent_{nullptr};
};
class OnkyoDisplayModeSelect : public select::Select {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void control(const std::string &value) override;
  OnkyoIscp *parent_{nullptr};
};

//Buttons ####################################################################################
class OnkyoVolumeUpButton : public button::Button {
 public:
  void set_parent(OnkyoIscp* parent) { parent_ = parent; }

 protected:
  void press_action() override;
  OnkyoIscp* parent_{nullptr};
};
class OnkyoVolumeDownButton : public button::Button {
 public:
  void set_parent(OnkyoIscp* parent) { parent_ = parent; }

 protected:
  void press_action() override;
  OnkyoIscp* parent_{nullptr};
};
class OnkyoQueryAllButton : public button::Button {
 public:
  void set_parent(OnkyoIscp* parent) { parent_ = parent; }

 protected:
  void press_action() override;
  OnkyoIscp* parent_{nullptr};
};
class OnkyoPresetUpButton : public button::Button {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void press_action() override;
  OnkyoIscp *parent_{nullptr};
};
class OnkyoPresetDownButton : public button::Button {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void press_action() override;
  OnkyoIscp *parent_{nullptr};
};
class OnkyoPresetStoreButton : public button::Button {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void press_action() override;
  OnkyoIscp *parent_{nullptr};
};
class OnkyoRdsRadioTextButton : public button::Button {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void press_action() override;
  OnkyoIscp *parent_{nullptr};
};
class OnkyoRdsPtyButton : public button::Button {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void press_action() override;
  OnkyoIscp *parent_{nullptr};
};
class OnkyoRdsTpButton : public button::Button {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void press_action() override;
  OnkyoIscp *parent_{nullptr};
};
class OnkyoRdsNextButton : public button::Button {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void press_action() override;
  OnkyoIscp *parent_{nullptr};
};
class OnkyoPtyScanButton : public button::Button {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void press_action() override;
  OnkyoIscp *parent_{nullptr};
};
class OnkyoTpScanButton : public button::Button {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void press_action() override;
  OnkyoIscp *parent_{nullptr};
};
class OnkyoOsdMenuButton : public button::Button {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void press_action() override;
  OnkyoIscp *parent_{nullptr};
};
class OnkyoOsdUpButton : public button::Button {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void press_action() override;
  OnkyoIscp *parent_{nullptr};
};
class OnkyoOsdDownButton : public button::Button {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void press_action() override;
  OnkyoIscp *parent_{nullptr};
};
class OnkyoOsdLeftButton : public button::Button {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void press_action() override;
  OnkyoIscp *parent_{nullptr};
};
class OnkyoOsdRightButton : public button::Button {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void press_action() override;
  OnkyoIscp *parent_{nullptr};
};
class OnkyoOsdEnterButton : public button::Button {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void press_action() override;
  OnkyoIscp *parent_{nullptr};
};
class OnkyoOsdExitButton : public button::Button {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void press_action() override;
  OnkyoIscp *parent_{nullptr};
};
class OnkyoOsdHomeButton : public button::Button {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void press_action() override;
  OnkyoIscp *parent_{nullptr};
};
class OnkyoOsdQuickButton : public button::Button {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void press_action() override;
  OnkyoIscp *parent_{nullptr};
};
class OnkyoOsdAudioButton : public button::Button {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void press_action() override;
  OnkyoIscp *parent_{nullptr};
};
class OnkyoOsdVideoButton : public button::Button {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void press_action() override;
  OnkyoIscp *parent_{nullptr};
};
class OnkyoDisplayAudioFormatButton : public button::Button {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void press_action() override;
  OnkyoIscp *parent_{nullptr};
};
class OnkyoDisplayVideoFormatButton : public button::Button {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void press_action() override;
  OnkyoIscp *parent_{nullptr};
};
class OnkyoDisplayModeNextButton : public button::Button {
 public:
  void set_parent(OnkyoIscp *parent) { parent_ = parent; }

 protected:
  void press_action() override;
  OnkyoIscp *parent_{nullptr};
};

//Main ####################################################################################
class OnkyoIscp : public PollingComponent, public uart::UARTDevice {
 public:
  void setup() override;
  void loop() override;
  void update() override;
  void dump_config() override;

  void send_command(const std::string& command);
  void query_all();
  void query_video_information();
  void set_power(bool state);
  void set_mute(bool state);
  void set_volume(float raw_value);
  void set_front_bass(float value);
  void set_front_treble(float value);
  void set_subwoofer_level(float value);
  void set_center_level(float value);
  void set_audyssey(bool state);
  void set_dynamic_eq(bool state);
  void set_re_eq(bool state);
  void set_music_optimizer(bool state);
  void set_dimmer(const std::string& mode);
  void set_sleep_timer(float minutes);
  void set_late_night(const std::string& mode);
  void set_dynamic_volume(const std::string& mode);
  void set_input(const std::string& input);
  void set_audio_selector(const std::string& selector);
  void set_speaker_layout(const std::string& layout);
  void set_listening_mode(const std::string& listening_mode);
  void set_fm_frequency(float frequency);
  void set_am_frequency(float frequency);
  void set_tuner_preset(float preset);
  void store_current_preset();
  void show_rds_radio_text();
  void show_rds_pty();
  void show_rds_tp();
  void show_next_rds_information();
  void set_pty(const std::string &pty);
  void start_pty_scan();
  void start_tp_scan();
  void query_tuner();
  void tuner_preset_up();
  void tuner_preset_down();
  void set_hdmi_audio_out(const std::string &mode);
  void set_monitor_resolution(const std::string &resolution);
  void set_video_wide_mode(const std::string &mode);
  void set_picture_mode(const std::string &mode);
  void send_osd_command(const std::string &command);
  void set_display_mode(const std::string &mode);
  void display_audio_format();
  void display_video_format();
  void display_mode_next();

  void set_connected_binary_sensor(binary_sensor::BinarySensor* entity) { connected_binary_sensor_ = entity; }
  void set_power_switch(OnkyoPowerSwitch* entity) { power_switch_ = entity; }
  void set_mute_switch(OnkyoMuteSwitch* entity) { mute_switch_ = entity; }
  void set_audyssey_switch(OnkyoAudysseySwitch* entity) { audyssey_switch_ = entity; }
  void set_dynamic_eq_switch(OnkyoDynamicEqSwitch* entity) { dynamic_eq_switch_ = entity; }
  void set_re_eq_switch(OnkyoReEqSwitch* entity) { re_eq_switch_ = entity; }
  void set_music_optimizer_switch(OnkyoMusicOptimizerSwitch* entity) { music_optimizer_switch_ = entity; }
  void set_dimmer_select(OnkyoDimmerSelect* entity) { dimmer_select_ = entity; }
  void set_sleep_timer_number(OnkyoSleepTimerNumber* entity) { sleep_timer_number_ = entity; }
  void set_late_night_select(OnkyoLateNightSelect* entity) { late_night_select_ = entity; }
  void set_dynamic_volume_select(OnkyoDynamicVolumeSelect* entity) { dynamic_volume_select_ = entity; }
  void set_volume_number(OnkyoVolumeNumber* entity) { volume_number_ = entity; }
  void set_front_bass_number(OnkyoFrontBassNumber* entity) { front_bass_number_ = entity; }
  void set_front_treble_number(OnkyoFrontTrebleNumber* entity) { front_treble_number_ = entity; }
  void set_subwoofer_level_number(OnkyoSubwooferLevelNumber* entity) { subwoofer_level_number_ = entity; }
  void set_center_level_number(OnkyoCenterLevelNumber* entity) { center_level_number_ = entity; }
  void set_input_select(OnkyoInputSelect* entity) { input_select_ = entity; }
  void set_last_frame_sensor(text_sensor::TextSensor* entity) { last_frame_sensor_ = entity; }
  void set_last_unknown_frame_sensor(text_sensor::TextSensor* entity) { last_unknown_frame_sensor_ = entity; }
  void set_display_sensor(text_sensor::TextSensor* entity) { display_sensor_ = entity; }
  void set_audio_information_sensor(text_sensor::TextSensor *entity) { audio_information_sensor_ = entity; }
  void set_video_information_sensor(text_sensor::TextSensor *entity) { video_information_sensor_ = entity; }
  void set_audio_input_format_sensor(text_sensor::TextSensor *entity) { audio_input_format_sensor_ = entity; }
  void set_audio_sample_rate_sensor(text_sensor::TextSensor *entity) { audio_sample_rate_sensor_ = entity; }
  void set_audio_input_channels_sensor(text_sensor::TextSensor *entity) { audio_input_channels_sensor_ = entity; }
  void set_audio_output_channels_sensor(text_sensor::TextSensor *entity) { audio_output_channels_sensor_ = entity; }
  void set_video_input_sensor(text_sensor::TextSensor *entity) { video_input_sensor_ = entity; }
  void set_video_input_resolution_sensor(text_sensor::TextSensor *entity) { video_input_resolution_sensor_ = entity; }
  void set_video_output_sensor(text_sensor::TextSensor *entity) { video_output_sensor_ = entity; }
  void set_video_output_resolution_sensor(text_sensor::TextSensor *entity) { video_output_resolution_sensor_ = entity; }
  void set_audio_selector_select(OnkyoAudioSelectorSelect* entity) { audio_selector_select_ = entity; }
  void set_speaker_layout_select(OnkyoSpeakerLayoutSelect* entity) { speaker_layout_select_ = entity; }
  void set_listening_mode_select(OnkyoListeningModeSelect* entity) { listening_mode_select_ = entity; }
  void set_fm_frequency_number(OnkyoFmFrequencyNumber *entity) { fm_frequency_number_ = entity; }
  void set_am_frequency_number(OnkyoAmFrequencyNumber *entity) { am_frequency_number_ = entity; }
  void set_tuner_preset_number(OnkyoTunerPresetNumber *entity) { tuner_preset_number_ = entity; }
  void set_pty_select(OnkyoPtySelect *entity) { pty_select_ = entity;}
  void set_hdmi_audio_out_select(OnkyoHdmiAudioOutSelect *entity) { hdmi_audio_out_select_ = entity; }
  void set_monitor_resolution_select(OnkyoMonitorResolutionSelect *entity) { monitor_resolution_select_ = entity; }
  void set_video_wide_mode_select(OnkyoVideoWideModeSelect *entity) { video_wide_mode_select_ = entity; }
  void set_picture_mode_select(OnkyoPictureModeSelect *entity) { picture_mode_select_ = entity; }
  void set_display_mode_select(OnkyoDisplayModeSelect *entity) { display_mode_select_ = entity; }

 protected:
  void read_uart_();
  void process_frame_(std::string frame);
  void process_command_(const std::string& command, const std::string& value);
  bool process_core_command_(const std::string &command, const std::string &value);
  bool process_audio_command_(const std::string &command, const std::string &value);
  bool process_tuner_command_(const std::string &command, const std::string &value);
  bool process_video_command_(const std::string &command, const std::string &value);
  bool process_auxiliary_command_(const std::string &command, const std::string &value);
  void process_unknown_command_(const std::string &command, const std::string &value);
  void query_core_();
  void query_audio_();
  void query_display_();
  void query_routing_();
  void enqueue_command_(const std::string &command);
  void process_queue_();
  void transmit_command_(const std::string &command);
  void mark_receiver_online_();
  void check_receiver_timeout_();
  void process_front_tone_(const std::string& value);
  void process_subwoofer_level_(const std::string& value);
  void process_center_level_(const std::string& value);
  void process_sleep_timer_(const std::string& value);
  void process_tuner_frequency_(const std::string &value);
  void process_tuner_preset_(const std::string &value);
  void process_audio_information_(const std::string &value);
  void process_video_information_(const std::string &value);

  static std::vector<std::string> split_information_fields_(const std::string &value);
  static std::string information_field_(const std::vector<std::string> &fields, size_t index);

  static std::string normalize_frame_(std::string frame);
  static std::string input_code_to_name_(const std::string& code);
  static std::string input_name_to_code_(const std::string& name);
  static std::string listening_mode_code_to_name_(const std::string& code);
  static std::string listening_mode_name_to_code_(const std::string& name);
  static std::string tone_value_to_code_(float value);
  static std::string dynamic_volume_code_to_name_(const std::string& code);
  static std::string dynamic_volume_name_to_code_(const std::string& name);
  static std::string late_night_code_to_name_(const std::string& code);
  static std::string late_night_name_to_code_(const std::string& name);
  static bool tone_code_to_value_(const std::string& code, float& value);
  static std::string level_value_to_code_(float value, int minimum, int maximum);
  static bool level_code_to_value_(const std::string& code, float& value);
  static std::string dimmer_code_to_name_(const std::string& code);
  static std::string dimmer_name_to_code_(const std::string& name);
  static std::string audio_selector_code_to_name_(const std::string& code);
  static std::string audio_selector_name_to_code_(const std::string& name);
  static std::string speaker_layout_code_to_name_(const std::string& code);
  static std::string speaker_layout_name_to_code_(const std::string& name);
  enum class TunerBand : uint8_t { UNKNOWN, FM, AM, };
  TunerBand tuner_band_{TunerBand::UNKNOWN};
  static std::string pty_code_to_name_(const std::string &code);
  static std::string pty_name_to_code_(const std::string &name);
  static std::string hdmi_audio_out_code_to_name_(const std::string &code);
  static std::string hdmi_audio_out_name_to_code_(const std::string &name);
  static std::string monitor_resolution_code_to_name_(const std::string &code);
  static std::string monitor_resolution_name_to_code_(const std::string &name);
  static std::string video_wide_mode_code_to_name_(const std::string &code);
  static std::string video_wide_mode_name_to_code_(const std::string &name);
  static std::string picture_mode_code_to_name_(const std::string &code);
  static std::string picture_mode_name_to_code_(const std::string &name);
  static std::string display_mode_code_to_name_( const std::string &code);
  static std::string display_mode_name_to_code_( const std::string &name);

  std::string rx_buffer_;
  std::deque<std::string> command_queue_;
  uint32_t last_command_ms_{0};
  static constexpr uint32_t COMMAND_GAP_MS = 100;
  static constexpr size_t MAX_COMMAND_QUEUE_SIZE = 64;
  static constexpr size_t MAX_FRAME_LENGTH = 160;
  uint32_t last_valid_frame_ms_{0};
  bool receiver_online_{false};
  bool receiver_power_on_{false};
  uint32_t last_full_query_ms_{0};
  static constexpr uint32_t RECEIVER_TIMEOUT_MS = 75000;
  static bool is_valid_ascii_frame_(const std::string& frame);
  uint8_t current_tuner_preset_{0};


  OnkyoPowerSwitch* power_switch_{nullptr};
  OnkyoMuteSwitch* mute_switch_{nullptr};
  OnkyoAudysseySwitch* audyssey_switch_{nullptr};
  OnkyoDynamicEqSwitch* dynamic_eq_switch_{nullptr};
  OnkyoReEqSwitch* re_eq_switch_{nullptr};
  OnkyoMusicOptimizerSwitch* music_optimizer_switch_{nullptr};
  OnkyoDynamicVolumeSelect* dynamic_volume_select_{nullptr};
  OnkyoVolumeNumber* volume_number_{nullptr};
  OnkyoFrontBassNumber* front_bass_number_{nullptr};
  OnkyoFrontTrebleNumber* front_treble_number_{nullptr};
  OnkyoSubwooferLevelNumber* subwoofer_level_number_{nullptr};
  OnkyoCenterLevelNumber* center_level_number_{nullptr};
  OnkyoSleepTimerNumber* sleep_timer_number_{nullptr};
  OnkyoInputSelect* input_select_{nullptr};
  OnkyoListeningModeSelect* listening_mode_select_{nullptr};
  OnkyoLateNightSelect* late_night_select_{nullptr};
  OnkyoDimmerSelect* dimmer_select_{nullptr};
  OnkyoAudioSelectorSelect* audio_selector_select_{nullptr};
  OnkyoSpeakerLayoutSelect* speaker_layout_select_{nullptr};
  OnkyoFmFrequencyNumber *fm_frequency_number_{nullptr};
  OnkyoAmFrequencyNumber *am_frequency_number_{nullptr};
  OnkyoTunerPresetNumber *tuner_preset_number_{nullptr};
  OnkyoPtySelect *pty_select_{nullptr};
  OnkyoHdmiAudioOutSelect *hdmi_audio_out_select_{nullptr};
  OnkyoMonitorResolutionSelect *monitor_resolution_select_{nullptr};
  OnkyoVideoWideModeSelect *video_wide_mode_select_{nullptr};
  OnkyoPictureModeSelect *picture_mode_select_{nullptr};
  OnkyoDisplayModeSelect *display_mode_select_{nullptr}; 
  text_sensor::TextSensor* last_frame_sensor_{nullptr};
  text_sensor::TextSensor* last_unknown_frame_sensor_{nullptr};
  text_sensor::TextSensor* display_sensor_{nullptr};
  binary_sensor::BinarySensor* connected_binary_sensor_{nullptr};
  text_sensor::TextSensor *audio_information_sensor_{nullptr};
  text_sensor::TextSensor *video_information_sensor_{nullptr};
  text_sensor::TextSensor *audio_input_format_sensor_{nullptr};
  text_sensor::TextSensor *audio_sample_rate_sensor_{nullptr};
  text_sensor::TextSensor *audio_input_channels_sensor_{nullptr};
  text_sensor::TextSensor *audio_output_channels_sensor_{nullptr};
  text_sensor::TextSensor *video_input_sensor_{nullptr};
  text_sensor::TextSensor *video_input_resolution_sensor_{nullptr};
  text_sensor::TextSensor *video_output_sensor_{nullptr};
  text_sensor::TextSensor *video_output_resolution_sensor_{nullptr};
};

template <typename... Ts>
class SendCommandAction final : public Action<Ts...> {
 public:
  explicit SendCommandAction(OnkyoIscp* parent) : parent_(parent) {}
  TEMPLATABLE_VALUE(std::string, command)
  void play(Ts... x) override {
    parent_->send_command(this->command_.value(x...));
  }

 protected:
  OnkyoIscp* parent_;
};

}  // namespace esphome::onkyo_iscp
