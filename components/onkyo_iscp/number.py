import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number

from . import OnkyoIscp, onkyo_iscp_ns
from .const import CONF_CENTER_LEVEL, CONR_SUBWOOFER_LEVEL, CONF_FRONT_BASS, CONF_FRONT_TREBLE, CONF_MASTER_VOLUME, CONF_ONKYO_ISCP_ID

OnkyoVolumeNumber = onkyo_iscp_ns.class_("OnkyoVolumeNumber", number.Number)
OnkyoFrontBassNumber = onkyo_iscp_ns.class_("OnkyoFrontBassNumber", number.Number)
OnkyoFrontTrebleNumber = onkyo_iscp_ns.class_("OnkyoFrontTrebleNumber", number.Number,)
OnkyoSubwooferLevelNumber = onkyo_iscp_ns.class_("OnkyoSubwooferLevelNumber", number.Number,)
OnkyoCenterLevelNumber = onkyo_iscp_ns.class_("OnkyoCenterLevelNumber", number.Number,)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ONKYO_ISCP_ID): cv.use_id(OnkyoIscp),
        cv.Optional(CONF_MASTER_VOLUME): number.number_schema(
            OnkyoVolumeNumber,
            icon="mdi:volume-high",
        ),
        cv.Optional(CONF_FRONT_BASS): number.number_schema(
            OnkyoFrontBassNumber,
            icon="mdi:tune-vertical",
            unit_of_measurement="dB",
        ),

        cv.Optional(CONF_FRONT_TREBLE): number.number_schema(
            OnkyoFrontTrebleNumber,
            icon="mdi:tune-vertical",
            unit_of_measurement="dB",
        ),
        cv.Optional(CONF_SUBWOOFER_LEVEL): number.number_schema(
            OnkyoSubwooferLevelNumber,
            icon="mdi:speaker",
            unit_of_measurement="dB",
        ),

        cv.Optional(CONF_CENTER_LEVEL): number.number_schema(
            OnkyoCenterLevelNumber,
            icon="mdi:speaker",
            unit_of_measurement="dB",
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
        
    if bass_config := config.get(CONF_FRONT_BASS):
        var = await number.new_number(
            bass_config,
            min_value=-10,
            max_value=10,
            step=2,
        )
        cg.add(var.set_parent(parent))
        cg.add(parent.set_front_bass_number(var))

    if treble_config := config.get(CONF_FRONT_TREBLE):
        var = await number.new_number(
            treble_config,
            min_value=-10,
            max_value=10,
            step=2,
        )
        cg.add(var.set_parent(parent))
        cg.add(parent.set_front_treble_number(var))

    if subwoofer_config := config.get(CONF_SUBWOOFER_LEVEL):
        var = await number.new_number(
            subwoofer_config,
            min_value=-15,
            max_value=12,
            step=1,
        )
        cg.add(var.set_parent(parent))
        cg.add(parent.set_subwoofer_level_number(var))

    if center_config := config.get(CONF_CENTER_LEVEL):
        var = await number.new_number(
            center_config,
            min_value=-12,
            max_value=12,
            step=1,
        )
        cg.add(var.set_parent(parent))
        cg.add(parent.set_center_level_number(var))