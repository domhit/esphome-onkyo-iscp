import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number

from . import OnkyoIscp, onkyo_iscp_ns
from .const import CONF_MASTER_VOLUME, CONF_ONKYO_ISCP_ID

OnkyoVolumeNumber = onkyo_iscp_ns.class_("OnkyoVolumeNumber", number.Number)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ONKYO_ISCP_ID): cv.use_id(OnkyoIscp),
        cv.Optional(CONF_MASTER_VOLUME): number.number_schema(
            OnkyoVolumeNumber,
            icon="mdi:volume-high",
        ),
    }
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_ONKYO_ISCP_ID])
    if volume_config := config.get(CONF_MASTER_VOLUME):
        var = await number.new_number(
            volume_config, min_value=0, max_value=100, step=1
        )
        cg.add(var.set_parent(parent))
        cg.add(parent.set_volume_number(var))
