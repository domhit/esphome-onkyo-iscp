# ESPHome Onkyo ISCP

Grundgerüst einer externen ESPHome-Komponente für die serielle 3,3-V-UART-Schnittstelle älterer Onkyo-Receiver, zunächst für den TX-SR608.

## Aktueller Funktionsumfang

- Power lesen/schalten
- Mute lesen/schalten
- Master-Volume als Rohwert lesen/setzen
- Eingang lesen/setzen
- Lauter/Leiser
- zyklische Statusabfrage
- letzter empfangener ISCP-Frame
- generische Action `onkyo_iscp.send` für noch nicht implementierte Befehle

## Features

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

### Tuner

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

### RDS limitations

The TX-SR608 acknowledges RDS display commands but no station
name, Radio Text or PTY content was observed as separate ISCP
data frames. RDS commands control the receiver display.

PTY and TP searches may return N/A depending on the station,
broadcast metadata and current reception conditions.

### Video and HDMI

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

### HDMI and video controls

| Entity | ISCP command | Supported values |
|---|---|---|
| HDMI Audio Out | HAO | Off, On, Auto |
| Monitor Resolution | RES | Through, Auto, 480p, 720p, 1080i, 1080p |
| Video Wide Mode | VWM | Auto, 4:3, Full, Zoom, Wide Zoom |
| Picture Mode | VPM | Through, Custom, Cinema, Game |

All writable values are followed by a status query. Home Assistant
therefore displays the state reported by the receiver rather than
assuming that a requested setting was accepted.

### Audio and video diagnostics

The component provides the full IFA and IFV responses as optional raw
diagnostic sensors.

Additional structured sensors expose:

Audio:

- Input format
- Sample rate
- Input channels
- Output channels

Video:

- Video input
- Input resolution
- Video output
- Output resolution

IFA and IFV responses are parsed defensively because the available
fields depend on the selected source, signal type and receiver
firmware. Missing fields are reported as `N/A`.

### OSD controls

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

The TX-SR608 accepts these commands without returning a successful
ISCP acknowledgement. Operation was verified using the receiver front
display.

A response of `OSDN/A` means that the requested action is not available
in the current context.

OSD entities are action buttons and do not represent the current menu
position.

## Internal architecture

Incoming ISCP frames are normalized and routed through logical parser
groups:

- Core
- Audio
- Tuner
- Video and signal diagnostics
- Auxiliary commands
- Unknown-frame diagnostics

Full-state synchronization is split into matching query groups.

All outbound commands use a shared command queue. The queue preserves
command order and enforces a minimum transmission interval of 100 ms.
This also applies to buttons and raw ISCP commands.

## Struktur

```text
components/onkyo_iscp/  Externe ESPHome-Komponente
examples/               Beispielkonfigurationen
docs/                   Hardware- und Protokolldokumentation
```

## Hardware

Für den ESP8266 ist UART0 auf GPIO1 TX und GPIO3 RX vorgesehen. Deshalb ist der serielle Logger deaktiviert. Vor USB-Flashvorgängen RX/TX zum Onkyo trennen; danach OTA verwenden.

Die Onkyo-Belegung siehe /docs/pinout.md

- Onkyo Prozessor-RX an ESP8266 GPIO1 TX
- Onkyo Prozessor-TX an ESP8266 GPIO3 RX
- gemeinsame Masse
- 
- keine Versorgung über den 3,3-V-Pin des Writing Ports, oder den +12VD_ST und +10VS der Mainboardversorgung
