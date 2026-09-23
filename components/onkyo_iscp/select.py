# SPDX-License-Identifier: GPL-3.0-only
# Copyright (c) 2026 Dominic Hitschel

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import select

from . import OnkyoIscp, onkyo_iscp_ns
from .const import (
    CONF_AUDIO_SELECTOR,
    CONF_DIMMER,
    CONF_DISPLAY_MODE,
    CONF_DYNAMIC_VOLUME,
    CONF_HDMI_AUDIO_OUT,
    CONF_INPUT,
    CONF_SOURCES,
    CONF_ONKYO_NAME,
    CONF_ENABLED,
    CONF_NAME,
    CONF_LATE_NIGHT,
    CONF_LISTENING_MODE,
    CONF_MONITOR_RESOLUTION,
    CONF_ONKYO_ISCP_ID,
    CONF_PICTURE_MODE,
    CONF_PTY,
    CONF_SPEAKER_LAYOUT,
    CONF_VIDEO_WIDE_MODE,
)

OnkyoInputSelect = onkyo_iscp_ns.class_("OnkyoInputSelect", select.Select)
OnkyoListeningModeSelect = onkyo_iscp_ns.class_("OnkyoListeningModeSelect", select.Select)
OnkyoDynamicVolumeSelect = onkyo_iscp_ns.class_("OnkyoDynamicVolumeSelect", select.Select)
OnkyoLateNightSelect = onkyo_iscp_ns.class_("OnkyoLateNightSelect", select.Select)
OnkyoDimmerSelect = onkyo_iscp_ns.class_("OnkyoDimmerSelect", select.Select)
OnkyoAudioSelectorSelect = onkyo_iscp_ns.class_("OnkyoAudioSelectorSelect", select.Select)
OnkyoSpeakerLayoutSelect = onkyo_iscp_ns.class_("OnkyoSpeakerLayoutSelect", select.Select)
OnkyoPtySelect = onkyo_iscp_ns.class_("OnkyoPtySelect", select.Select)
OnkyoHdmiAudioOutSelect = onkyo_iscp_ns.class_("OnkyoHdmiAudioOutSelect", select.Select)
OnkyoMonitorResolutionSelect = onkyo_iscp_ns.class_("OnkyoMonitorResolutionSelect", select.Select)
OnkyoVideoWideModeSelect = onkyo_iscp_ns.class_("OnkyoVideoWideModeSelect", select.Select)
OnkyoPictureModeSelect = onkyo_iscp_ns.class_("OnkyoPictureModeSelect", select.Select)
OnkyoDisplayModeSelect = onkyo_iscp_ns.class_("OnkyoDisplayModeSelect", select.Select)

INPUT_OPTIONS = [
    "VCR/DVR",
    "CBL/SAT",
    "GAME/TV",
    "AUX1",
    "AUX2",
    "PC",
    "BD/DVD",
    "TAPE",
    "PHONO",
    "TV/CD",
    "FM",
    "AM",
    "TUNER",
    "MULTI CH",
    "UNIVERSAL PORT",
    "HDMI 5",
    "HDMI 6",
    "HDMI 7",
]


INPUT_CODES = {
    name: code
    for code, name in [
        ("00", "VCR/DVR"),
        ("01", "CBL/SAT"),
        ("02", "GAME/TV"),
        ("03", "AUX1"),
        ("04", "AUX2"),
        ("05", "PC"),
        ("10", "BD/DVD"),
        ("20", "TAPE"),
        ("22", "PHONO"),
        ("23", "TV/CD"),
        ("24", "FM"),
        ("25", "AM"),
        ("26", "TUNER"),
        ("30", "MULTI CH"),
        ("40", "UNIVERSAL PORT"),
        ("55", "HDMI 5"),
        ("56", "HDMI 6"),
        ("57", "HDMI 7"),
    ]
}

INPUT_SOURCE_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_ONKYO_NAME): cv.one_of(*INPUT_CODES),
        cv.Optional(CONF_ENABLED, default=True): cv.boolean,
        cv.Optional(CONF_NAME): cv.string_strict,
    }
)


def validate_input_sources(value):
    enabled = [source for source in value if source[CONF_ENABLED]]
    if not enabled:
        raise cv.Invalid("At least one input source must be enabled")

    onkyo_names = [source[CONF_ONKYO_NAME] for source in value]
    if len(onkyo_names) != len(set(onkyo_names)):
        raise cv.Invalid("Duplicate onkyo_name in input sources")

    names = [
        source.get(CONF_NAME, source[CONF_ONKYO_NAME]).strip()
        for source in enabled
    ]
    if any(not name for name in names):
        raise cv.Invalid("Input source names must not be empty")
    if len(names) != len(set(names)):
        raise cv.Invalid("Duplicate display name in enabled input sources")
    return value

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

