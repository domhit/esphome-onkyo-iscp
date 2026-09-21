# ESPHome Onkyo ISCP

External ESPHome component for controlling older Onkyo AV receivers through their internal 3.3 V UART ISCP interface. Development and hardware testing currently focus on the Onkyo TX-SR608.

## Current feature set

- Native ESPHome API integration
- Bidirectional Onkyo ISCP communication over UART
- Receiver availability detection and automatic state synchronization
- Power and mute control
- Absolute master volume from 0 to 100
- Relative master volume from -82 to +18 dB
- Configurable input selection with custom aliases
- Volume up and down
- Listening modes
- Front bass and treble
- Temporary subwoofer and center levels
- Audyssey, Dynamic EQ and Dynamic Volume
- Late Night, Re-EQ and Music Optimizer
- Sleep timer and display dimmer
- Audio selector and speaker layout
- FM and AM tuner control, presets and RDS actions
- HDMI and video controls
- Audio and video signal diagnostics
- OSD and setup navigation
- Speaker Level Calibration actions
- Raw ISCP command action for commands not exposed as native entities
- Unknown ISCP frame diagnostics
- Shared outbound command queue with a minimum 100 ms transmission interval

## Features by version

### v0.2.0

- Native ESPHome API
- Bidirectional Onkyo ISCP communication
- Receiver availability detection
- Power and mute
- Master volume
- Input selection
- Listening modes
- Front bass and treble
- Temporary subwoofer and center levels
- Audyssey
- Dynamic EQ
- Dynamic Volume
- Late Night
- Re-EQ
- Music Optimizer
- Sleep timer
- Display dimmer
- Audio selector
- Speaker layout
- Unknown ISCP frame diagnostics
- Automatic state synchronization

### v0.3.0

#### Tuner

- FM frequency read and control
- AM frequency read and control
- Main and Zone 2 tuner notifications
- Preset selection from 1 to 40
- Manual tuning state using preset 0
- Preset next and previous
- Preset memory
- RDS display controls
- PTY selection and search
- TP search

#### RDS limitations

The TX-SR608 acknowledges RDS display commands, but no station name, Radio Text or PTY content was observed as separate ISCP data frames. RDS commands therefore primarily control the receiver display.

PTY and TP searches may return `N/A` depending on the station, broadcast metadata and current reception conditions.

### v0.4.0

#### Video and HDMI

Version 0.4.0 adds bidirectional HDMI and video controls:

- HDMI Audio Out
- Monitor output resolution
- Video Wide Mode
- Picture Mode
- Raw audio signal information
- Structured audio diagnostics
- Raw video signal information
- Structured video diagnostics
- OSD and setup navigation

#### HDMI and video controls

| Entity | ISCP command | Supported values |
|---|---|---|
| HDMI Audio Out | `HAO` | Off, On, Auto |
| Monitor Resolution | `RES` | Through, Auto, 480p, 720p, 1080i, 1080p |
| Video Wide Mode | `VWM` | Auto, 4:3, Full, Zoom, Wide Zoom |
| Picture Mode | `VPM` | Through, Custom, Cinema, Game |

All writable values are followed by a status query. Home Assistant therefore displays the state reported by the receiver instead of assuming that a requested setting was accepted.

#### Audio and video diagnostics

The component provides the full `IFA` and `IFV` responses as optional raw diagnostic sensors.

Additional structured audio sensors expose:

- Input format
- Sample rate
- Input channels
- Output channels

Additional structured video sensors expose:

- Video input
- Input resolution
- Video output
- Output resolution

`IFA` and `IFV` responses are parsed defensively because the available fields depend on the selected source, signal type and receiver firmware. Missing fields are reported as `N/A`.

#### OSD controls

The following OSD actions are available:

- Setup menu
- Up
- Down
- Left
- Right
- Enter
- Exit
- Home
- Quick Setup
- Audio menu
- Video menu

The TX-SR608 accepts these commands without returning a successful ISCP acknowledgement. Operation was verified using the receiver front display.

A response of `OSDN/A` means that the requested action is not available in the current context. OSD entities are action buttons and do not represent the current menu position.

### v0.5.0

Version 0.5.0 focuses on display diagnostics and internal architecture improvements.

- Added Display Mode control using the `DIF` command group
- Added temporary audio-format display with `IFA` refresh
- Added temporary video-format display with `IFV` refresh
- Added Display Mode wrap-around control
- Split incoming ISCP parsing into logical command groups
- Split full-state synchronization into logical query groups
- Consolidated bidirectional value mappings
- Routed all outgoing ISCP commands through the shared command queue
- Raw ISCP commands now use the same queue as native entities
- Fixed the PTY `Varied` name-to-code mapping

### v0.6.0

Version 0.6.0 expands volume handling, speaker calibration and input customization.

#### Absolute and relative master volume

Two master-volume entities can be enabled at the same time:

- `master_volume`: absolute Onkyo scale from 0 to 100
- `master_volume_relative`: relative scale from -82 to +18 dB

Both entities represent the same physical receiver volume and remain synchronized through `MVL` responses. Either entity can be used to control the receiver.

Example:

