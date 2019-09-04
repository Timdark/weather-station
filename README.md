# Weather-station

This project is about arduino internet weather station with inside temperature. Project is coded using VS code.

## Hardware

- Arduino mega
- mcufriend 3.5" TFT screen 320x480 pixels
- W5100 RJ45 internet adapter
- DS18B20 temperature sensor
- wires and one 4k7 resistor

**Example src/conf.h file**
This file contains openweathermap personal information. (apikey, latitude, longitude)

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
- more modular fuctions
