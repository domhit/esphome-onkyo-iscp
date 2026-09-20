# TX-SR608 UART Pinout and Hardware Notes

## UART Test Points

The ESP8266 communicates directly with UART test points on the TX-SR608 mainboard.

images/tx-sr608-uart-testpoints.png

*TX-SR608 UART test points. Cross wiring: `FLRXD → ESP8266 TX (GPIO1)`, `FLTXD → ESP8266 RX (GPIO3)`, and `GND → GND`.*

## UART Wiring

Connect the receiver test points to the ESP8266 hardware UART:

- `FLRXD` → ESP8266 `GPIO1 / TX`
- `FLTXD` → ESP8266 `GPIO3 / RX`
- `GND` → ESP8266 `GND`

The signal names are shown from the receiver's perspective:

- `FLRXD` is the receiver RX signal and therefore connects to the ESP8266 TX pin.
- `FLTXD` is the receiver TX signal and therefore connects to the ESP8266 RX pin.
- `GND` must be shared between the receiver and ESP8266.

In other words, the UART data lines are cross-connected:

```text
TX-SR608                    ESP8266 / Wemos D1 Mini

FLRXD  <------------------- GPIO1 / TX
FLTXD  -------------------> GPIO3 / RX
GND     ------------------- GND
```

## Serial Configuration

The UART connection uses:

```text
Baud rate:  9600
Data bits:  8
Parity:     None
Stop bits:  1
Flow control: None
```

## ESP8266 Hardware

Development and testing were performed using a Wemos D1 Mini based on the ESP8266.

The hardware UART is used for ISCP communication:

- `GPIO1` → TX
- `GPIO3` → RX

Because GPIO1 and GPIO3 are used for the receiver connection, the ESPHome logger must not use the same UART for serial log output.

## Power Supply

The ESP8266 is powered independently through the USB connector of the Wemos D1 Mini.

Supplying the ESP8266 directly from the TX-SR608 was investigated but was not found to be reliable.

The following receiver standby rails were tested:

- `+12VD_ST`
- `+10VS`

Both rails are available while the receiver is in Standby.

However, when an ESP8266 was connected through an LM2596 buck converter, the supply voltage dropped to approximately 5 V and the receiver could no longer power on reliably.

For this reason, the ESP8266 currently uses an independent USB power supply.

## Hardware Notes

- Do not power the ESP8266 directly from unknown receiver power rails.
- Always verify voltage levels before connecting UART signals.
- Always use a common ground between the receiver and ESP8266.
- Keep UART wiring short and mechanically secured.
- Insulate solder joints and exposed conductors.
- Ensure that the ESP8266 and wiring do not obstruct receiver ventilation.
- Retain physical access to the ESP8266 USB connector for recovery or serial flashing.

## Installation Documentation

For a reproducible installation, the following should be documented:

- TX-SR608 mainboard UART test-point location
- `FLRXD` solder point
- `FLTXD` solder point
- `GND` solder point
- ESP8266 installation location
- Final UART wiring
- USB power connection
- Cable routing inside the receiver