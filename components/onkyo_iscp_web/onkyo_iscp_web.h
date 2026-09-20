#pragma once

#include <string>

#include <ESPUI.h>

#include "esphome/core/component.h"
#include "esphome/components/onkyo_iscp/onkyo_iscp.h"

namespace esphome::onkyo_iscp_web {

class OnkyoIscpWeb : public Component {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }

  void set_parent(onkyo_iscp::OnkyoIscp *parent) { parent_ = parent; }
  void set_title(const std::string &title) { title_ = title; }

 protected:
  onkyo_iscp::OnkyoIscp *parent_{nullptr};
  std::string title_{"Onkyo TX-SR608"};
};

}  // namespace esphome::onkyo_iscp_web