```yaml
number:
  - platform: onkyo_iscp
    onkyo_iscp_id: onkyo_receiver

    master_volume:
      name: Master Volume Absolute
      mode: slider

    master_volume_relative:
      name: Master Volume Relative
      mode: slider
```

A receiver value of 52 on the absolute scale corresponds to -30 dB on the relative scale. The relative conversion is:

```text
relative dB = absolute value - 82
absolute value = relative dB + 82
```

#### Speaker Level Calibration

The `SLC` command group is exposed as action buttons for receiver speaker-level calibration:

| Entity | ISCP command | Function |
|---|---|---|
| Speaker Level Test | `SLCTEST` | Start or select the receiver test-tone function |
| Speaker Level Next | `SLCCHSEL` | Select the next calibration channel |
| Speaker Level Up | `SLCUP` | Increase the selected speaker level |
| Speaker Level Down | `SLCDOWN` | Decrease the selected speaker level |

Example:

```yaml
button:
  - platform: onkyo_iscp
    onkyo_iscp_id: onkyo_receiver

    speaker_level_test:
      name: Speaker Level Test
      disabled_by_default: true

    speaker_level_next:
      name: Next Speaker
      disabled_by_default: true

    speaker_level_up:
      name: Speaker Level Up
      disabled_by_default: true

    speaker_level_down:
      name: Speaker Level Down
      disabled_by_default: true
```

These entities are actions and do not represent persistent calibration state.

#### Configurable input sources and aliases

The input selector can optionally be restricted to the inputs that actually exist on a receiver. Each configured input can also be assigned a custom Home Assistant display name.

Example:

```yaml
select:
  - platform: onkyo_iscp
    onkyo_iscp_id: onkyo_receiver

    input:
      name: Input
      sources:
        - onkyo_name: "VCR/DVR"
          name: "XBOX"

        - onkyo_name: "CBL/SAT"
          name: "PS4"

        - onkyo_name: "GAME/TV"
          name: "PS3"

        - onkyo_name: "AUX1"
          name: "Switch"

        - onkyo_name: "PC"
          name: "Wii"

        - onkyo_name: "BD/DVD"
          name: "PS2"

        - onkyo_name: "TV/CD"
          name: "XBOX360"

        - onkyo_name: "FM"
          name: "FM"

        - onkyo_name: "AM"
          name: "AM"
```

Only enabled entries in `sources` appear in the Home Assistant select entity. The `onkyo_name` value always refers to the original Onkyo input name, while `name` is the custom display name.

An input can be kept in the configuration but hidden from the selector:

```yaml
        - onkyo_name: "AUX2"
          name: "Reserve"
          enabled: false
```

If `sources` is omitted completely, the component uses the full legacy input list. Existing configurations therefore remain compatible.

Known receiver input codes that are not enabled in the custom source profile are recognized but are not published as invalid select options.

## Raw ISCP commands

The generic `onkyo_iscp.send` action can be used for development, diagnostics or commands that do not yet have a native entity.

```yaml
on_press:
  - onkyo_iscp.send:
      id: onkyo_receiver
      command: "IFAQSTN"
```

The `!1` prefix and carriage-return terminator are added automatically when required. Raw commands use the same outbound command queue as native component controls.

## Internal architecture

Incoming ISCP frames are normalized and routed through logical parser groups:

- Core
- Audio
- Tuner
- Video and signal diagnostics
- Auxiliary commands
- Unknown-frame diagnostics

Full-state synchronization is split into matching query groups.

All outbound commands use a shared bounded command queue. The queue preserves command order, contains up to 64 commands and enforces a minimum transmission interval of 100 ms. This also applies to buttons and raw ISCP commands. Commands are intentionally not automatically deduplicated because query order can be significant.

## Project structure

```text
components/onkyo_iscp/  External ESPHome component
examples/               Example configurations
docs/                   Hardware and protocol documentation
```

## Hardware

The current ESP8266 setup uses UART0 with GPIO1 as TX and GPIO3 as RX. The serial logger must therefore be disabled when these pins are used for receiver communication.

Disconnect the RX/TX connection to the Onkyo receiver before flashing the ESP8266 through USB. OTA updates are recommended after initial installation.

See `docs/pinout.md` for the Onkyo connection details.

Wiring used for the current setup:

- Onkyo processor RX to ESP8266 GPIO1 TX
- Onkyo processor TX to ESP8266 GPIO3 RX
- Common ground between receiver and ESP8266
- Do not power the ESP8266 from the 3.3 V pin of the receiver writing port
- Do not use the receiver mainboard `+12VD_ST` or `+10VS` rails as an ESP8266 power source

## Home Assistant dashboard

An example dashboard using built-in Home Assistant cards is available at:

`examples/home-assistant-dashboard.yaml`

Replace the example entity IDs with the entity IDs from your Home Assistant
installation.

> [!WARNING]
> The master-volume slider can cause large and very fast volume changes.
> Verify the current value before moving the slider and increase the volume
> carefully. A configurable maximum-volume limit is planned for a future
> release but is not implemented in v0.7.0.

## Compatibility

Development and live testing are currently performed with an Onkyo TX-SR608. Other Onkyo receivers using compatible ISCP commands may work, but command availability and accepted values can differ between models.

## License

This project is licensed under the GNU General Public License v3.0 only
(`GPL-3.0-only`). See the LICENSE file for details.