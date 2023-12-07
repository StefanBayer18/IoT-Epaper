# IoT-Epaper
Beispielcode https://github.com/waveshareteam/e-Paper/tree/master/Arduino/epd7in5b_V2

Grober Plan

main
- Klassen initalisieren
- connect to wifi()
- putEspToSleep 15 min


DisplayDriver
- Display Image()
- Sleep() -> IMMER nach image update
- waitBusy()
- sendCommand()
- reset()
- sendData()

ImageDriver
- imageArray
- addtext()
- addImage()
- addLine() / addRect()
- addPoint()
- addGraph() -- graph unten Rechts für Temperatur im Zeitverlauf

DatenGetter
- Datenbank verbinden()
- Datenbank auslesen()
- Api auslesen()
- wie deren Daten speichern? -> Temp. verlauf in 8kB SRAM speichern
