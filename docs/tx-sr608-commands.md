# TX-SR608 ISCP Command Verification

This document records ISCP command groups that have been verified during development of the ESPHome Onkyo ISCP component.

The original command candidates were derived from the Integra DTR-30.2 protocol matrix, a closely related receiver from the same generation.

The focus of this document is the actual behavior observed on the TX-SR608.

## Verification Status

- Verified: Tested successfully on the TX-SR608
- Partial: Recognized or investigated, but not fully implemented
- Unsupported: Tested and found unsupported
- Experimental: Used for diagnostics or development

## Core Controls

Verified:

`PWR` - Power

`AMT` - Mute

`MVL` - Master Volume

`SLI` - Input Selector

`DIM` - Display Dimmer

`SLP` - Sleep Timer

`FLD` - Receiver Display

`DIF` - Display Mode and temporary signal 