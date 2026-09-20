#pragma once

#include <string>
#include <ESPUI.h>

#include "esphome/components/onkyo_iscp/onkyo_iscp.h"
#include "esphome/core/component.h"

namespace esphome::onkyo_iscp_web {

class OnkyoIscpWeb : public Component, public onkyo_iscp::OnkyoIscpStateListener {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }

  void set_parent(onkyo_iscp::OnkyoIscp *parent) { parent_ = parent; }
  void set_title(const std::string &title) { title_ = title; }

  void on_receiver_online_state(bool state) override {}
  void on_power_state(bool state) override;
  void on_mute_state(bool state) override {}
  void on_volume_state(float absolute, float relative) override;
  void on_input_state(const std::string &input) override;
  void on_listening_mode_state(const std::string &mode) override {}

 protected:
  void handle_power_(int type);
  void handle_volume_(Control *sender, int type);
  void handle_input_(Control *sender, int type);
  void handle_osd_(const char *command, int type);
  bool can_update_web_() const;
  void mark_web_update_();

  onkyo_iscp::OnkyoIscp *parent_{nullptr};
  std::string title_{"Onkyo TX-SR608"};
  uint16_t power_control_id_{0};
  uint16_t volume_label_id_{0};
  uint16_t volume_control_id_{0};
  uint16_t input_control_id_{0};
  uint32_t sync_due_ms_{0};
  uint32_t last_web_update_ms_{0};
  bool initial_sync_pending_{false};
  bool power_state_{false};
  int volume_absolute_{0};
  int volume_relative_{-82};
  std::string input_state_;
  bool power_update_pending_{false};
  bool volume_update_pending_{false};
  bool input_update_pending_{false};

  static constexpr uint32_t INITIAL_SYNC_DELAY_MS = 3000;
  static constexpr uint32_t WEB_UPDATE_GAP_MS = 50;
};

}  // namespace esphome::onkyo_iscp_web
