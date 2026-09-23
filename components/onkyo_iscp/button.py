# SPDX-License-Identifier: GPL-3.0-only
# Copyright (c) 2026 Dominic Hitschel

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button

from . import OnkyoIscp, onkyo_iscp_ns
from .const import (
    CONF_ONKYO_ISCP_ID,
    CONF_PRESET_DOWN,
    CONF_PRESET_STORE,
    CONF_PRESET_UP,
    CONF_QUERY_ALL,
    CONF_RDS_NEXT,
    CONF_RDS_PTY,
    CONF_RDS_RADIO_TEXT,
    CONF_RDS_TP,
    CONF_PTY_SCAN,
    CONF_TP_SCAN,
    CONF_VOLUME_DOWN,
    CONF_VOLUME_UP,
    CONF_OSD_AUDIO,
    CONF_OSD_DOWN,
    CONF_OSD_ENTER,
    CONF_OSD_EXIT,
    CONF_OSD_HOME,
    CONF_OSD_LEFT,
    CONF_OSD_MENU,
    CONF_OSD_QUICK,
    CONF_OSD_RIGHT,
    CONF_OSD_UP,
    CONF_OSD_VIDEO,
    CONF_DISPLAY_AUDIO_FORMAT,
    CONF_DISPLAY_MODE_NEXT,
    CONF_DISPLAY_VIDEO_FORMAT,
    CONF_SPEAKER_LEVEL_TEST,
    CONF_SPEAKER_LEVEL_NEXT,
    CONF_SPEAKER_LEVEL_UP,
    CONF_SPEAKER_LEVEL_DOWN,
)

