# Changelog

## 0.1.0

- Erstes Grundgerüst
- UART-Parser und Warteschlange
- Power, Mute, Lautstärke, Eingang
- native ESPHome-Entitäten
- generische Rohbefehl-Action

## 0.2.0

### Added

- Receiver availability detection
- Lightweight heartbeat polling
- Automatic state synchronization after reconnect
- Listening mode control
- Front bass and treble control
- Temporary subwoofer level control
- Temporary center level control
- Audyssey control
- Dynamic EQ control
- Dynamic Volume control
- Late Night control
- Re-EQ control
- Music Optimizer control
- Sleep timer
- Display dimmer
- Audio selector
- Speaker layout
- Unknown ISCP frame diagnostics

### Changed

- Reduced regular polling to a lightweight power heartbeat
- Improved UART frame parsing
- Added startup noise filtering

### Fixed

- Invalid UART startup bytes could be forwarded as invalid UTF-8
- Combined TFR bass/treble responses are now parsed correctly

# Planned changes
## 0.3.0
TUN
PRS
PRM
RDS
PTS
TPS
PTY
TP

## 0.4.0
HAO
RES
VWM
VPM
DIF
IFA
IFV
OSD

## open
korrekte dB-Umrechnung
Lautsprecherpegel
Parserstruktur für weitere ISCP-Kommandogruppen vorbereiten
Rohbefehl-Funktion für Entwicklungszwecke behalten

## Version 1.0.0
auf TX-SR608 vollständig getestet
reproduzierbare Beispielkonfiguration
bekannte Einschränkungen dokumentiert

