# Protokollnotizen

Serielle Parameter nach bisherigem Projektstand:

```text
9600 Baud
8 Datenbits
keine Parität
1 Stopbit
```

Frameformat:

```text
!1<COMMAND><VALUE>\r
```

Beispiele:

```text
!1PWRQSTN\r
!1PWR01\r
!1MVL30\r
!1SLI10\r
```

Der Parser trennt die ersten drei Zeichen als Befehlsgruppe und den Rest als Wert. Neue Gruppen werden in `process_command_()` ergänzt.
