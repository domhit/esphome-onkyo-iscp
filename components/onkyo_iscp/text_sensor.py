import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import ENTITY_CATEGORY_DIAGNOSTIC

from . import OnkyoIscp
from .const import (
    CONF_AUDIO_INFORMATION,
    CONF_AUDIO_INPUT_CHANNELS,
    CONF_AUDIO_INPUT_FORMAT,
    CONF_AUDIO_OUTPUT_FORMAT,
    CONF_AUDIO_SAMPLE_RATE,
    CONF_AUDIO_SOURCE,
    CONF_DISPLAY,
    CONF_LAST_FRAME,
    CONF_LAST_UNKNOWN_FRAME,
    CONF_ONKYO_ISCP_ID,
    CONF_VIDEO_INFORMATION,
    CONF_VIDEO_INPUT,
    CONF_VIDEO_INPUT_COLOR_DEPTH,
    CONF_VIDEO_INPUT_COLOR_SPACE,
    CONF_VIDEO_INPUT_RESOLUTION,
    CONF_VIDEO_OUTPUT,
    CONF_VIDEO_OUTPUT_COLOR_DEPTH,
    CONF_VIDEO_OUTPUT_COLOR_SPACE,
    CONF_VIDEO_OUTPUT_RESOLUTION,
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ONKYO_ISCP_ID): cv.use_id(OnkyoIscp),
        cv.Optional(CONF_LAST_FRAME): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon="mdi:serial-port",
        ),
        cv.Optional(CONF_DISPLAY): text_sensor.text_sensor_schema(
            icon="mdi:message-text-outline"
        ),
        cv.Optional(CONF_LAST_UNKNOWN_FRAME): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon="mdi:help-box-outline"
        ),
        cv.Optional(CONF_AUDIO_INFORMATION): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon="mdi:waveform",
        ),
        cv.Optional(CONF_VIDEO_INFORMATION): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon="mdi:video-outline",
        ),
        cv.Optional(CONF_AUDIO_SOURCE): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon="mdi:audio-input-rca",
        ),
        cv.Optional(CONF_AUDIO_INPUT_FORMAT): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon="mdi:audio-input-stereo-minijack",
        ),
        cv.Optional(CONF_AUDIO_SAMPLE_RATE): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon="mdi:sine-wave",
        ),
        cv.Optional(CONF_AUDIO_INPUT_CHANNELS): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon="mdi:surround-sound",
        ),
        cv.Optional(CONF_AUDIO_OUTPUT_FORMAT): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon="mdi:surround-sound-variant",
        ),
        cv.Optional(CONF_VIDEO_INPUT): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon="mdi:video-input-component",
        ),
        cv.Optional(CONF_VIDEO_INPUT_RESOLUTION): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon="mdi:monitor-arrow-down",
        ),
        cv.Optional(CONF_VIDEO_INPUT_COLOR_SPACE): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon="mdi:palette-outline",
        ),
        cv.Optional(CONF_VIDEO_INPUT_COLOR_DEPTH): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon="mdi:gradient-horizontal",
        ),
        cv.Optional(CONF_VIDEO_OUTPUT): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon="mdi:video-output",
        ),
        cv.Optional(CONF_VIDEO_OUTPUT_RESOLUTION): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon="mdi:monitor-arrow-up",
        ),
        cv.Optional(CONF_VIDEO_OUTPUT_COLOR_SPACE): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon="mdi:palette",
        ),
        cv.Optional(CONF_VIDEO_OUTPUT_COLOR_DEPTH): text_sensor.text_sensor_schema(
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
            icon="mdi:gradient-vertical",
        ),
    }
)

async def to_code(config):
    parent = await cg.get_variable(config[CONF_ONKYO_ISCP_ID])
    if last_frame_config := config.get(CONF_LAST_FRAME):
        var = await text_sensor.new_text_sensor(last_frame_config)
        cg.add(parent.set_last_frame_sensor(var))
    if last_unknown_config := config.get(CONF_LAST_UNKNOWN_FRAME):
        var = await text_sensor.new_text_sensor(last_unknown_config)
        cg.add(parent.set_last_unknown_frame_sensor(var))
    if display_config := config.get(CONF_DISPLAY):
        var = await text_sensor.new_text_sensor(display_config)
        cg.add(parent.set_display_sensor(var))
    if config_value := config.get(CONF_AUDIO_INFORMATION):
        var = await text_sensor.new_text_sensor(config_value)
        cg.add(parent.set_audio_information_sensor(var))
    if config_value := config.get(CONF_VIDEO_INFORMATION):
        var = await text_sensor.new_text_sensor(config_value)
        cg.add(parent.set_video_information_sensor(var))
    if sensor_config := config.get(CONF_AUDIO_SOURCE):
        var = await text_sensor.new_text_sensor(sensor_config)
        cg.add(parent.set_audio_source_sensor(var))
    if sensor_config := config.get(CONF_AUDIO_INPUT_FORMAT):
        var = await text_sensor.new_text_sensor(sensor_config)
        cg.add(parent.set_audio_input_format_sensor(var))
    if sensor_config := config.get(CONF_AUDIO_SAMPLE_RATE):
        var = await text_sensor.new_text_sensor(sensor_config)
        cg.add(parent.set_audio_sample_rate_sensor(var))
    if sensor_config := config.get(CONF_AUDIO_INPUT_CHANNELS):
        var = await text_sensor.new_text_sensor(sensor_config)
        cg.add(parent.set_audio_input_channels_sensor(var))
    if sensor_config := config.get(CONF_AUDIO_OUTPUT_FORMAT):
        var = await text_sensor.new_text_sensor(sensor_config)
        cg.add(parent.set_audio_output_format_sensor(var))
    if sensor_config := config.get(CONF_VIDEO_INPUT):
        var = await text_sensor.new_text_sensor(sensor_config)
        cg.add(parent.set_video_input_sensor(var))
    if sensor_config := config.get(CONF_VIDEO_INPUT_RESOLUTION):
        var = await text_sensor.new_text_sensor(sensor_config)
        cg.add(parent.set_video_input_resolution_sensor(var))
    if sensor_config := config.get(CONF_VIDEO_INPUT_COLOR_SPACE):
        var = await text_sensor.new_text_sensor(sensor_config)
        cg.add(parent.set_video_input_color_space_sensor(var))
    if sensor_config := config.get(CONF_VIDEO_INPUT_COLOR_DEPTH):
        var = await text_sensor.new_text_sensor(sensor_config)
        cg.add(parent.set_video_input_color_depth_sensor(var))
    if sensor_config := config.get(CONF_VIDEO_OUTPUT):
        var = await text_sensor.new_text_sensor(sensor_config)
        cg.add(parent.set_video_output_sensor(var))
    if sensor_config := config.get(CONF_VIDEO_OUTPUT_RESOLUTION):
        var = await text_sensor.new_text_sensor(sensor_config)
        cg.add(parent.set_video_output_resolution_sensor(var))
    if sensor_config := config.get(CONF_VIDEO_OUTPUT_COLOR_SPACE):
        var = await text_sensor.new_text_sensor(sensor_config)
        cg.add(parent.set_video_output_color_space_sensor(var))
    if sensor_config := config.get(CONF_VIDEO_OUTPUT_COLOR_DEPTH):
        var = await text_sensor.new_text_sensor(sensor_config)
        cg.add(parent.set_video_output_color_depth_sensor(var))