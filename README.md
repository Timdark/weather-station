# Weather-station

This project is about Arduino internet weather station with inside temperature. Project is coded using VS code.

## Hardware

- Arduino mega
- Mcufriend 3.5" TFT screen 320x480 pixels
- W5100 RJ45 Ethernet adapter
- DS18B20 temperature sensor
- Wires and one 4k7 resistor

**Example src/conf.h file**
This file contains OpenWeatherMap personal information. (apikey, latitude, longitude)

```
#pragma once

#include <Arduino.h>

// OpenWeatherMap api stuff
String apiKey = "";
String lat = "";
String lon = "";
```

## TODO

- forecast
- different time new data get (OWM and room temp)
- background image
- object placement on the screen
- more modular functions
