#include "onkyo_iscp_web.h"

#include <cstdio>
#include "esphome/core/log.h"

namespace esphome::onkyo_iscp_web {

static const char *const TAG = "onkyo_iscp_web";

void OnkyoIscpWeb::setup() {
  ESP_LOGI(TAG, "Starting minimal ESPUI remote");

  power_control_id_ = ESPUI.switcher("Power", [this](Control *, int type) { this->handle_power_(type); }, ControlColor::Emerald, false);
  volume_label_id_ = ESPUI.label("Volume", ControlColor::Turquoise, "0 / -82 dB");
  volume_control_id_ = ESPUI.slider("Master Volume", [this](Control *sender, int type) { this->handle_volume_(sender, type); }, ControlColor::Peterriver, 0, 0, 100);

  const std::string first_input = parent_ != nullptr && parent_->get_input_source_count() > 0 ? parent_->get_input_source_name(0) : std::string{};
  input_control_id_ = ESPUI.addControl(ControlType::Select, "Input", first_input.c_str(), ControlColor::Carrot, Control::noParent, [this](Control *sender, int type) { this->handle_input_(sender, type); });
  if (parent_ != nullptr) {
    for (size_t index = 0; index < parent_->get_input_source_count(); index++) {
      const std::string &name = parent_->get_input_source_name(index);
      ESPUI.addControl(ControlType::Option, name.c_str(), name.c_str(), ControlColor::Carrot, input_control_id_);
    }
  }

  ESPUI.button("Up", [this](Control *, int type) { this->handle_osd_("UP", type); }, ControlColor::Wetasphalt, "UP");
  ESPUI.button("Left", [this](Control *, int type) { this->handle_osd_("LEFT", type); }, ControlColor::Wetasphalt, "LEFT");
  ESPUI.button("Enter", [this](Control *, int type) { this->handle_osd_("ENTER", type); }, ControlColor::Peterriver, "ENTER");
  ESPUI.button("Right", [this](Control *, int type) { this->handle_osd_("RIGHT", type); }, ControlColor::Wetasphalt, "RIGHT");
  ESPUI.button("Down", [this](Control *, int type) { this->handle_osd_("DOWN", type); }, ControlColor::Wetasphalt, "DOWN");

  ESPUI.captivePortal = false;
  ESPUI.begin(title_.c_str());
  if (parent_ != nullptr) {
    parent_->set_state_listener(this);
    sync_due_ms_ = millis() + INITIAL_SYNC_DELAY_MS;
    initial_sync_pending_ = true;
  }
}

void OnkyoIscpWeb::loop() {
  const uint32_t now = millis();
  if (initial_sync_pending_ && parent_ != nullptr && static_cast<int32_t>(now - sync_due_ms_) >= 0) {
    initial_sync_pending_ = false;
    parent_->query_all();
    return;
  }
  if (!this->can_update_web_()) return;

  if (power_update_pending_) {
    power_update_pending_ = false;
    ESPUI.updateSwitcher(power_control_id_, power_state_);
    this->mark_web_update_();
    return;
  }
  if (volume_update_pending_) {
    volume_update_pending_ = false;
    ESPUI.updateSlider(volume_control_id_, volume_absolute_);
    char label[24];
    std::snprintf(label, sizeof(label), "%d / %+d dB", volume_absolute_, volume_relative_);
    ESPUI.updateLabel(volume_label_id_, label);
    this->mark_web_update_();
    return;
  }
  if (input_update_pending_) {
    input_update_pending_ = false;
    ESPUI.updateSelect(input_control_id_, input_state_.c_str());
    this->mark_web_update_();
  }
}

void OnkyoIscpWeb::dump_config() {
  ESP_LOGCONFIG(TAG, "Minimal Onkyo ESPUI remote:");
  ESP_LOGCONFIG(TAG, "  Title: %s", title_.c_str());
  ESP_LOGCONFIG(TAG, "  Input options: %u", parent_ != nullptr ? static_cast<unsigned int>(parent_->get_input_source_count()) : 0U);
  ESP_LOGCONFIG(TAG, "  Web update gap: %u ms", WEB_UPDATE_GAP_MS);
}

bool OnkyoIscpWeb::can_update_web_() const { return ESPUI.clientCount() > 0 && millis() - last_web_update_ms_ >= WEB_UPDATE_GAP_MS; }
void OnkyoIscpWeb::mark_web_update_() { last_web_update_ms_ = millis(); }
void OnkyoIscpWeb::on_power_state(bool state) { if (power_state_ != state || power_update_pending_) { power_state_ = state; power_update_pending_ = true; } }
void OnkyoIscpWeb::on_volume_state(float absolute, float relative) { volume_absolute_ = static_cast<int>(absolute); volume_relative_ = static_cast<int>(relative); volume_update_pending_ = true; }
void OnkyoIscpWeb::on_input_state(const std::string &input) { input_state_ = input; input_update_pending_ = true; }
void OnkyoIscpWeb::handle_power_(int type) { if (parent_ != nullptr && (type == S_ACTIVE || type == S_INACTIVE)) parent_->set_power(type == S_ACTIVE); }
void OnkyoIscpWeb::handle_volume_(Control *sender, int type) { if (parent_ != nullptr && type == SL_VALUE) parent_->set_volume(static_cast<float>(sender->value.toInt())); }
void OnkyoIscpWeb::handle_input_(Control *sender, int type) { if (parent_ != nullptr && type == S_VALUE) parent_->set_input(sender->value.c_str()); }
void OnkyoIscpWeb::handle_osd_(const char *command, int type) { if (parent_ != nullptr && type == B_DOWN) parent_->send_osd_command(command); }

}  // namespace esphome::onkyo_iscp_web
