## v0.7.0 Regression Test

### Test environment

- Hardware: ESP8266 / NodeMCU
- Receiver: Onkyo TX-SR608
- UART: 9600 baud, 8N1
- ESPHome: 2026.9.0
- Branch: `dev/v0.7.0`
- Power supply: Amazon Fire TV 4K Max power supply
- Test start: 2026-09-21 05:49 CEST

### Background

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

### Stability test

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

### Diagnostic results

Free heap remained approximately in the 28-32 kB range during the stress test and recovered after temporary load.

The largest free heap block remained approximately in the 25-30 kB range and also recovered after temporary load.

Maximum loop time was normally approximately 18-30 ms.

One isolated loop-time peak of approximately 148 ms was observed under heavy load. The loop time returned to approximately 21 ms immediately afterwards.

No progressive memory loss was observed.

No progressive reduction of the largest free heap block was observed.

No reset occurred during the stress test.

### Result

**PASS**

The current `dev/v0.7.0` implementation remained stable during intensive receiver control and repeated full-state synchronization.

The previously observed repeated `Power On` resets have not occurred since replacing the power supply.

The previous power supply is therefore considered the primary suspect for the spontaneous resets. This conclusion remains provisional until the long-term stability test is completed.

### Long-term test

The device will continue running without firmware changes.

Acceptance criteria:

- 24 h uptime without unexpected reset
- 48 h uptime without unexpected reset
- No unexpected `Power On` reset reason
- No progressive heap loss
- No progressive heap fragmentation
- Normal receiver operation
- Stable UART communication

Final long-term result: **PENDING**