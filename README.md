# ESPHome Onkyo ISCP

External ESPHome component for controlling older Onkyo AV receivers through their internal 3.3 V UART ISCP interface. Development and hardware testing currently focus on the Onkyo TX-SR608.

## Current feature set

- Native ESPHome API integration
- Bidirectional Onkyo ISCP communication over UART
- Receiver availability detection and automatic state synchronization
- Power and mute control
- Absolute master volume from 0 to 100
- Relative master volume from -82 to +18 dB
- Persistent runtime-configurable maximum master volume
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
- Raw and structured audio/video signal diagnostics
- OSD and setup navigation
- Speaker Level Calibration actions
- Raw ISCP command action for commands not exposed as native entities
- Unknown ISCP frame diagnostics
- Shared bounded outbound command queue with a minimum 100 ms transmission interval

## Features by version

### v0.2.0

- Receiver availability detection
- Listening modes
- Front bass and treble
- Temporary subwoofer and center levels
- Audyssey audio controls
- Sleep timer and display dimmer
- Audio selector and speaker layout
- Unknown-frame diagnostics
- Automatic state synchronization

### v0.3.0

#### Tuner

- FM and AM frequency read/control
- Main and Zone 2 tuner notifications
- Preset selection from 1 to 40
- Manual tuning state using preset 0
- Preset next/previous and preset memory
- RDS display actions
- PTY selection/search and TP search

#### RDS limitations

The TX-SR608 acknowledges RDS display commands, but no station name, Radio Text or PTY content was observed as separate ISCP data frames. RDS commands therefore primarily control the receiver display.

PTY and TP searches may return `N/A` depending on the station, broadcast metadata and current reception conditions.

### v0.4.0

#### Video and HDMI

Version 0.4.0 added bidirectional HDMI/video controls, raw and structured signal diagnostics, and OSD/setup navigation.

Supported controls include:

- HDMI Audio Out (`HAO`): Off, On, Auto
- Monitor Resolution (`RES`): Through, Auto, 480p, 720p, 1080i, 1080p
- Video Wide Mode (`VWM`): Auto, 4:3, Full, Zoom, Wide Zoom
- Picture Mode (`VPM`): Through, Custom, Cinema, Game

#### Audio and video diagnostics

The full `IFA` and `IFV` responses are available as raw diagnostic text sensors.

Structured `IFA` fields expose:

- Audio source
- Audio input format
- Audio sample rate
- Audio input channels
- Audio output format

Structured `IFV` fields expose:

- Video input
- Video input resolution
- Video input color space
- Video input color depth
- Video output
- Video output resolution
- Video output color space
- Video output color depth

IFA and IFV fields depend on the selected source and signal type. Missing fields are reported as `N/A`; receiver-provided values such as `UNKNOWN` and `None` are preserved.

#### OSD controls

Available OSD actions:

- Setup menu
- Up / Down / Left / Right
- Enter / Exit / Home
- Quick Setup
- Audio menu
- Video menu

The TX-SR608 normally accepts successful OSD commands without returning an acknowledgement. `OSDN/A` is treated as a known response when an action is unavailable in the current context.

### v0.5.0

Version 0.5.0 focused on display diagnostics and internal architecture improvements:

- Display Mode control using `DIF`
- Temporary audio/video format display
- Logical parser and full-sync query groups
- Consolidated bidirectional value mappings
- Shared bounded outbound command queue
- Minimum 100 ms interval between outgoing commands
- Raw commands routed through the same queue

### v0.6.0

Version 0.6.0 expanded volume handling, speaker calibration and input customization.

#### Absolute and relative master volume

Both master-volume entities can be enabled simultaneously:

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

Both entities represent the same physical receiver volume and remain synchronized through `MVL` responses.

Conversion:

```text
relative dB = absolute value - 82
absolute value = relative dB + 82
```

For example, absolute volume `52` corresponds to `-30 dB`.

> [!WARNING]
> The master-volume slider can cause large and very fast volume changes. Version 0.8.0 adds an optional persistent maximum-volume limit that can be changed at runtime from Home Assistant. The limit applies to native absolute and relative master-volume control and to Volume Up. Raw ISCP commands intentionally bypass this protection.

#### Speaker Level Calibration

The `SLC` command group is exposed as action buttons:

- `SLCTEST`: start the speaker-level test tone
- `SLCCHSEL`: select the next calibration channel
- `SLCUP`: increase the selected speaker level
- `SLCDOWN`: decrease the selected speaker level

These controls are actions and do not represent persistent calibration state.

#### Configurable input sources and aliases

The input selector can be restricted to inputs that actually exist on the receiver, and each input can have a custom Home Assistant display name.

```yaml
select:
  - platform: onkyo_iscp
    onkyo_iscp_id: onkyo_receiver

    input:
      name: Input
      sources:
        - onkyo_name: "VCR/DVR"
          name: "Game Console"

        - onkyo_name: "CBL/SAT"
          name: "Streaming Box"

        - onkyo_name: "AUX2"
          name: "Reserve"
          enabled: false
```

`onkyo_name` always refers to the original Onkyo input name. `name` is the Home Assistant display name. `enabled` defaults to `true` and can normally be omitted.

