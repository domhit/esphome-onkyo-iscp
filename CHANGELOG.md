## Changelog

### 0.1.0

#### Added
- Initial external ESPHome component
- UART ISCP parser
- Internal command queue
- Power control
- Mute control
- Master-volume control
- Input selection
- Native ESPHome entities
- Generic raw ISCP command action

### 0.2.0

#### Added
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

#### Changed
- Reduced regular polling to a lightweight power heartbeat
- Improved UART frame parsing
- Added startup noise filtering

#### Fixed
- Invalid UART startup bytes could be forwarded as invalid UTF-8
- Combined TFR bass/treble responses are now parsed correctly

### 0.3.0

#### Added
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

#### Changed
- Tuner queries now run only when a tuner input is active
- Preset changes now synchronize both preset and frequency
- Tuner commands use the internal command queue

#### Fixed
- Zone 2 tuner notifications no longer appear as unknown frames
- Preset 0 is now handled as manual tuning
- PTY and TP N/A responses are handled as valid states

#### Known limitations
- No separate RDS station name or Radio Text data frames were observed on the TX-SR608
- PTY and TP searches can return N/A depending on station metadata

### 0.4.0

#### Added
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

#### Changed
- Added HDMI and video state queries to full synchronization
- Preserved raw IFA and IFV responses for diagnostics
- Added defensive parsing for variable information fields
- Updated input options from AUX to AUX1 and AUX2
- OSD actions now use a validated command whitelist
- OSD actions are blocked while receiver communication is offline

#### Fixed
- AUX1 and AUX2 responses no longer produce invalid select option errors
- Removed duplicate C++ helper method definitions
- Power-on from standby now triggers a full state refresh
- Prevented duplicate full synchronization after power restoration
- TSTN/A is handled as a known unavailable function
- SLZ80 is handled as a Zone 2 input notification
- OSDN/A is handled as a valid unavailable OSD response

#### Known limitations
- Successful OSD commands do not return acknowledgements on the TX-SR608
- IFA and IFV fields depend on the selected source and signal type
- Zone 2 notifications are recognized, but full Zone 2 control is not part of this release

### 0.5.0

#### Added
- Display Mode control
- Temporary audio-format display
- Temporary video-format display
- Display Mode wrap-around control
- Automatic IFA refresh after audio-format display
- Automatic IFV refresh after video-format display
- Bounded ISCP command queue

#### Changed
- Split incoming command processing into logical parser groups
- Split full-state synchronization into logical query groups
- Consolidated bidirectional value mappings
- Routed all outgoing ISCP commands through the command queue
- Raw ISCP commands now use the same queue as native entities
- Enforced a minimum command interval of 100 ms for all outgoing commands

#### Fixed
- Display Mode responses now update the Home Assistant select
- Fixed the PTY Varied name-to-code mapping
- Prevented direct commands from bypassing the configured command gap

#### Notes
- The command queue is limited to 64 entries
- Commands are intentionally not deduplicated because query order can be significant

### 0.6.0

#### Added
- Relative master-volume control from -82 to +18 dB
- Parallel absolute and relative master-volume entities
- Speaker Level Calibration controls using the SLC command group
- Speaker Level Test action using SLCTEST
- Speaker Level channel selection using SLCCHSEL
- Speaker Level increase using SLCUP
- Speaker Level decrease using SLCDOWN
- Configurable input source profiles
- Custom Home Assistant aliases for receiver inputs
- Optional disabling of individual input sources

#### Changed
- Absolute master volume remains available on the Onkyo 0 to 100 scale
- Absolute and relative volume entities represent and control the same receiver volume state
- Every MVL response updates both configured master-volume entities
- Input selection can be restricted to inputs actually available on the receiver
- Receiver input responses are translated back to configured Home Assistant aliases
- Input source customization is optional; configurations without sources continue to use the legacy full input list
- Speaker Level Calibration commands use the shared outbound command queue

#### Fixed
- Unsupported receiver inputs no longer need to be exposed in the Home Assistant input selector
- Known but disabled input codes no longer produce invalid select states
- Custom input aliases remain synchronized when the input is changed directly on the receiver

#### Notes
- Absolute and relative master-volume entities can be enabled and used simultaneously
- Relative master volume is calculated as absolute volume - 82
- The TX-SR608 volume scale uses 1 dB steps in the tested configuration
- Input profiles validate duplicate Onkyo names, duplicate display names and empty enabled profiles
- Speaker Level Calibration entities are action buttons and do not represent persistent calibration state

