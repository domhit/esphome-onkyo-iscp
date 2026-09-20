# ISCP Protocol Notes

## Serial Connection

The TX-SR608 communicates with the ESP8266 using a TTL UART connection.

Serial parameters:

- 9600 baud
- 8 data bits
- No parity
- 1 stop bit
- No flow control

## Frame Format

Commands use the Onkyo ISCP serial format:

`!1<COMMAND><VALUE>\r`

Examples:

`!1PWRQSTN\r`

`!1PWR01\r`

`!1MVL30\r`

`!1SLI10\r`

The `!1` prefix identifies Main Zone commands.

The serial frame is terminated by carriage return (`\r`).

## Parser

Incoming frames are normalized before processing.

The first three characters after the `!1` prefix are interpreted as the command group. The remaining characters form the command value.

Example:

`PWR01`

Command:

`PWR`

Value:

`01`

The parser is split into logical command groups:

- Core
- Audio
- Tuner
- Video and signal diagnostics
- Auxiliary commands
- Unknown-frame handling

The central dispatcher routes commands through:

`process_core_command_()`

`process_audio_command_()`

`process_tuner_command_()`

`process_video_command_()`

`process_auxiliary_command_()`

Commands that are not handled by any known group are passed to:

`process_unknown_command_()`

Known but unsupported or currently unused responses can be handled by the auxiliary parser so that they do not incorrectly appear as unknown frames.

Truly unknown frames are published through the diagnostic unknown-frame sensor.

## Command Queue

All outgoing commands use the internal command queue.

The queue:

- Preserves command order
- Contains up to 64 commands
- Enforces a minimum transmission interval of 100 ms
- Rejects empty commands
- Is used by native entities and raw ISCP commands

Commands are intentionally not automatically deduplicated because the position of status queries can be significant.

Example:

`VPM03`

followed by:

`VPMQSTN`

The query must remain after the write command.

## Full State Synchronization

Normal operation uses a lightweight heartbeat:

`PWRQSTN`

A complete state synchronization is triggered when:

- Receiver communication is re-established
- The receiver changes from Standby to On
- The user requests a complete state refresh

State queries are grouped internally into:

- Core
- Audio
- Display
- Routing
- Video and signal information
- Tuner, when applicable

Tuner queries are only triggered when an FM, AM or tuner input is active.

## Receiver Availability

Receiver availability is based on valid ISCP communication, not on the receiver power state.

This means:

`PWR00` means the receiver is connected and in Standby.

`PWR01` means the receiver is connected and powered on.

If no valid ISCP frame is received within the configured timeout, the receiver is considered unavailable.

A valid frame immediately restores receiver availability and triggers state synchronization.

## Startup Noise

Invalid UART startup bytes are discarded before they can reach ESPHome text sensors or the Home Assistant API.

Only normalized valid ISCP frames are forwarded to diagnostic entities.

## Raw Commands

Advanced and model-specific commands can be sent using the ESPHome raw-command action.

Example:

```yaml
- onkyo_iscp.send:
    id: onkyo_receiver
    command: "IFAQSTN"
```

The component automatically adds the `!1` prefix and carriage-return terminator when required.

Raw commands use the same internal command queue as native component controls.