If `sources` is omitted completely, the legacy full input list remains available for backward compatibility.

Known input codes that are not enabled in the custom profile are recognized but are not published as invalid select states.

### v0.7.0

Version 0.7.0 is the stabilization and release-candidate milestone for the current TX-SR608 feature set.

Key changes and validation work:

- Cleaned up component code and formatting without changing the public configuration API
- Corrected structured `IFA` audio field parsing and added Audio Source / Audio Output Format
- Expanded structured `IFV` diagnostics with input/output color space and color depth
- Added ESP uptime, reset-reason, heap, largest-heap-block, fragmentation and loop-time diagnostics to the reference example
- Added a native Home Assistant dashboard example
- Updated the ESP8266 reference example and documentation
- Added GPL-3.0-only licensing and SPDX identifiers to source files
- Completed intensive command, full-sync and volume stress testing
- Completed more than 48 hours of continuous runtime testing; approximately 55 hours of diagnostic history were evaluated
- No progressive memory loss or heap fragmentation was observed
- No unexpected `Power On` resets occurred after replacing the unstable power supply

Detailed regression and stability-test results are documented in `docs/testing.md`.

#### v0.8.0

Version 0.8.0 adds runtime-configurable master-volume protection and improves handling of known Zone 2 notifications.

##### Maximum master volume

An optional `maximum_volume` number can be exposed in Home Assistant:

```yaml
number:
  - platform: onkyo_iscp
    onkyo_iscp_id: onkyo_receiver

    maximum_volume:
      name: Maximum Volume
      mode: box
```
The maximum-volume setting:

Defaults to 60 on the absolute Onkyo volume scale
Can be changed at runtime from Home Assistant without recompiling
Is stored persistently and restored after an ESP restart
Applies to native absolute master-volume control
Applies to native relative master-volume control
Prevents Volume Up from exceeding the configured maximum
Allows Volume Down while the current receiver volume is above the configured maximum

Lowering the configured maximum below the current receiver volume does not automatically reduce the volume. This avoids an unexpected volume change when only the protection limit is adjusted.

The raw onkyo_iscp.send action intentionally remains unrestricted and can bypass the maximum-volume protection. It should therefore be treated as an expert interface.

Zone 2 frame recognition

Known TX-SR608 Zone 2 command groups are recognized so normal Zone 2 notifications do not appear as unknown ISCP frames:

ZPW: Zone 2 power
ZMT: Zone 2 mute
ZVL: Zone 2 volume
ZTN: Zone 2 tone
ZBL: Zone 2 balance
SLZ: Zone 2 input selector
TUZ: Zone 2 tuner frequency
PRZ: Zone 2 tuner preset

Zone 2 control is intentionally not exposed as native Home Assistant entities because it is outside the tested project scope.

TUZ and PRZ are handled separately from Main Zone tuner state and therefore no longer update the Main Zone frequency or tuner-preset entities.

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

All outbound commands use a shared bounded command queue. The queue preserves command order, contains up to 64 commands and enforces a minimum transmission interval of 100 ms. Commands are intentionally not automatically deduplicated because query order can be significant.

## Project structure

```text
components/onkyo_iscp/              External ESPHome component
examples/tx-sr608-esp8266.yaml      Reference ESP8266 configuration
examples/home-assistant-dashboard.yaml
                                    Native Home Assistant dashboard example
docs/                               Hardware, protocol and test documentation
```

## Hardware

The current ESP8266 setup uses UART0 with GPIO1 as TX and GPIO3 as RX. The serial logger must therefore be disabled when these pins are used for receiver communication.

Disconnect the RX/TX connection to the Onkyo receiver before flashing the ESP8266 through USB. OTA updates are recommended after initial installation.

See `docs/pinout.md` for the verified TX-SR608 connection details.

Important hardware notes:

- Cross-connect receiver RX to ESP8266 TX and receiver TX to ESP8266 RX
- Use a common ground between receiver and ESP8266
- Do not power the ESP8266 from the receiver writing-port 3.3 V pin
- Do not use the tested receiver `+12VD_ST` or `+10VS` rails as an ESP8266 power source
- Use a stable external USB power supply

During v0.7.0 testing, repeated spontaneous `Power On` resets disappeared after replacing the previous USB power supply. The current reference setup uses an Amazon Fire TV 4K Max power supply. See `docs/testing.md` for the test results.

## Home Assistant dashboard

An example dashboard using only built-in Home Assistant cards is available at:

`examples/home-assistant-dashboard.yaml`

Replace the example entity IDs with the entity IDs from your Home Assistant installation.

## Compatibility

Development and live testing are currently performed with an Onkyo TX-SR608. Other Onkyo receivers using compatible ISCP commands may work, but command availability and accepted values can differ between models.

## Documentation

- `docs/pinout.md`: UART wiring and hardware notes
- `docs/protocol.md`: serial protocol and component architecture
- `docs/tx-sr608-commands.md`: verified TX-SR608 command matrix
- `docs/testing.md`: regression and long-term stability testing
- `docs/espui-poc.md`: ESPUI proof-of-concept findings

## License

This project is licensed under the GNU General Public License v3.0 only (`GPL-3.0-only`). See the `LICENSE` file for details.