### 0.7.0

#### Added
- Audio Source structured diagnostic from IFA responses
- Audio Output Format structured diagnostic from IFA responses
- Video input color-space diagnostic
- Video input color-depth diagnostic
- Video output color-space diagnostic
- Video output color-depth diagnostic
- ESP uptime and runtime diagnostic monitoring in the reference configuration
- Home Assistant dashboard example using built-in cards
- Updated English ESP8266 reference configuration
- GPL-3.0-only project licensing and SPDX source identifiers

#### Changed
- Corrected the structured IFA field mapping to represent source, input format, sample rate, input channels and output format
- Expanded structured IFV parsing to expose color space and color depth for both input and output
- Cleaned up C++ and Python component code without changing the public YAML configuration API
- Updated protocol, pinout, command-verification and testing documentation
- Reference hardware documentation now recommends a stable external USB power supply

#### Fixed
- Removed the incorrect Audio Output Channels interpretation from IFA structured diagnostics
- Preserved receiver-provided values such as `UNKNOWN` and `None` while continuing to report missing information fields as `N/A`

#### Validation
- Completed intensive command and full-state-query regression testing
- Completed approximately 100 rapid master-volume changes without a reset
- Completed repeated full-state queries, including bursts with less than 15 seconds between requests
- Completed more than 48 hours of continuous operation; approximately 55 hours of diagnostic history were evaluated
- No unexpected ESP resets occurred during the long-term test after the power-supply replacement
- No progressive memory loss or heap fragmentation was observed
- Median free heap was approximately 31.9 kB
- Median largest free heap block was approximately 29.1 kB
- 99% of recorded maximum-loop-time values were 26 ms or less
- Wi-Fi/API interruptions recovered automatically without affecting receiver operation

#### Notes
- Repeated spontaneous `Power On` resets observed during development stopped after replacing the previous USB power supply; the previous supply is considered the most likely cause
- Detailed regression and long-term stability results are documented in `docs/testing.md`
- The Home Assistant master-volume slider can cause large and rapid volume changes
- A configurable runtime maximum-volume limit is planned for a future release but is not implemented in v0.7.0
- The ESPUI proof of concept remains experimental and is not included in the ESP8266 release because page initialization/reloads could trigger Soft WDT resets during testing

#### 0.8.0

##### Added
- Persistent runtime-configurable maximum master-volume entity
- Default maximum master-volume value of 60 on the absolute Onkyo scale
- Maximum-volume protection for native absolute volume, relative volume and Volume Up control
- Recognition of known TX-SR608 Zone 2 command groups: ZPW, ZMT, ZVL, ZTN, ZBL, SLZ, TUZ and PRZ

##### Changed
- Maximum master volume can be changed from Home Assistant without recompiling the firmware
- Lowering the configured maximum below the current receiver volume does not automatically change the current volume
- Volume Down remains available while the receiver is above the configured maximum
- Known Zone 2 notifications are handled as recognized auxiliary frames instead of unknown ISCP frames

##### Fixed
- TUZ Zone 2 tuner-frequency notifications no longer update Main Zone FM/AM frequency entities
- PRZ Zone 2 tuner-preset notifications no longer update the Main Zone tuner-preset entity
- Rapid native Volume Up actions cannot enqueue volume changes beyond the configured maximum

##### Notes
- Raw `onkyo_iscp.send` commands intentionally bypass the maximum-volume protection
- Zone 2 control is intentionally not exposed as native entities; only known Zone 2 frame recognition is implemented
- The maximum-volume setting is restored after an ESP restart
- 
### Roadmap

Development is focused on stable and well-tested support for the Onkyo TX-SR608. The goal is not to expose every model-dependent ISCP command, but to provide reliable Home Assistant entities for useful receiver functions while retaining raw-command access for advanced use cases.

#### v1.0.0: Stable release

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

#### Future considerations

The following features are intentionally outside the current pre-1.0 scope and may be considered later:

- Full Zone 2 control; known Zone 2 notifications are recognized, but control is intentionally not exposed
- Zone 3 support
- Network and USB playback
- Onkyo dock and iPod control
- HDMI and digital-input assignment
- Extended speaker setup
- Additional model-specific ISCP commands
- Standalone web interface, preferably on hardware with more runtime headroom or as an external frontend

Zone-related notifications may still be recognized to prevent them from appearing as unknown frames, even when full zone control is not implemented.
