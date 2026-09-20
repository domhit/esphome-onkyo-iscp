import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import uart
from esphome.const import CONF_ID

AUTO_LOAD = ["binary_sensor", "switch", "number", "select", "button", "text_sensor"]
DEPENDENCIES = ["uart"]
MULTI_CONF = True

CONF_COMMAND = "command"

onkyo_iscp_ns = cg.esphome_ns.namespace("onkyo_iscp")
OnkyoIscp = onkyo_iscp_ns.class_("OnkyoIscp", cg.PollingComponent, uart.UARTDevice)
SendCommandAction = onkyo_iscp_ns.class_("SendCommandAction", automation.Action)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(OnkyoIscp),
        }
    )
    .extend(cv.polling_component_schema("30s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)

FINAL_VALIDATE_SCHEMA = uart.final_validate_device_schema(
    "onkyo_iscp",
    baud_rate=9600,
    require_rx=True,
    require_tx=True,
    data_bits=8,
    parity="NONE",
    stop_bits=1,
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

SEND_COMMAND_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.use_id(OnkyoIscp),
        cv.Required(CONF_COMMAND): cv.templatable(cv.string_strict),
    }
)

@automation.register_action(
    "onkyo_iscp.send",
    SendCommandAction,
    SEND_COMMAND_SCHEMA,
    synchronous=True,
)
async def onkyo_send_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, parent)
    command = await cg.templatable(config[CONF_COMMAND], args, cg.std_string)
    cg.add(var.set_command(command))
    return var
