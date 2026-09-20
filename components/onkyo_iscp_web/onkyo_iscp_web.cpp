#include "onkyo_iscp_web.h"

#include "esphome/core/log.h"

namespace esphome::onkyo_iscp_web {

static const char *const TAG = "onkyo_iscp_web";

void OnkyoIscpWeb::setup() {
  ESP_LOGI(TAG, "Starting ESPUI proof of concept");

  ESPUI.label(
      "Status",
      ControlColor::Turquoise,
      "ESPUI proof of concept 0.2a is running"
  );
  power_control_id_ = ESPUI.switcher(
      "Power",
      [this](Control *, int type) { this->handle_power_(type); },
      ControlColor::Emerald,
      false
  );
  mute_control_id_ = ESPUI.switcher(
      "Mute",
      [this](Control *, int type) { this->handle_mute_(type); },
      ControlColor::Alizarin,
      false
  );
  ESPUI.begin(title_.c_str());
}

void OnkyoIscpWeb::dump_config() {
  ESP_LOGCONFIG(TAG, "Onkyo ISCP ESPUI proof of concept:");
  ESP_LOGCONFIG(TAG, "  Title: %s", title_.c_str());
  ESP_LOGCONFIG(TAG, "  Backend: %s", parent_ != nullptr ? "configured" : "missing");
  ESP_LOGCONFIG(TAG, "  Power control ID: %u", power_control_id_);
  ESP_LOGCONFIG(TAG, "  Mute control ID: %u", mute_control_id_);
}

void OnkyoIscpWeb::handle_power_(int type) {
  if (parent_ == nullptr) {
    ESP_LOGW(TAG, "Cannot change power because the Onkyo backend is missing");
    return;
  }
  if (type != S_ACTIVE && type != S_INACTIVE) return;

  const bool state = type == S_ACTIVE;
  ESP_LOGD(TAG, "ESPUI power request: %s", state ? "ON" : "OFF");
  parent_->set_power(state);
}

void OnkyoIscpWeb::handle_mute_(int type) {
  if (parent_ == nullptr) {
    ESP_LOGW(TAG, "Cannot change mute because the Onkyo backend is missing");
    return;
  }
  if (type != S_ACTIVE && type != S_INACTIVE) return;

  const bool state = type == S_ACTIVE;
  ESP_LOGD(TAG, "ESPUI mute request: %s", state ? "ON" : "OFF");
  parent_->set_mute(state);
}

}  // namespace esphome::onkyo_iscp_web
