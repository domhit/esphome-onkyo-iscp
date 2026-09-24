# TX-SR608 ISCP Command Verification

This document records ISCP command groups that have been verified or investigated during development of the ESPHome Onkyo ISCP component.

The original command candidates were derived from the Integra DTR-30.2 protocol matrix, a closely related receiver from the same generation. The focus of this document is the actual behavior observed on the Onkyo TX-SR608.

## Verification Status

- **Verified**: Tested successfully on the TX-SR608 and supported by the component
- **Partial**: Recognized or investigated, but not fully exposed or not available in every receiver state/configuration
- **Unsupported**: Tested and found unsupported on the TX-SR608
- **Experimental**: Used for diagnostics, development, or low-level testing

## Core Controls

### Verified

- `PWR` - Power
- `AMT` - Mute
- `MVL` - Master Volume
- `SLI` - Input Selector
- `DIM` - Display Dimmer
- `SLP` - Sleep Timer
- `FLD` - Receiver Display
- `DIF` - Display Mode and temporary signal-format display

### Master Volume

`MVL` uses the Onkyo absolute volume scale from 0 to 100. The component can expose both absolute and relative volume entities for the same physical receiver volume.

Conversion used by the component:

```text
relative dB = absolute value - 82
absolute value = relative dB + 82
```

For example, absolute volume 52 corresponds to -30 dB.

Since v0.8.0, the component can expose an optional persistent runtime-configurable maximum-volume limit. The limit is implemented locally by the ESPHome component and is **not** a separate ISCP command.

The maximum-volume protection applies to native absolute volume, relative volume and Volume Up control. Raw `onkyo_iscp.send` commands intentionally bypass this protection.

## Audio Controls

### Verified

- `LMD` - Listening Mode
- `TFR` - Front Bass and Treble
- `SWL` - Temporary Subwoofer Level
- `CTL` - Temporary Center Level
- `ADY` - Audyssey
- `ADQ` - Dynamic EQ
- `ADV` - Dynamic Volume
- `LTN` - Late Night
- `RAS` - Re-EQ
- `MOT` - Music Optimizer
- `SLA` - Audio Selector
- `SPL` - Speaker Layout

### Notes

`TFR` responses may contain bass and treble in one combined frame. The component parses both values individually.

Temporary speaker-level commands such as `SWL` and `CTL` represent receiver runtime adjustments and are separate from the Speaker Level Calibration command group described below.

## Tuner Controls

### Verified

- `TUN` - Main Zone tuner frequency
- `PRS` - Main Zone tuner preset
- `PRM` - Store current tuner frequency in the selected preset
- `RDS` - RDS display control
- `PTS` - PTY selection and PTY scan
- `TPS` - TP scan

### FM and AM

The TX-SR608 uses the `TUN` command group for both FM and AM tuning. The active tuner band is derived from the selected input and valid returned frequency range.

Tested frequency ranges used by the component:

- FM: 87.50 to 108.00 MHz
- AM: 522 to 1611 kHz

### Presets

`PRS` supports presets 1 to 40. Preset 0 is treated by the component as manual tuning with no active preset.

### RDS limitations

The TX-SR608 acknowledges the tested RDS display commands, but no station name, Radio Text or PTY content was observed as separate ISCP data frames during development.

RDS commands therefore primarily control the receiver display. PTY and TP searches may return `N/A` depending on station metadata and reception conditions.

## HDMI and Video Controls

### Verified

- `HAO` - HDMI Audio Out
- `RES` - Monitor Output Resolution
- `VWM` - Video Wide Mode
- `VPM` - Picture Mode

### Verified Values

`HAO`:

- Off
- On
- Auto

`RES`:

- Through
- Auto
- 480p
- 720p
- 1080i
- 1080p

`VWM`:

- Auto
- 4:3
- Full
- Zoom
- Wide Zoom

`VPM`:

- Through
- Custom
- Cinema
- Game

Writable HDMI/video settings are followed by a status query so Home Assistant displays the state reported by the receiver instead of assuming that a requested value was accepted.

## Audio and Video Signal Information

### Verified

- `IFA` - Audio Signal Information
- `IFV` - Video Signal Information

The complete receiver response is available as a raw diagnostic text sensor. The component also parses structured fields from the returned data.

### IFA Fields

The structured IFA parser exposes:

- Audio source
- Audio input format
- Audio sample rate
- Audio input channels
- Audio output format

### IFV Fields

The structured IFV parser exposes:

- Video input
- Video input resolution
- Video input color space
- Video input color depth
- Video output
- Video output resolution
- Video output color space
- Video output color depth

The number and content of available fields depend on the selected source, signal type and receiver state. Missing fields are reported as `N/A`. Receiver-provided values such as `UNKNOWN` and `None` are preserved.

## OSD and Setup Navigation

### Verified

- `OSD` - On-Screen Display / setup navigation

Tested actions:

- `MENU`
- `UP`
- `DOWN`
- `LEFT`
- `RIGHT`
- `ENTER`
- `EXIT`
- `HOME`
- `QUICK`
- `AUDIO`
- `VIDEO`

