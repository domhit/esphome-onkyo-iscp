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
