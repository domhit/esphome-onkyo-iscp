# SPDX-License-Identifier: GPL-3.0-only
# Copyright (c) 2026 Dominic Hitschel

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch

from . import OnkyoIscp, onkyo_iscp_ns
from .const import (
    CONF_AUDYSSEY,
    CONF_DYNAMIC_EQ,
    CONF_MUSIC_OPTIMIZER,
    CONF_MUTE,
    CONF_ONKYO_ISCP_ID,
    CONF_POWER,
    CONF_RE_EQ,
)

OnkyoPowerSwitch = onkyo_iscp_ns.class_("OnkyoPowerSwitch", switch.Switch)
OnkyoMuteSwitch = onkyo_iscp_ns.class_("OnkyoMuteSwitch", switch.Switch)
OnkyoAudysseySwitch = onkyo_iscp_ns.class_(
    "OnkyoAudysseySwitch",
    switch.Switch,
)
OnkyoDynamicEqSwitch = onkyo_iscp_ns.class_(
    "OnkyoDynamicEqSwitch",
    switch.Switch,
)
OnkyoReEqSwitch = onkyo_iscp_ns.class_(
    "OnkyoReEqSwitch",
    switch.Switch,
)
OnkyoMusicOptimizerSwitch = onkyo_iscp_ns.class_(
    "OnkyoMusicOptimizerSwitch",
    switch.Switch,
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ONKYO_ISCP_ID): cv.use_id(OnkyoIscp),
        cv.Optional(CONF_POWER): switch.switch_schema(OnkyoPowerSwitch),
        cv.Optional(CONF_MUTE): switch.switch_schema(OnkyoMuteSwitch),
        cv.Optional(CONF_AUDYSSEY): switch.switch_schema(
            OnkyoAudysseySwitch,
            icon="mdi:tune-variant",
        ),
        cv.Optional(CONF_DYNAMIC_EQ): switch.switch_schema(
            OnkyoDynamicEqSwitch,
            icon="mdi:equalizer",
        ),
        cv.Optional(CONF_RE_EQ): switch.switch_schema(
            OnkyoReEqSwitch,
            icon="mdi:movie-filter",
        ),
        cv.Optional(CONF_MUSIC_OPTIMIZER): switch.switch_schema(
            OnkyoMusicOptimizerSwitch,
            icon="mdi:music-note-plus",
        ),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_ONKYO_ISCP_ID])
    if power_config := config.get(CONF_POWER):
        var = await switch.new_switch(power_config)
        cg.add(var.set_parent(parent))
        cg.add(parent.set_power_switch(var))
    if mute_config := config.get(CONF_MUTE):
        var = await switch.new_switch(mute_config)
        cg.add(var.set_parent(parent))
        cg.add(parent.set_mute_switch(var))
    if audyssey_config := config.get(CONF_AUDYSSEY):
        var = await switch.new_switch(audyssey_config)
        cg.add(var.set_parent(parent))
        cg.add(parent.set_audyssey_switch(var))
    if dynamic_eq_config := config.get(CONF_DYNAMIC_EQ):
        var = await switch.new_switch(dynamic_eq_config)
        cg.add(var.set_parent(parent))
        cg.add(parent.set_dynamic_eq_switch(var))
    if re_eq_config := config.get(CONF_RE_EQ):
        var = await switch.new_switch(re_eq_config)
        cg.add(var.set_parent(parent))
        cg.add(parent.set_re_eq_switch(var))
    if music_optimizer_config := config.get(CONF_MUSIC_OPTIMIZER):
        var = await switch.new_switch(music_optimizer_config)
        cg.add(var.set_parent(parent))
        cg.add(parent.set_music_optimizer_switch(var))