LATE_NIGHT_OPTIONS = [
    "Off",
    "Low",
    "High",
    "Auto",
]
DIMMER_OPTIONS = [
    "Bright",
    "Dim",
    "Dark",
]

AUDIO_SELECTOR_OPTIONS = [
    "Analog",
    "HDMI",
    "Coax/Optical",
    "ARC",
]

SPEAKER_LAYOUT_OPTIONS = [
    "Surround Back",
    "Front High",
    "Front Wide",
]

PTY_OPTIONS = [
    "None",
    "News",
    "Affairs",
    "Info",
    "Sport",
    "Educate",
    "Drama",
    "Culture",
    "Science",
    "Varied",
    "Pop M",
    "Rock M",
    "Easy M",
    "Light M",
    "Classics",
    "Other M",
    "Weather",
    "Finance",
    "Children",
    "Social",
    "Religion",
    "Phone In",
    "Travel",
    "Leisure",
    "Jazz",
    "Country",
    "Nation M",
    "Oldies",
    "Folk M",
    "Document",
    "TEST",
    "Alarm",
]

HDMI_AUDIO_OUT_OPTIONS = [
    "Off",
    "On",
    "Auto",
]

MONITOR_RESOLUTION_OPTIONS = [
    "Through",
    "Auto",
    "480p",
    "720p",
    "1080i",
    "1080p",
]

VIDEO_WIDE_MODE_OPTIONS = [
    "Auto",
    "4:3",
    "Full",
    "Zoom",
    "Wide Zoom",
]

PICTURE_MODE_OPTIONS = [
    "Through",
    "Custom",
    "Cinema",
    "Game",
]


