# Geplanter Anschluss

Nach aktuellem Stand des Serviceplans:

- P751A Pin 3: FSI/FLRXD, Prozessor-RX, Verbindung zum ESP-TX
- P751A Pin 7: FSO/FLTXD, Prozessor-TX, Verbindung zum ESP-RX
- P751A Pin 8: vermutete Masse, vor Anschluss messen
- P751A Pin 5: +3.3VMPU, nicht zur ESP-Versorgung verwenden

## Vor Anschluss verifizieren

1. Receiver stromlos und vom Netz getrennt.
2. Widerstand Pin 8 gegen Gehäusemasse messen. Erwartung nahe 0 Ohm.
3. Receiver unter Beachtung der elektrischen Sicherheit einschalten.
4. Spannung Pin 5 gegen Pin 8 messen. Erwartung etwa 3,3 V.
5. UART-Leitungen prüfen. Keine echte RS-232-Pegelwandlung anschließen, solange 3,3-V-TTL nicht bestätigt ist.
