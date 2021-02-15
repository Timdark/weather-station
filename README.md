# Weather-station

This project is about Arduino internet weather station with inside temperature. Project is coded using VS code. 
Project using Weatherbit.io free weather/forecast service.

## Features

- Get internet weather and forecast
- Get inside temperature using DS18B20 temperature sensor
- Prints feels-like temperatures to 3.5 inch tft screen
- Auto ethernet recall
    - Tells if no internet connection
- Change background image every 6 hours
    - Font color change between black and white, debends background
- Weather condition icons
- Day name texts

## Screen shots

![IMG_20200128_232720](https://user-images.githubusercontent.com/32216377/107982292-6c8b1d80-6fcc-11eb-91ae-5bddfc8bfa92.jpg)
![IMG_20200129_000804](https://user-images.githubusercontent.com/32216377/107982317-7a40a300-6fcc-11eb-8122-ca9d2200fa83.jpg)
![IMG_20200129_140229](https://user-images.githubusercontent.com/32216377/107982321-7ca2fd00-6fcc-11eb-9757-b677024b12d9.jpg)
![IMG_20200129_142453](https://user-images.githubusercontent.com/32216377/107982328-7e6cc080-6fcc-11eb-92eb-39783cee7485.jpg)

## Secret variables file

Create conf.h file src/ folder. This file contains your Weatherbit apikey and your location.

**Example src/conf.h file**
This file contains Weatherbit personal information. (apikey, latitude, longitude)

```
#pragma once

#include <Arduino.h>

// Weatherbit api stuff
String apiKey = "";
String lat = "";
String lon = "";
```

## Hardware

- Arduino mega
- Mcufriend 3.5" TFT screen 320x480 pixels
- W5100 RJ45 Ethernet adapter
- DS18B20 temperature sensor
- Wires and one 4k7 resistor

## Libraries
- Arduino Time library          https://github.com/PaulStoffregen/Time?utm_source=platformio&utm_medium=piohome
- Arduino Timezone library      https://github.com/JChristensen/Timezone
- Arduino JPEGDecoder library   https://github.com/Bodmer/JPEGDecoder
    - This library source files have custom modification (removed SD card support)
- MCUFRIEND_kbv library         https://github.com/prenticedavid/MCUFRIEND_kbv
- ArduinoJson library           https://arduinojson.org/
- Adafruit_GFX library          https://github.com/adafruit/Adafruit-GFX-Library
- DallasTemperature library     https://github.com/milesburton/Arduino-Temperature-Control-Library
- OneWire library               https://github.com/PaulStoffregen/OneWire
- Arduino & SPI library
- Ethernet library

## Image converting tools

- File to hexadecimal converter     http://tomeko.net/online_tools/file_to_hex.php?lang=en