OnkyoVolumeUpButton = onkyo_iscp_ns.class_("OnkyoVolumeUpButton", button.Button)
OnkyoVolumeDownButton = onkyo_iscp_ns.class_("OnkyoVolumeDownButton", button.Button)
OnkyoQueryAllButton = onkyo_iscp_ns.class_("OnkyoQueryAllButton", button.Button)
OnkyoPresetUpButton = onkyo_iscp_ns.class_("OnkyoPresetUpButton", button.Button)
OnkyoPresetDownButton = onkyo_iscp_ns.class_("OnkyoPresetDownButton", button.Button)
OnkyoPresetStoreButton = onkyo_iscp_ns.class_("OnkyoPresetStoreButton", button.Button)
OnkyoRdsRadioTextButton = onkyo_iscp_ns.class_("OnkyoRdsRadioTextButton", button.Button)
OnkyoRdsPtyButton = onkyo_iscp_ns.class_("OnkyoRdsPtyButton", button.Button)
OnkyoRdsTpButton = onkyo_iscp_ns.class_("OnkyoRdsTpButton", button.Button)
OnkyoRdsNextButton = onkyo_iscp_ns.class_("OnkyoRdsNextButton", button.Button)
OnkyoPtyScanButton = onkyo_iscp_ns.class_("OnkyoPtyScanButton", button.Button)
OnkyoTpScanButton = onkyo_iscp_ns.class_("OnkyoTpScanButton", button.Button)
OnkyoOsdMenuButton = onkyo_iscp_ns.class_("OnkyoOsdMenuButton", button.Button)
OnkyoOsdUpButton = onkyo_iscp_ns.class_("OnkyoOsdUpButton", button.Button)
OnkyoOsdDownButton = onkyo_iscp_ns.class_("OnkyoOsdDownButton", button.Button)
OnkyoOsdLeftButton = onkyo_iscp_ns.class_("OnkyoOsdLeftButton", button.Button)
OnkyoOsdRightButton = onkyo_iscp_ns.class_("OnkyoOsdRightButton", button.Button)
OnkyoOsdEnterButton = onkyo_iscp_ns.class_("OnkyoOsdEnterButton", button.Button)
OnkyoOsdExitButton = onkyo_iscp_ns.class_("OnkyoOsdExitButton", button.Button)
OnkyoOsdHomeButton = onkyo_iscp_ns.class_("OnkyoOsdHomeButton", button.Button)
OnkyoOsdQuickButton = onkyo_iscp_ns.class_("OnkyoOsdQuickButton", button.Button)
OnkyoOsdAudioButton = onkyo_iscp_ns.class_("OnkyoOsdAudioButton", button.Button)
OnkyoOsdVideoButton = onkyo_iscp_ns.class_("OnkyoOsdVideoButton", button.Button)
OnkyoDisplayAudioFormatButton = onkyo_iscp_ns.class_("OnkyoDisplayAudioFormatButton", button.Button)
OnkyoDisplayVideoFormatButton = onkyo_iscp_ns.class_("OnkyoDisplayVideoFormatButton", button.Button)
OnkyoDisplayModeNextButton = onkyo_iscp_ns.class_("OnkyoDisplayModeNextButton", button.Button)
OnkyoSpeakerLevelTestButton = onkyo_iscp_ns.class_("OnkyoSpeakerLevelTestButton", button.Button)
OnkyoSpeakerLevelNextButton = onkyo_iscp_ns.class_("OnkyoSpeakerLevelNextButton", button.Button)
OnkyoSpeakerLevelUpButton = onkyo_iscp_ns.class_("OnkyoSpeakerLevelUpButton", button.Button)
OnkyoSpeakerLevelDownButton = onkyo_iscp_ns.class_("OnkyoSpeakerLevelDownButton", button.Button)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ONKYO_ISCP_ID): cv.use_id(OnkyoIscp),
        cv.Optional(CONF_VOLUME_UP): button.button_schema(
            OnkyoVolumeUpButton, icon="mdi:volume-plus"
        ),
        cv.Optional(CONF_VOLUME_DOWN): button.button_schema(
            OnkyoVolumeDownButton, icon="mdi:volume-minus"
        ),
        cv.Optional(CONF_QUERY_ALL): button.button_schema(
            OnkyoQueryAllButton, icon="mdi:refresh"
        ),
        cv.Optional(CONF_PRESET_UP): button.button_schema(
            OnkyoPresetUpButton,
            icon="mdi:arrow-up-bold",
        ),
        cv.Optional(CONF_PRESET_DOWN): button.button_schema(
            OnkyoPresetDownButton,
            icon="mdi:arrow-down-bold",
        ),
        cv.Optional(CONF_PRESET_STORE): button.button_schema(
            OnkyoPresetStoreButton,
            icon="mdi:content-save",
        ),
        cv.Optional(CONF_RDS_RADIO_TEXT): button.button_schema(
            OnkyoRdsRadioTextButton,
            icon="mdi:text-box-outline",
        ),
        cv.Optional(CONF_RDS_PTY): button.button_schema(
            OnkyoRdsPtyButton,
            icon="mdi:format-list-bulleted-type",
        ),
        cv.Optional(CONF_RDS_TP): button.button_schema(
            OnkyoRdsTpButton,
            icon="mdi:car-info",
        ),
        cv.Optional(CONF_RDS_NEXT): button.button_schema(
            OnkyoRdsNextButton,
            icon="mdi:skip-next",
        ),
        cv.Optional(CONF_PTY_SCAN): button.button_schema(
            OnkyoPtyScanButton,
            icon="mdi:magnify",
        ),
        cv.Optional(CONF_TP_SCAN): button.button_schema(
            OnkyoTpScanButton,
            icon="mdi:car-search",
        ),
        cv.Optional(CONF_OSD_MENU): button.button_schema(
            OnkyoOsdMenuButton,
            icon="mdi:cog",
        ),
        cv.Optional(CONF_OSD_UP): button.button_schema(
            OnkyoOsdUpButton,
            icon="mdi:chevron-up",
        ),
        cv.Optional(CONF_OSD_DOWN): button.button_schema(
            OnkyoOsdDownButton,
            icon="mdi:chevron-down",
        ),
        cv.Optional(CONF_OSD_LEFT): button.button_schema(
            OnkyoOsdLeftButton,
            icon="mdi:chevron-left",
        ),
        cv.Optional(CONF_OSD_RIGHT): button.button_schema(
            OnkyoOsdRightButton,
            icon="mdi:chevron-right",
        ),
        cv.Optional(CONF_OSD_ENTER): button.button_schema(
            OnkyoOsdEnterButton,
            icon="mdi:checkbox-marked-circle-outline",
        ),
        cv.Optional(CONF_OSD_EXIT): button.button_schema(
            OnkyoOsdExitButton,
            icon="mdi:close-circle-outline",
        ),
        cv.Optional(CONF_OSD_HOME): button.button_schema(
            OnkyoOsdHomeButton,
            icon="mdi:home",
        ),
        cv.Optional(CONF_OSD_QUICK): button.button_schema(
            OnkyoOsdQuickButton,
            icon="mdi:tune-variant",
        ),
        cv.Optional(CONF_OSD_AUDIO): button.button_schema(
            OnkyoOsdAudioButton,
            icon="mdi:music-note",
        ),
        cv.Optional(CONF_OSD_VIDEO): button.button_schema(
            OnkyoOsdVideoButton,
            icon="mdi:video-outline",
        ),
        cv.Optional(CONF_DISPLAY_AUDIO_FORMAT): button.button_schema(
            OnkyoDisplayAudioFormatButton,
            icon="mdi:waveform",
        ),
        cv.Optional(CONF_DISPLAY_VIDEO_FORMAT): button.button_schema(
            OnkyoDisplayVideoFormatButton,
            icon="mdi:video-outline",
        ),
        cv.Optional(CONF_DISPLAY_MODE_NEXT): button.button_schema(
            OnkyoDisplayModeNextButton,
            icon="mdi:repeat",
        ),
        cv.Optional(CONF_SPEAKER_LEVEL_TEST): button.button_schema(
            OnkyoSpeakerLevelTestButton,
            icon="mdi:speaker-play",
        ),
        cv.Optional(CONF_SPEAKER_LEVEL_NEXT): button.button_schema(
            OnkyoSpeakerLevelNextButton,
            icon="mdi:speaker-multiple",
        ),
        cv.Optional(CONF_SPEAKER_LEVEL_UP): button.button_schema(
            OnkyoSpeakerLevelUpButton,
            icon="mdi:plus",
        ),
        cv.Optional(CONF_SPEAKER_LEVEL_DOWN): button.button_schema(
            OnkyoSpeakerLevelDownButton,
            icon="mdi:minus",
        ),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_ONKYO_ISCP_ID])

    for key in (
        CONF_VOLUME_UP,
        CONF_VOLUME_DOWN,
        CONF_QUERY_ALL,
        CONF_PRESET_UP,
        CONF_PRESET_DOWN,
        CONF_PRESET_STORE,
        CONF_RDS_RADIO_TEXT,
        CONF_RDS_PTY,
        CONF_RDS_TP,
        CONF_RDS_NEXT,
        CONF_PTY_SCAN,
        CONF_TP_SCAN,
        CONF_OSD_MENU,
        CONF_OSD_UP,
        CONF_OSD_DOWN,
        CONF_OSD_LEFT,
        CONF_OSD_RIGHT,
        CONF_OSD_ENTER,
        CONF_OSD_EXIT,
        CONF_OSD_HOME,
        CONF_OSD_QUICK,
        CONF_OSD_AUDIO,
        CONF_OSD_VIDEO,
        CONF_DISPLAY_AUDIO_FORMAT,
        CONF_DISPLAY_VIDEO_FORMAT,
        CONF_DISPLAY_MODE_NEXT,
        CONF_SPEAKER_LEVEL_TEST,
        CONF_SPEAKER_LEVEL_NEXT,
        CONF_SPEAKER_LEVEL_UP,
        CONF_SPEAKER_LEVEL_DOWN,
    ):
        if button_config := config.get(key):
            var = await button.new_button(button_config)
            cg.add(var.set_parent(parent))