The TX-SR608 normally accepts successful OSD actions without returning a successful acknowledgement frame. Operation was verified using the receiver display.

`OSDN/A` is treated as a known response indicating that the requested action is unavailable in the current context.

## Speaker Level Calibration

### Verified

- `SLCTEST` - Start/select the speaker-level test-tone function
- `SLCCHSEL` - Select the next calibration channel
- `SLCUP` - Increase the selected speaker level
- `SLCDOWN` - Decrease the selected speaker level

These are action commands and do not represent persistent calibration state in Home Assistant.

## Auxiliary / Known Non-Entity Frames

### Partial

- `TST` - Test-related function

`TSTN/A` has been observed and is treated as a known unavailable response instead of an unknown frame.

## Zone 2

### Partial

The following Zone 2 command groups have been verified or investigated on the TX-SR608:

- `ZPW` - Zone 2 Power
- `ZMT` - Zone 2 Mute
- `ZVL` - Zone 2 Volume
- `ZTN` - Zone 2 Tone
- `ZBL` - Zone 2 Balance
- `SLZ` - Zone 2 Input Selector
- `TUZ` - Zone 2 Tuner Frequency
- `PRZ` - Zone 2 Tuner Preset

Zone 2 control is intentionally **not** exposed as native ESPHome/Home Assistant entities. The command groups are recognized so normal Zone 2 notifications do not appear as unknown ISCP frames.

### Observed TX-SR608 Behavior

Power:

```text
ZPWQSTN -> ZPW00
ZPW01   -> ZPW01
ZPWQSTN -> ZPW01
```

Mute:

```text
ZMTQSTN -> ZMT00
ZMT01   -> ZMT01
ZMTQSTN -> ZMT01
ZMT00   -> ZMT00
ZMTQSTN -> ZMT00
```

Volume:

```text
ZVLQSTN -> ZVL40
ZVL28   -> ZVLN/A
```

`ZVL40` represents hexadecimal `0x40`, or decimal 64. A write of `ZVL28` returned `N/A` in the tested receiver configuration. This result is documented as observed behavior only; no general conclusion about other Zone 2 configurations is made.

Input selector:

```text
SLZQSTN -> SLZ80
SLZ24   -> SLZ24
```

`SLZ80` represents Main Source. `SLZ24` selects FM and was accepted by the TX-SR608.

The TX-SR608 testing confirms `SLZ` as the working Zone 2 selector command character.

Tone and balance:

```text
ZTNQSTN -> ZTNN/A
ZBLQSTN -> ZBLN/A
```

Both responses were observed while Zone 2 was powered on. They are treated as known unavailable responses in the tested receiver configuration.

Zone 2 tuner:

```text
TUZQSTN -> TUZ08800
PRZQSTN -> PRZ01
```

Selecting FM through `SLZ24` also produced a `TUZ08800` notification.

### Main Zone / Zone 2 Separation

`TUN` and `PRS` represent Main Zone tuner state.

`TUZ` and `PRZ` represent Zone 2 tuner notifications and must not update Main Zone frequency or preset entities. Since v0.8.0, these command groups are recognized separately.

## Raw ISCP Command Access

### Experimental

The component provides a generic `onkyo_iscp.send` action for development, diagnostics and protocol investigation.

Example:

```yaml
on_press:
  - onkyo_iscp.send:
      id: onkyo_receiver
      command: "IFAQSTN"
```

The component automatically adds the `!1` prefix and carriage-return terminator when required. Raw commands use the same bounded outbound command queue and minimum 100 ms transmission interval as native controls.

Raw commands are intentionally not restricted by higher-level component safeguards such as the runtime maximum-volume limit.

## Known Unavailable / Context-Dependent Responses

The TX-SR608 can return `N/A` when a valid command group or action is unavailable in the current receiver context. Confirmed examples include:

- `TSTN/A`
- `OSDN/A`
- `ZTNN/A`
- `ZBLN/A`
- `ZVLN/A` for the tested Zone 2 volume write

These responses should not automatically be treated as unsupported protocol groups.

## Unknown Frame Diagnostics

Frames that do not match a supported or explicitly recognized command group are routed to the unknown-frame diagnostic path and can be exposed through the `last_unknown_frame` text sensor.

Known context-dependent or informational frames should be added to the appropriate parser group rather than permanently remaining in unknown-frame diagnostics.

## Implementation Scope

The project intentionally does not attempt to expose every command available in the complete Onkyo/Integra ISCP protocol.

The current approach is:

1. Verify behavior on the TX-SR608.
2. Expose useful, reproducible functions as native ESPHome entities.
3. Recognize known informational/context-dependent frames where appropriate.
4. Retain raw ISCP command access for diagnostics and advanced use.
5. Avoid adding native controls that cannot be meaningfully tested and maintained on the reference receiver setup.

Full Zone 2 control, Zone 3, network/USB playback and other model-specific command groups remain outside the current tested scope.
