### v0.7.0 Regression Test

#### Test environment

- Hardware: ESP8266 / NodeMCU
- Receiver: Onkyo TX-SR608
- UART: 9600 baud, 8N1
- ESPHome: 2026.9.0
- Branch: `dev/v0.7.0`
- Power supply: Amazon Fire TV 4K Max power supply
- Test start: 2026-09-21 05:49 CEST

#### Background

During development, sporadic ESP8266 restarts were observed.

Diagnostic monitoring was added for:

- ESP uptime
- reset reason
- free heap
- largest free heap block
- heap fragmentation
- maximum loop time

Before the power supply was changed, multiple spontaneous restarts were recorded with the reset reason `Power On`.

The power supply was replaced at 05:49 CEST without changing the USB cable, firmware, UART wiring, or ESPHome configuration.

#### Stability test

After changing the power supply, the device was initially left running under normal operating conditions.

At an uptime of approximately 8311 seconds (2 h 18 min), an intensive stress/regression test was performed.

The following operations were tested:

- All available receiver controls exercised
- Input switching
- Listening mode changes
- Audio settings
- Video settings
- OSD controls
- Audio and video information queries
- Approximately 15-20 full status queries
- Some full status queries executed less than 15 seconds apart
- Approximately 100 rapid master-volume changes

#### Stress-test diagnostic results

Free heap remained approximately in the 28-32 kB range during the stress test and recovered after temporary load.

The largest free heap block remained approximately in the 25-30 kB range and also recovered after temporary load.

Maximum loop time was normally approximately 18-30 ms.

One isolated loop-time peak of approximately 148 ms was observed under heavy load. The loop time returned to approximately 21 ms immediately afterwards.

No progressive memory loss was observed.

No progressive reduction of the largest free heap block was observed.

No reset occurred during the stress test.

#### Long-term stability results

The device was then left running without firmware changes. More than 48 hours of continuous operation were completed successfully; approximately 55 hours of diagnostic history were evaluated.

Diagnostic results over the recorded period:

- Unexpected ESP resets: 0
- Unexpected `Power On` resets: 0
- Receiver communication remained stable
- Free heap median: approximately 31.9 kB
- Free heap minimum: approximately 28.1 kB
- Largest free heap block median: approximately 29.1 kB
- Largest free heap block minimum: approximately 23.4 kB
- Normal maximum loop time: approximately 18-24 ms
- 99% of recorded loop-time values: 26 ms or less
- Progressive memory loss: not observed
- Progressive heap fragmentation: not observed
- Progressive reduction of the largest free heap block: not observed

Two brief Home Assistant/API availability interruptions were observed during the recorded period.

One interruption coincided with a planned Wi-Fi access-point restart. The ESP reconnected automatically and normal operation resumed immediately.

A second brief API interruption also recovered automatically. The diagnostic entities became unavailable for less than one second and immediately returned with normal heap and loop-time values. No ESP reset was observed.

The largest loop-time peak was approximately 891 ms during the planned access-point restart. A second reconnect-related peak of approximately 155 ms was observed during the other brief API interruption. In both cases, loop time immediately returned to the normal range.

#### Result

**PASS**

The `dev/v0.7.0` implementation remained stable during intensive receiver control, repeated full-state synchronization, normal daily operation, and more than 48 hours of continuous runtime.

The previously observed repeated `Power On` resets did not recur after replacing the power supply. The previous power supply is therefore considered the most likely cause of the spontaneous resets observed during development.

No evidence of a memory leak, progressive heap fragmentation, command-queue instability, or long-term UART instability was observed.

Final long-term result: **PASS**

#### v0.8.0 Feature Regression Test

##### Test environment
- Hardware: ESP8266 / NodeMCU
- Receiver: Onkyo TX-SR608
- UART: 9600 baud, 8N1
- ESPHome: 2026.9.0
- Branch: dev/v0.8.0

##### Maximum-volume protection

The runtime-configurable maximum-volume feature was tested with the native
absolute and relative master-volume entities and the Volume Up/Down controls.

Verified behavior:
- Maximum volume is exposed as a Home Assistant number entity
- Default maximum is 60 on the absolute Onkyo volume scale
- The limit can be changed at runtime without recompilation
- The selected maximum is stored persistently and restored after an ESP restart
- Absolute volume requests above the limit are clamped to the configured maximum
- Relative volume requests obey the same maximum after conversion to the absolute scale
- Volume Up stops at the configured maximum
- Volume Down remains available while the current receiver volume is above the configured maximum
- Lowering the maximum below the current receiver volume does not automatically change the current volume
- Raw `onkyo_iscp.send` commands intentionally bypass the maximum-volume protection

Result: **PASS**

##### Zone 2 frame recognition

Zone 2 was investigated using raw ISCP commands against the TX-SR608.
Native Zone 2 control is intentionally outside the v0.8.0 scope.
The purpose of the test was to identify normal Zone 2 notifications and
prevent them from being reported as unknown ISCP frames.

Observed responses:
- `ZPWQSTN` returned `ZPW00` and `ZPW01`
- `ZMTQSTN` returned `ZMT00` and `ZMT01`
- `ZVLQSTN` returned `ZVL40`
- `ZVL28` returned `ZVLN/A` in the tested receiver configuration
- `SLZQSTN` returned `SLZ80`
- Selecting FM with `SLZ24` returned `SLZ24`
- `ZTNQSTN` returned `ZTNN/A`
- `ZBLQSTN` returned `ZBLN/A`
- `TUZQSTN` returned `TUZ08800`
- `PRZQSTN` returned `PRZ01`

The command groups `ZPW`, `ZMT`, `ZVL`, `ZTN`, `ZBL`, `SLZ`, `TUZ`
and `PRZ` are recognized as known Zone 2 frames.

`TUZ` and `PRZ` are handled separately from Main Zone tuner state and
therefore no longer update the Main Zone frequency or tuner-preset entities.

No native Zone 2 control entities are implemented.

Result: **PASS**

##### v0.8.0 result

**PASS**

The v0.8.0 feature scope is complete. Runtime-configurable maximum-volume
protection operates as intended and known Zone 2 notifications are handled
without adding unsupported Zone 2 control entities.