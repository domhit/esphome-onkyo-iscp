import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import select

from . import OnkyoIscp, onkyo_iscp_ns
from .const import CONF_DYNAMIC_VOLUME, CONF_INPUT, CONF_LISTENING_MODE, CONF_ONKYO_ISCP_ID

OnkyoInputSelect = onkyo_iscp_ns.class_("OnkyoInputSelect", select.Select)
OnkyoListeningModeSelect = onkyo_iscp_ns.class_("OnkyoListeningModeSelect", select.Select,)
OnkyoDynamicVolumeSelect = onkyo_iscp_ns.class_("OnkyoDynamicVolumeSelect", select.Select,)

INPUT_OPTIONS = [
    "VCR/DVR", "CBL/SAT", "GAME/TV", "AUX", "PC",
    "BD/DVD", "TV/CD", "FM", "AM", "TUNER",
    "UNIVERSAL PORT", "HDMI 5", "HDMI 6", "HDMI 7",
]

LISTENING_MODE_OPTIONS = [
    "Stereo",
    "Direct",
    "Surround",
    "Game RPG",
    "Game Action",
    "Game Rock",
    "Orchestra",
    "Unplugged",
    "Studio-Mix",
    "TV Logic",
    "All Ch Stereo",
    "Theater-Dimensional",
    "Game Sports",
    "Mono",
    "Full Mono",
    "Audyssey DSX",
    "Straight Decode",
    "THX Cinema",
    "THX Surround EX",
    "THX Music",
    "THX Games",
    "THX Cinema 2",
    "THX Music Mode",
    "THX Games Mode",
    "PLII/PLIIx Movie",
    "PLII/PLIIx Music",
    "Neo:6 Cinema",
    "Neo:6 Music",
    "PLII/PLIIx THX Cinema",
    "Neo:6 THX Cinema",
    "PLII/PLIIx Game",
    "PLII/PLIIx THX Games",
    "Neo:6 THX Games",
    "PLII/PLIIx THX Music",
    "Neo:6 THX Music",
    "PLIIz Height",
    "PLIIz Height + THX Cinema",
    "PLIIz Height + THX Music",
    "PLIIz Height + THX Games",
    "PLII/PLIIx Movie + Audyssey DSX",
    "PLII/PLIIx Music + Audyssey DSX",
    "PLII/PLIIx Game + Audyssey DSX",
    "Neo:6 Cinema + Audyssey DSX",
    "Neo:6 Music + Audyssey DSX",
]

DYNAMIC_VOLUME_OPTIONS = [
    "Off",
    "Light",
    "Medium",
    "Heavy",
]

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ONKYO_ISCP_ID): cv.use_id(OnkyoIscp),
        cv.Optional(CONF_INPUT): select.select_schema(
            OnkyoInputSelect,
            icon="mdi:video-input-hdmi",
        ),
        cv.Optional(CONF_LISTENING_MODE): select.select_schema(
            OnkyoListeningModeSelect,
            icon="mdi:surround-sound",
        ),
        cv.Optional(CONF_DYNAMIC_VOLUME): select.select_schema(
            OnkyoDynamicVolumeSelect,
            icon="mdi:volume-equal",
        ),
    }
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_ONKYO_ISCP_ID])
    if input_config := config.get(CONF_INPUT):
        var = await select.new_select(input_config, options=INPUT_OPTIONS)
        cg.add(var.set_parent(parent))
        cg.add(parent.set_input_select(var))
    if listening_mode_config := config.get(CONF_LISTENING_MODE):
        var = await select.new_select(listening_mode_config, options=LISTENING_MODE_OPTIONS)
        cg.add(var.set_parent(parent))
        cg.add(parent.set_listening_mode_select(var))
    if dynamic_volume_config := config.get(
        CONF_DYNAMIC_VOLUME
    ):
        var = await select.new_select(
            dynamic_volume_config,
            options=DYNAMIC_VOLUME_OPTIONS,
        )

        cg.add(var.set_parent(parent))
        cg.add(parent.set_dynamic_volume_select(var))