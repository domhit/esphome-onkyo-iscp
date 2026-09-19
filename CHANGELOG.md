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

## 0.3.0

### Added

- FM frequency control and state reporting
- AM frequency control and state reporting
- Tuner band detection
- Main and Zone 2 tuner status handling
- Tuner preset selection
- Preset next and previous controls
- Preset memory
- Manual tuning state using preset 0
- RDS receiver display controls
- RDS PTY selection and search
- RDS TP search

### Changed

- Tuner queries now run only when a tuner input is active
- Preset changes now synchronize both preset and frequency
- Tuner commands use the internal command queue

### Fixed

- Zone 2 tuner notifications no longer appear as unknown frames
- Preset 0 is now handled as manual tuning
- PTY and TP N/A responses are handled as valid states

### Known limitations

- No separate RDS station name or Radio Text data frames were
  observed on the TX-SR608
- PTY and TP searches can return N/A depending on station metadata

## 0.4.0

### Added

- HDMI Audio Out control
- Monitor output resolution control
- Video Wide Mode control
- Picture Mode control
- Raw audio signal information
- Structured audio signal diagnostics
- Raw video signal information
- Structured video signal diagnostics
- OSD and setup navigation
- OSD quick, audio and video menu controls
- Full state synchronization after the receiver powers on

### Changed

- Added HDMI and video state queries to full synchronization
- Preserved raw IFA and IFV responses for diagnostics
- Added defensive parsing for variable information fields
- Updated input options from AUX to AUX1 and AUX2
- OSD actions now use a validated command whitelist
- OSD actions are blocked while receiver communication is offline

### Fixed

- AUX1 and AUX2 responses no longer produce invalid select option errors
- Removed duplicate C++ helper method definitions
- Power-on from standby now triggers a full state refresh
- Prevented duplicate full synchronization after power restoration
- `TSTN/A` is handled as a known unavailable function
- `SLZ80` is handled as a Zone 2 input notification
- `OSDN/A` is handled as a valid unavailable OSD response

### Known limitations

- Successful OSD commands do not return acknowledgements on the TX-SR608
- IFA and IFV fields depend on the selected source and signal type
- HDMI signal diagnostics need additional real-world validation with
  connected HDMI sources and a display
- Zone 2 notifications are recognized but full Zone 2 control is not
  part of this release

# Planned changes

## open
korrekte dB-Umrechnung
Lautsprecherpegel
Parserstruktur für weitere ISCP-Kommandogruppen vorbereiten
Rohbefehl-Funktion für Entwicklungszwecke behalten

## Version 1.0.0
auf TX-SR608 vollständig getestet
reproduzierbare Beispielkonfiguration
bekannte Einschränkungen dokumentiert

