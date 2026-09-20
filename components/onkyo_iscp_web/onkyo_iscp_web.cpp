#include "onkyo_iscp_web.h"

#include <cstdio>

#include "esphome/core/log.h"

namespace esphome::onkyo_iscp_web {

static const char *const TAG = "onkyo_iscp_web";

void OnkyoIscpWeb::setup() {
  ESP_LOGI(TAG, "Starting ESPUI Full PoC 0.3");

  main_tab_id_ = ESPUI.addControl(ControlType::Tab, "Main", "Main");
  receiver_status_id_ = ESPUI.addControl(
      ControlType::Label, "Receiver", "Unknown", ControlColor::Turquoise, main_tab_id_);
  power_control_id_ = ESPUI.addControl(
      ControlType::Switcher, "Power", "0", ControlColor::Emerald, main_tab_id_,
      [this](Control *, int type) { this->handle_power_(type); });
  mute_control_id_ = ESPUI.addControl(
      ControlType::Switcher, "Mute", "0", ControlColor::Alizarin, main_tab_id_,
      [this](Control *, int type) { this->handle_mute_(type); });
  volume_label_id_ = ESPUI.addControl(
      ControlType::Label, "Volume", "0 / -82 dB", ControlColor::Turquoise, main_tab_id_);
  volume_control_id_ = ESPUI.addControl(
      ControlType::Slider, "Master Volume", "0", ControlColor::Peterriver, main_tab_id_,
      [this](Control *sender, int type) { this->handle_volume_(sender, type); });
  ESPUI.addControl(
      ControlType::Min, "Master Volume", "0", ControlColor::None, volume_control_id_);
  ESPUI.addControl(
      ControlType::Max, "Master Volume", "100", ControlColor::None, volume_control_id_);
  volume_down_id_ = ESPUI.addControl(
      ControlType::Button, "Volume Down", "-", ControlColor::Wetasphalt, main_tab_id_,
      [this](Control *, int type) { this->handle_volume_down_(type); });
  volume_up_id_ = ESPUI.addControl(
      ControlType::Button, "Volume Up", "+", ControlColor::Wetasphalt, main_tab_id_,
      [this](Control *, int type) { this->handle_volume_up_(type); });

  const std::string first_input =
      parent_ != nullptr && parent_->get_input_source_count() > 0
          ? parent_->get_input_source_name(0)
          : std::string{};
  input_control_id_ = ESPUI.addControl(
      ControlType::Select, "Input", first_input.c_str(), ControlColor::Carrot, main_tab_id_,
      [this](Control *sender, int type) { this->handle_input_(sender, type); });
  if (parent_ != nullptr) {
    for (size_t index = 0; index < parent_->get_input_source_count(); index++) {
      const std::string &name = parent_->get_input_source_name(index);
      ESPUI.addControl(
          ControlType::Option, name.c_str(), name.c_str(), ControlColor::Carrot,
          input_control_id_);
    }
  }

  const char *first_mode =
      parent_ != nullptr && parent_->get_listening_mode_count() > 0
          ? parent_->get_listening_mode_name(0)
          : "";
  listening_mode_control_id_ = ESPUI.addControl(
      ControlType::Select, "Listening Mode", first_mode, ControlColor::Sunflower, main_tab_id_,
      [this](Control *sender, int type) { this->handle_listening_mode_(sender, type); });
  if (parent_ != nullptr) {
    for (size_t index = 0; index < parent_->get_listening_mode_count(); index++) {
      const char *name = parent_->get_listening_mode_name(index);
      ESPUI.addControl(
          ControlType::Option, name, name, ControlColor::Sunflower,
          listening_mode_control_id_);
    }
  }

  ESPUI.captivePortal = false;
  ESPUI.begin(title_.c_str());

  if (parent_ != nullptr) {
    parent_->set_state_listener(this);
    receiver_online_ = parent_->is_receiver_online();
    receiver_update_pending_ = true;
    sync_due_ms_ = millis() + INITIAL_SYNC_DELAY_MS;
    initial_sync_pending_ = true;
  }
}

void OnkyoIscpWeb::loop() {
  const uint32_t now = millis();
  if (initial_sync_pending_ && parent_ != nullptr &&
      static_cast<int32_t>(now - sync_due_ms_) >= 0) {
    initial_sync_pending_ = false;
    ESP_LOGD(TAG, "Starting delayed Onkyo state synchronization");
    parent_->query_all();
    return;
  }

  if (!this->can_update_web_()) return;

  if (receiver_update_pending_) {
    receiver_update_pending_ = false;
    ESPUI.updateLabel(receiver_status_id_, receiver_online_ ? "Online" : "Offline");
    this->mark_web_update_();
    return;
  }
  if (power_update_pending_) {
    power_update_pending_ = false;
    ESPUI.updateSwitcher(power_control_id_, power_state_);
    this->mark_web_update_();
    return;
  }
  if (mute_update_pending_) {
    mute_update_pending_ = false;
    ESPUI.updateSwitcher(mute_control_id_, mute_state_);
    this->mark_web_update_();
    return;
  }
  if (volume_update_pending_) {
    volume_update_pending_ = false;
    ESPUI.updateSlider(volume_control_id_, volume_absolute_);
    char label[24];
    std::snprintf(
        label, sizeof(label), "%d / %+d dB", volume_absolute_, volume_relative_);
    ESPUI.updateLabel(volume_label_id_, label);
    this->mark_web_update_();
    return;
  }
  if (input_update_pending_) {
    input_update_pending_ = false;
    ESPUI.updateSelect(input_control_id_, input_state_.c_str());
    this->mark_web_update_();
    return;
  }
  if (listening_mode_update_pending_) {
    listening_mode_update_pending_ = false;
    ESPUI.updateSelect(listening_mode_control_id_, listening_mode_state_.c_str());
    this->mark_web_update_();
  }
}

void OnkyoIscpWeb::dump_config() {
  ESP_LOGCONFIG(TAG, "Onkyo ISCP ESPUI Full PoC 0.3:");
  ESP_LOGCONFIG(TAG, "  Title: %s", title_.c_str());
  ESP_LOGCONFIG(TAG, "  Backend: %s", parent_ != nullptr ? "configured" : "missing");
  ESP_LOGCONFIG(
      TAG, "  Input options: %u",
      parent_ != nullptr ? static_cast<unsigned int>(parent_->get_input_source_count()) : 0U);
  ESP_LOGCONFIG(
      TAG, "  Listening modes: %u",
      parent_ != nullptr ? static_cast<unsigned int>(parent_->get_listening_mode_count()) : 0U);
  ESP_LOGCONFIG(TAG, "  Web update gap: %u ms", WEB_UPDATE_GAP_MS);
}

bool OnkyoIscpWeb::can_update_web_() const {
  return ESPUI.clientCount() > 0 && millis() - last_web_update_ms_ >= WEB_UPDATE_GAP_MS;
}

void OnkyoIscpWeb::mark_web_update_() { last_web_update_ms_ = millis(); }

void OnkyoIscpWeb::on_receiver_online_state(bool state) {
  if (receiver_online_ == state && !receiver_update_pending_) return;
  receiver_online_ = state;
  receiver_update_pending_ = true;
}

void OnkyoIscpWeb::on_power_state(bool state) {
  if (power_state_ == state && !power_update_pending_) return;
  power_state_ = state;
  power_update_pending_ = true;
}

void OnkyoIscpWeb::on_mute_state(bool state) {
  if (mute_state_ == state && !mute_update_pending_) return;
  mute_state_ = state;
  mute_update_pending_ = true;
}

void OnkyoIscpWeb::on_volume_state(float absolute, float relative) {
  const int absolute_value = static_cast<int>(absolute);
  const int relative_value = static_cast<int>(relative);
  if (volume_absolute_ == absolute_value && volume_relative_ == relative_value &&
      !volume_update_pending_)
    return;
  volume_absolute_ = absolute_value;
  volume_relative_ = relative_value;
  volume_update_pending_ = true;
}

void OnkyoIscpWeb::on_input_state(const std::string &input) {
  if (input_state_ == input && !input_update_pending_) return;
  input_state_ = input;
  input_update_pending_ = true;
}

void OnkyoIscpWeb::on_listening_mode_state(const std::string &mode) {
  if (listening_mode_state_ == mode && !listening_mode_update_pending_) return;
  listening_mode_state_ = mode;
  listening_mode_update_pending_ = true;
}

void OnkyoIscpWeb::handle_power_(int type) {
  if (parent_ == nullptr || (type != S_ACTIVE && type != S_INACTIVE)) return;
  parent_->set_power(type == S_ACTIVE);
}

void OnkyoIscpWeb::handle_mute_(int type) {
  if (parent_ == nullptr || (type != S_ACTIVE && type != S_INACTIVE)) return;
  parent_->set_mute(type == S_ACTIVE);
}

void OnkyoIscpWeb::handle_volume_(Control *sender, int type) {
  if (parent_ == nullptr || type != SL_VALUE) return;
  parent_->set_volume(static_cast<float>(sender->value.toInt()));
}

void OnkyoIscpWeb::handle_volume_up_(int type) {
  if (parent_ != nullptr && type == B_DOWN) parent_->send_command("MVLUP");
}

void OnkyoIscpWeb::handle_volume_down_(int type) {
  if (parent_ != nullptr && type == B_DOWN) parent_->send_command("MVLDOWN");
}

void OnkyoIscpWeb::handle_input_(Control *sender, int type) {
  if (parent_ != nullptr && type == S_VALUE) parent_->set_input(sender->value.c_str());
}

void OnkyoIscpWeb::handle_listening_mode_(Control *sender, int type) {
  if (parent_ != nullptr && type == S_VALUE)
    parent_->set_listening_mode(sender->value.c_str());
}

}  // namespace esphome::onkyo_iscp_web
