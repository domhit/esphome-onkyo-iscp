#pragma once

#include <string>

#include <ESPUI.h>

#include "esphome/core/component.h"
#include "esphome/components/onkyo_iscp/onkyo_iscp.h"

namespace esphome::onkyo_iscp_web {

class OnkyoIscpWeb : public Component, public onkyo_iscp::OnkyoIscpStateListener {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }

  void set_parent(onkyo_iscp::OnkyoIscp *parent) { parent_ = parent; }
  void set_title(const std::string &title) { title_ = title; }

  void on_power_state(bool state) override;
  void on_mute_state(bool state) override;
  void on_volume_state(float absolute, float relative) override;

 protected:
  void handle_power_(int type);
  void handle_mute_(int type);
  void handle_volume_(Control *sender, int type);

  onkyo_iscp::OnkyoIscp *parent_{nullptr};
  std::string title_{"Onkyo TX-SR608"};
  uint16_t power_control_id_{0};
  uint16_t mute_control_id_{0};
  uint16_t volume_control_id_{0};
  uint16_t volume_label_id_{0};
  uint32_t sync_due_ms_{0};
  bool initial_sync_pending_{false};
};

}  // namespace esphome::onkyo_iscp_web
