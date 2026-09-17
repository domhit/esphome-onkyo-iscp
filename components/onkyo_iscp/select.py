import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import select

from . import OnkyoIscp, onkyo_iscp_ns
from .const import CONF_INPUT, CONF_ONKYO_ISCP_ID

OnkyoInputSelect = onkyo_iscp_ns.class_("OnkyoInputSelect", select.Select)

INPUT_OPTIONS = [
    "VCR/DVR", "CBL/SAT", "GAME/TV", "AUX1", "AUX2", "PC",
    "BD/DVD", "TAPE", "PHONO", "TV/CD", "FM", "AM", "TUNER",
    "MULTI CH", "UNIVERSAL PORT", "HDMI 5", "HDMI 6", "HDMI 7",
]

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ONKYO_ISCP_ID): cv.use_id(OnkyoIscp),
        cv.Optional(CONF_INPUT): select.select_schema(
            OnkyoInputSelect,
            icon="mdi:video-input-hdmi",
        ),
    }
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_ONKYO_ISCP_ID])
    if input_config := config.get(CONF_INPUT):
        var = await select.new_select(input_config, options=INPUT_OPTIONS)
        cg.add(var.set_parent(parent))
        cg.add(parent.set_input_select(var))
