import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

from ..onkyo_iscp import OnkyoIscp
from ..onkyo_iscp.const import CONF_ONKYO_ISCP_ID

DEPENDENCIES = ["wifi", "onkyo_iscp"]

CONF_TITLE = "title"

onkyo_iscp_web_ns = cg.esphome_ns.namespace("onkyo_iscp_web")
OnkyoIscpWeb = onkyo_iscp_web_ns.class_("OnkyoIscpWeb", cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(OnkyoIscpWeb),
        cv.Required(CONF_ONKYO_ISCP_ID): cv.use_id(OnkyoIscp),
        cv.Optional(CONF_TITLE, default="Onkyo TX-SR608"): cv.string_strict,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    parent = await cg.get_variable(config[CONF_ONKYO_ISCP_ID])
    cg.add(var.set_parent(parent))
    cg.add(var.set_title(config[CONF_TITLE]))

    cg.add_library(
        name="ESPUI",
        repository="https://github.com/s00500/ESPUI.git",
        version="1eb5e2ffc4eebf3a854247fd5208a5b88edc051e",
)