DISPLAY_MODE_OPTIONS = [
    "Selector + Volume",
    "Selector + Listening Mode",
]

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ONKYO_ISCP_ID): cv.use_id(OnkyoIscp),
        cv.Optional(CONF_INPUT): select.select_schema(
            OnkyoInputSelect, icon="mdi:video-input-hdmi"
        ).extend(
            {
                cv.Optional(CONF_SOURCES): cv.All(
                    cv.ensure_list(INPUT_SOURCE_SCHEMA),
                    validate_input_sources,
                )
            }
        ),
        cv.Optional(CONF_LISTENING_MODE): select.select_schema(
            OnkyoListeningModeSelect,
            icon="mdi:surround-sound",
        ),
        cv.Optional(CONF_DYNAMIC_VOLUME): select.select_schema(
            OnkyoDynamicVolumeSelect,
            icon="mdi:volume-equal",
        ),
        cv.Optional(CONF_LATE_NIGHT): select.select_schema(
            OnkyoLateNightSelect,
            icon="mdi:weather-night",
        ),
        cv.Optional(CONF_DIMMER): select.select_schema(
            OnkyoDimmerSelect,
            icon="mdi:brightness-6",
        ),
        cv.Optional(CONF_AUDIO_SELECTOR): select.select_schema(
            OnkyoAudioSelectorSelect,
            icon="mdi:audio-input-stereo-minijack",
        ),
        cv.Optional(CONF_SPEAKER_LAYOUT): select.select_schema(
            OnkyoSpeakerLayoutSelect,
            icon="mdi:speaker-multiple",
        ),
        cv.Optional(CONF_PTY): select.select_schema(
            OnkyoPtySelect,
            icon="mdi:radio",
        ),
        cv.Optional(CONF_HDMI_AUDIO_OUT): select.select_schema(
            OnkyoHdmiAudioOutSelect,
            icon="mdi:hdmi-port",
        ),
        cv.Optional(CONF_MONITOR_RESOLUTION): select.select_schema(
            OnkyoMonitorResolutionSelect,
            icon="mdi:monitor",
        ),
        cv.Optional(CONF_VIDEO_WIDE_MODE): select.select_schema(
            OnkyoVideoWideModeSelect,
            icon="mdi:aspect-ratio",
        ),
        cv.Optional(CONF_PICTURE_MODE): select.select_schema(
            OnkyoPictureModeSelect,
            icon="mdi:image-filter-center-focus",
        ),
        cv.Optional(CONF_DISPLAY_MODE): select.select_schema(
            OnkyoDisplayModeSelect,
            icon="mdi:television-guide",
        ),
    }
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_ONKYO_ISCP_ID])
    if input_config := config.get(CONF_INPUT):
        sources = input_config.get(CONF_SOURCES)
        if sources is None:
            sources = [
                {CONF_ONKYO_NAME: name, CONF_NAME: name, CONF_ENABLED: True}
                for name in INPUT_OPTIONS
            ]
        enabled_sources = [source for source in sources if source[CONF_ENABLED]]
        options = [
            source.get(CONF_NAME, source[CONF_ONKYO_NAME])
            for source in enabled_sources
        ]
        var = await select.new_select(input_config, options=options)
        cg.add(var.set_parent(parent))
        cg.add(parent.set_input_select(var))
        for source in enabled_sources:
            onkyo_name = source[CONF_ONKYO_NAME]
            cg.add(
                parent.add_input_source(
                    INPUT_CODES[onkyo_name],
                    source.get(CONF_NAME, onkyo_name),
                )
            )
    if listening_mode_config := config.get(CONF_LISTENING_MODE):
        var = await select.new_select(
            listening_mode_config, options=LISTENING_MODE_OPTIONS
        )
        cg.add(var.set_parent(parent))
        cg.add(parent.set_listening_mode_select(var))
    if dynamic_volume_config := config.get(CONF_DYNAMIC_VOLUME):
        var = await select.new_select(
            dynamic_volume_config, options=DYNAMIC_VOLUME_OPTIONS
        )
        cg.add(var.set_parent(parent))
        cg.add(parent.set_dynamic_volume_select(var))
    if late_night_config := config.get(CONF_LATE_NIGHT):
        var = await select.new_select(
            late_night_config,
            options=LATE_NIGHT_OPTIONS,
        )
        cg.add(var.set_parent(parent))
        cg.add(parent.set_late_night_select(var))
    if dimmer_config := config.get(CONF_DIMMER):
        var = await select.new_select(
            dimmer_config,
            options=DIMMER_OPTIONS,
        )
        cg.add(var.set_parent(parent))
        cg.add(parent.set_dimmer_select(var))
    if audio_selector_config := config.get(CONF_AUDIO_SELECTOR):
        var = await select.new_select(
            audio_selector_config,
            options=AUDIO_SELECTOR_OPTIONS,
        )
        cg.add(var.set_parent(parent))
        cg.add(parent.set_audio_selector_select(var))
    if speaker_layout_config := config.get(CONF_SPEAKER_LAYOUT):
        var = await select.new_select(
            speaker_layout_config,
            options=SPEAKER_LAYOUT_OPTIONS,
        )
        cg.add(var.set_parent(parent))
        cg.add(parent.set_speaker_layout_select(var))
    if pty_config := config.get(CONF_PTY):
        var = await select.new_select(
            pty_config,
            options=PTY_OPTIONS,
        )
        cg.add(var.set_parent(parent))
        cg.add(parent.set_pty_select(var))
    if hdmi_audio_out_config := config.get(CONF_HDMI_AUDIO_OUT):
        var = await select.new_select(
            hdmi_audio_out_config,
            options=HDMI_AUDIO_OUT_OPTIONS,
        )
        cg.add(var.set_parent(parent))
        cg.add(parent.set_hdmi_audio_out_select(var))
    if monitor_resolution_config := config.get(CONF_MONITOR_RESOLUTION):
        var = await select.new_select(
            monitor_resolution_config,
            options=MONITOR_RESOLUTION_OPTIONS,
        )
        cg.add(var.set_parent(parent))
        cg.add(parent.set_monitor_resolution_select(var))
    if video_wide_mode_config := config.get(CONF_VIDEO_WIDE_MODE):
        var = await select.new_select(
            video_wide_mode_config,
            options=VIDEO_WIDE_MODE_OPTIONS,
        )
        cg.add(var.set_parent(parent))
        cg.add(parent.set_video_wide_mode_select(var))
    if picture_mode_config := config.get(CONF_PICTURE_MODE):
        var = await select.new_select(
            picture_mode_config,
            options=PICTURE_MODE_OPTIONS,
        )
        cg.add(var.set_parent(parent))
        cg.add(parent.set_picture_mode_select(var))
    if display_mode_config := config.get(CONF_DISPLAY_MODE):
        var = await select.new_select(
            display_mode_config,
            options=DISPLAY_MODE_OPTIONS,
        )
        cg.add(var.set_parent(parent))
        cg.add(parent.set_display_mode_select(var))