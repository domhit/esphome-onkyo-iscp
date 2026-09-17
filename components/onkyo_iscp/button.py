import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button

from . import OnkyoIscp, onkyo_iscp_ns
from .const import (
    CONF_ONKYO_ISCP_ID,
    CONF_QUERY_ALL,
    CONF_VOLUME_DOWN,
    CONF_VOLUME_UP,
)

OnkyoVolumeUpButton = onkyo_iscp_ns.class_("OnkyoVolumeUpButton", button.Button)
OnkyoVolumeDownButton = onkyo_iscp_ns.class_("OnkyoVolumeDownButton", button.Button)
OnkyoQueryAllButton = onkyo_iscp_ns.class_("OnkyoQueryAllButton", button.Button)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ONKYO_ISCP_ID): cv.use_id(OnkyoIscp),
        cv.Optional(CONF_VOLUME_UP): button.button_schema(
            OnkyoVolumeUpButton, icon="mdi:volume-plus"
        ),
        cv.Optional(CONF_VOLUME_DOWN): button.button_schema(
            OnkyoVolumeDownButton, icon="mdi:volume-minus"
        ),
        cv.Optional(CONF_QUERY_ALL): button.button_schema(
            OnkyoQueryAllButton, icon="mdi:refresh"
        ),
    }
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_ONKYO_ISCP_ID])
    for key, cls in (
        (CONF_VOLUME_UP, "volume_up"),
        (CONF_VOLUME_DOWN, "volume_down"),
        (CONF_QUERY_ALL, "query_all"),
    ):
        if button_config := config.get(key):
            var = await button.new_button(button_config)
            cg.add(var.set_parent(parent))
