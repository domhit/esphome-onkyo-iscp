#include "onkyo_iscp_web.h"

#include <cstdio>

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
  volume_label_id_ = ESPUI.label("Volume", ControlColor::Turquoise, "0 / -82 dB");
  volume_control_id_ = ESPUI.slider(
      "Master Volume",
      [this](Control *sender, int type) { this->handle_volume_(sender, type); },
      ControlColor::Peterriver,
      0,
      0,
      100
  );
  ESPUI.captivePortal = false;
  ESPUI.begin(title_.c_str());
  if (parent_ != nullptr) {
    parent_->set_state_listener(this);
    sync_due_ms_ = millis() + 3000;
    initial_sync_pending_ = true;
  }
}

void OnkyoIscpWeb::loop() {
  if (!initial_sync_pending_ || parent_ == nullptr) return;
  if (static_cast<int32_t>(millis() - sync_due_ms_) < 0) return;

  initial_sync_pending_ = false;
  ESP_LOGD(TAG, "Starting delayed Onkyo state synchronization");
  parent_->query_all();
}

void OnkyoIscpWeb::dump_config() {
  ESP_LOGCONFIG(TAG, "Onkyo ISCP ESPUI proof of concept:");
  ESP_LOGCONFIG(TAG, "  Title: %s", title_.c_str());
  ESP_LOGCONFIG(TAG, "  Backend: %s", parent_ != nullptr ? "configured" : "missing");
  ESP_LOGCONFIG(TAG, "  Power control ID: %u", power_control_id_);
  ESP_LOGCONFIG(TAG, "  Mute control ID: %u", mute_control_id_);
  ESP_LOGCONFIG(TAG, "  Volume control ID: %u", volume_control_id_);
}

void OnkyoIscpWeb::on_power_state(bool state) {
  ESPUI.updateSwitcher(power_control_id_, state);
}

void OnkyoIscpWeb::on_mute_state(bool state) {
  ESPUI.updateSwitcher(mute_control_id_, state);
}

void OnkyoIscpWeb::on_volume_state(float absolute, float relative) {
  ESPUI.updateSlider(volume_control_id_, static_cast<int>(absolute));
  char label[24];
  std::snprintf(label, sizeof(label), "%.0f / %+.0f dB", absolute, relative);
  ESPUI.updateLabel(volume_label_id_, label);
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

void OnkyoIscpWeb::handle_volume_(Control *sender, int type) {
  if (parent_ == nullptr) {
    ESP_LOGW(TAG, "Cannot change volume because the Onkyo backend is missing");
    return;
  }
  if (type != SL_VALUE) return;
  const int value = sender->value.toInt();
  ESP_LOGD(TAG, "ESPUI volume request: %d", value);
  parent_->set_volume(static_cast<float>(value));
}

}  // namespace esphome::onkyo_iscp_web
