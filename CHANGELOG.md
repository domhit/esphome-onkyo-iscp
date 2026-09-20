# Changelog

## 0.1.0

### Added
- Initial external ESPHome component
- UART ISCP parser
- Internal command queue
- Power control
- Mute control
- Master-volume control
- Input selection
- Native ESPHome entities
- Generic raw ISCP command action

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
- PTY and TP `N/A` responses are handled as valid states

### Known limitations
- No separate RDS station name or Radio Text data frames were observed on the TX-SR608
- PTY and TP searches can return `N/A` depending on station metadata

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
- HDMI signal diagnostics need additional real-world validation with connected HDMI sources and a display
- Zone 2 notifications are recognized, but full Zone 2 control is not part of this release

## 0.5.0

### Added
- Display Mode control
- Temporary audio-format display
- Temporary video-format display
- Display Mode wrap-around control
- Automatic IFA refresh after audio-format display
- Automatic IFV refresh after video-format display
- Bounded ISCP command queue

### Changed
- Split incoming command processing into logical parser groups
- Split full-state synchronization into logical query groups
- Consolidated bidirectional value mappings
- Routed all outgoing ISCP commands through the command queue
- Raw ISCP commands now use the same queue as native entities
- Enforced a minimum command interval of 100 ms for all outgoing commands

### Fixed
- Display Mode responses now update the Home Assistant select
- Fixed the PTY `Varied` name-to-code mapping
- Prevented direct commands from bypassing the configured command gap

### Notes
- The command queue is limited to 64 entries
- Commands are intentionally not deduplicated because query order can be significant

## 0.6.0

### Added
- Relative master-volume control from -82 to +18 dB
- Parallel absolute and relative master-volume entities
- Speaker Level Calibration controls using the SLC command group
- Speaker Level Test action using `SLCTEST`
- Speaker Level channel selection using `SLCCHSEL`
- Speaker Level increase using `SLCUP`
- Speaker Level decrease using `SLCDOWN`
- Configurable input source profiles
- Custom Home Assistant aliases for receiver inputs
- Optional disabling of individual input sources

### Changed
- Absolute master volume remains available on the Onkyo 0 to 100 scale
- Absolute and relative volume entities represent and control the same receiver volume state
- Every MVL response updates both configured master-volume entities
- Input selection can be restricted to inputs actually available on the receiver
- Receiver input responses are translated back to configured Home Assistant aliases
- Input source customization is optional; configurations without `sources` continue to use the legacy full input list
- Speaker Level Calibration commands use the shared outbound command queue

### Fixed
- Unsupported receiver inputs no longer need to be exposed in the Home Assistant input selector
- Known but disabled input codes no longer produce invalid select states
- Custom input aliases remain synchronized when the input is changed directly on the receiver

### Notes
- Absolute and relative master-volume entities can be enabled and used simultaneously
- Relative master volume is calculated as `absolute volume - 82`
- The TX-SR608 volume scale uses 1 dB steps in the tested configuration
- Input profiles validate duplicate Onkyo names, duplicate display names and empty enabled profiles
- Speaker Level Calibration entities are action buttons and do not represent persistent calibration state

## Roadmap

Development is focused on stable and well-tested support for the Onkyo TX-SR608. The goal is not to expose every model-dependent ISCP command, but to provide reliable Home Assistant entities for useful receiver functions while retaining raw-command access for advanced use cases.

### v0.7.0: Stabilization and release candidate
- Extended runtime testing
- Standby and power-cycle testing
- Receiver disconnect and reconnect testing
- UART startup-noise testing
- Home Assistant API reconnect testing
- Command queue stress testing
- Input alias and input-selection regression testing
- Absolute and relative volume regression testing
- Speaker Level Calibration regression testing
- Listening-mode regression testing
- Tuner and preset regression testing
- HDMI, video and OSD regression testing
- Review all known and unknown ISCP frames
- Freeze the public YAML configuration before version 1.0.0

### v1.0.0: Stable release

Version 1.0.0 will focus on stability and reproducibility rather than implementing every command available in the complete ISCP protocol.

Release requirements:
- Fully tested on the Onkyo TX-SR608
- Reproducible ESPHome example configuration
- Documented hardware and UART wiring
- Documented supported entities and commands
- Documented known limitations
- Stable reconnect and state synchronization
- Verified absolute and relative master-volume handling
- Verified Speaker Level Calibration controls
- Verified customizable input source profiles
- Structured and maintainable command parser
- Raw ISCP command action retained for development and advanced use
- Complete regression test passed
- Multi-day stability test passed
- Public YAML configuration considered stable

### Future considerations

The following features are intentionally outside the current roadmap and may be considered after version 1.0.0:
- Full Zone 2 control
- Zone 3 support
- Network and USB playback
- Onkyo dock and iPod control
- HDMI and digital-input assignment
- Extended speaker setup
- Additional model-specific ISCP commands
- WebUI

Zone-related notifications may still be recognized to prevent them from appearing as unknown frames, even when full zone control is not implemented.
