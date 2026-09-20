#include "onkyo_iscp_web.h"

#include "esphome/core/log.h"

namespace esphome::onkyo_iscp_web {

static const char *const TAG = "onkyo_iscp_web";

void OnkyoIscpWeb::setup() {
  ESP_LOGI(TAG, "Starting ESPUI proof of concept");

  ESPUI.label(
      "Status",
      ControlColor::Turquoise,
      "ESPUI proof of concept is running"
  );
  ESPUI.begin(title_.c_str());
}

void OnkyoIscpWeb::dump_config() {
  ESP_LOGCONFIG(TAG, "Onkyo ISCP ESPUI proof of concept:");
  ESP_LOGCONFIG(TAG, "  Title: %s", title_.c_str());
  ESP_LOGCONFIG(TAG, "  Backend: %s", parent_ != nullptr ? "configured" : "missing");
}

}  // namespace esphome::onkyo_iscp_web
