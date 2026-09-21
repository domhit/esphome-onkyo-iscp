# SPDX-License-Identifier: GPL-3.0-only
# Copyright (c) 2026 Dominic Hitschel

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import (
    DEVICE_CLASS_CONNECTIVITY,
    ENTITY_CATEGORY_DIAGNOSTIC,
)

from . import OnkyoIscp
from .const import CONF_CONNECTED, CONF_ONKYO_ISCP_ID


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ONKYO_ISCP_ID): cv.use_id(OnkyoIscp),
        cv.Optional(CONF_CONNECTED): binary_sensor.binary_sensor_schema(
            device_class=DEVICE_CLASS_CONNECTIVITY,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon="mdi:connection",
        ),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_ONKYO_ISCP_ID])

    if connected_config := config.get(CONF_CONNECTED):
        var = await binary_sensor.new_binary_sensor(connected_config)
        cg.add(parent.set_connected_binary_sensor(var))