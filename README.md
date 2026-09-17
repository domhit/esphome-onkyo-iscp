# ESPHome Onkyo ISCP

Grundgerüst einer externen ESPHome-Komponente für die serielle 3,3-V-UART-Schnittstelle älterer Onkyo-Receiver, zunächst für den TX-SR608.

## Aktueller Funktionsumfang

- Power lesen/schalten
- Mute lesen/schalten
- Master-Volume als Rohwert lesen/setzen
- Eingang lesen/setzen
- Lauter/Leiser
- zyklische Statusabfrage
- letzter empfangener ISCP-Frame
- generische Action `onkyo_iscp.send` für noch nicht implementierte Befehle

## Struktur

```text
components/onkyo_iscp/  Externe ESPHome-Komponente
examples/               Beispielkonfigurationen
docs/                   Hardware- und Protokolldokumentation
```

## Lokale Entwicklung

Das Beispiel bindet `../components` lokal ein. Aus dem Repository-Stamm:

```bash
esphome config examples/tx-sr608-esp8266.yaml
esphome compile examples/tx-sr608-esp8266.yaml
```

Für die Nutzung direkt aus GitHub später den Block `external_components` auf die Git-URL umstellen.

## Hardware

Für den ESP8266 ist UART0 auf GPIO1 TX und GPIO3 RX vorgesehen. Deshalb ist der serielle Logger deaktiviert. Vor USB-Flashvorgängen RX/TX zum Onkyo trennen; danach OTA verwenden.

Die angenommene Onkyo-Belegung muss vor dem Anschluss messtechnisch bestätigt werden:

- Onkyo Prozessor-RX an ESP8266 GPIO1 TX
- Onkyo Prozessor-TX an ESP8266 GPIO3 RX
- gemeinsame Masse
- keine Versorgung über den 3,3-V-Pin des Writing Ports

## Hinweis

Dies ist bewusst ein entwickelbares Grundgerüst. Eingangscodes und die Umrechnung des Lautstärke-Rohwerts in dB müssen am TX-SR608 praktisch validiert werden.
