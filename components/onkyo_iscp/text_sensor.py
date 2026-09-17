import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import ENTITY_CATEGORY_DIAGNOSTIC

from . import OnkyoIscp
from .const import CONF_DISPLAY, CONF_LAST_FRAME, CONF_LAST_UNKNOWN_FRAME, CONF_ONKYO_ISCP_ID

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ONKYO_ISCP_ID): cv.use_id(OnkyoIscp),
        cv.Optional(CONF_LAST_FRAME): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon="mdi:serial-port",
        ),
        cv.Optional(CONF_DISPLAY): text_sensor.text_sensor_schema(
            icon="mdi:message-text-outline"
        ),
        cv.Optional(CONF_LAST_UNKNOWN_FRAME): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon="mdi:help-box-outline"
        ),
    }
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_ONKYO_ISCP_ID])
    if last_frame_config := config.get(CONF_LAST_FRAME):
        var = await text_sensor.new_text_sensor(last_frame_config)
        cg.add(parent.set_last_frame_sensor(var))
    if last_unknown_config := config.get(CONF_LAST_UNKNOWN_FRAME):
        var = await text_sensor.new_text_sensor(last_unknown_config)
        cg.add(parent.set_last_unknown_frame_sensor(var))
    if display_config := config.get(CONF_DISPLAY):
        var = await text_sensor.new_text_sensor(display_config)
        cg.add(parent.set_display_sensor(var))
