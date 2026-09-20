# ESPUI Proof of Concept

## Goal

The proof of concept evaluated whether the ESPHome Onkyo ISCP
component could provide a standalone browser-based receiver remote
using ESPUI on the existing ESP8266 hardware.

## Tested functionality

The following functions were implemented and tested:

- Local ESPUI web interface
- Power control
- Mute control
- Absolute master-volume control
- Relative volume display
- Configurable input selection with aliases
- OSD navigation controls
- Bidirectional state synchronization
- Parallel Home Assistant and ESPUI operation

## Resource usage

Baseline without ESPUI:

- RAM: 39,156 bytes, 47.8%
- Flash: 465,561 bytes, 44.6%

ESPUI proof of concept:

- RAM: approximately 40,900 to 41,100 bytes, about 50%
- Flash: approximately 553,000 to 557,000 bytes, about 53%

Static RAM and flash usage remained acceptable.

## Result

The web interface and receiver controls worked successfully during
normal operation.

However, opening or reloading the ESPUI page could trigger Soft WDT
resets on the ESP8266. The issue became more reproducible with larger
control structures and multiple browser connections.

Reducing the interface to a minimal remote did not provide sufficient
reload stability for production use.

## Technical findings

- ESPUI and ESPHome can run together on the ESP8266.
- Home Assistant and UART communication continued to work in parallel.
- Runtime memory and WebSocket processing during page initialization
  were more critical than static RAM usage.
- The ESPUI control structure is transmitted when a browser connects.
- Large or repeated initial transfers can trigger the ESP8266 software
  watchdog.
- Additional timing, deferred-update and dependency workarounds would
  create a platform-specific and difficult-to-maintain integration.

## Decision

ESPUI support is not included in the ESP8266-based component.

The stable Onkyo ISCP component remains focused on UART communication,
Home Assistant entities and reliable long-term operation.

The ESPUI proof-of-concept branch and tag are retained for reference.

## Future option

A standalone web remote may be reconsidered using an ESP32 or an
external web application.

An ESP32 provides substantially more runtime headroom for:

- ESPUI
- WebSockets
- Multiple browser clients
- Larger control structures
- Parallel ESPHome API and receiver communication

Any future implementation should keep the web frontend separate from
the Onkyo ISCP backend and continue using the existing command queue.