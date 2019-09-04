/********************************** DEFINES '****************************/
// LCD defines
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// SD Card defines
#define USE_SDFAT
#define SD_CS     10
#define NAMEMATCH ""         // "" matches any name
#define PALETTEDEPTH   8     // support 256-colour Palette

// LCD color defines
// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Temperature sensor data pin
#define ONE_WIRE_BUS 31

/******************************* LIBRARIES **********************************/

// Labrarys and classes inits
#include <Arduino.h>
#include <SPI.h>
#include <TimeLib.h>
#include <Timezone.h>
#include <Ethernet.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>   // Hardware-specific library
#include <MCUFRIEND_kbv.h>

#include <OneWire.h> 
#include <DallasTemperature.h>

#include "bitmaps.h"        // weather icons bitmaps

#include "conf.h"           // openweathermap api stuff

// initialize the library instance:
EthernetClient client;
MCUFRIEND_kbv tft;
OneWire onewire(ONE_WIRE_BUS);
DallasTemperature sensors(&onewire);

 /******************************* GLOBAL STUFF **********************************/

// assign a MAC address for the ethernet controller.
// fill in your address here:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

// Server address
char server[] = "api.openweathermap.org";

// Weather data
String currentTemp = "";
String currentIcon;
String currentDesc;
time_t currentTime;

// Room temperature
String insideTemp;

// Set timezone
TimeChangeRule fiWinter = {"SUM", Last, Sun, Oct, 3, 120};
TimeChangeRule fiSummer = {"WIN", Last, Sun, Mar, 3, 180};
Timezone fi(fiSummer, fiWinter);
 
// fuctiot
void getWeather();
void parseWeatherJson(String jsonData);
void getForecast();
void parseForecastJson(String jsonData);
void printData();
void printIcon(int x, int y, String name);
void getTemp();
void setBackground();

/**************************** STARTUP SETUP *****************************/

void setup() {
  // start serial port:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // SS pin
  Ethernet.init(53);

  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }

  // give the Ethernet shield a second to initialize:
  delay(1000);

  // LCD INIT
  uint16_t ID = tft.readID(); //
  Serial.print("ID = 0x");
  Serial.println(ID, HEX);

  if (ID == 0xD3D3) ID = 0x9481; // write-only shield

  tft.begin(ID);
  tft.fillScreen(BLACK);
  tft.setRotation(1);

  // Start temp sensor
  sensors.begin();
}

/******************************* MAIN LOOP ****************************/

// MAIN LOOP
void loop() {
  getWeather();
  //getForecast();
  getTemp();
  printData();
  delay(600000);
}

/****************************** FUCTIONS *******************************/

// Getting current weather from server:
void getWeather() {
  String jsonData = "";

  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP GET request:
    client.println("GET /data/2.5/weather?lat=" + lat + "&lon=" + lon + "&units=metric&appid=" + apiKey);
    client.println("Host: http://api.openweathermap.org");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();

  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }

  delay(2000); // Waitig data

  if (client.available()) {
    jsonData = client.readStringUntil('\n');
    Serial.println(jsonData);

    parseWeatherJson(jsonData);
  }
}

// Parse weather JSON data
void parseWeatherJson(String jsonData) {
  Serial.println("parsingValues");

  StaticJsonDocument<5000> root;

  deserializeJson(root, jsonData);

  currentTemp = root["main"]["temp"].as<String>();
  currentIcon = root["weather"][0]["icon"].as<String>();
  currentDesc = root["weather"][0]["description"].as<String>();

  TimeChangeRule *tcr;

  currentTime = fi.toLocal(root["dt"], &tcr);
}

// Getting forecast data
void getForecast(){
  String jsonData = "";

  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP GET request:
    client.println("GET /data/2.5/forecast?lat=" + lat + "&lon=" + lon + "&units=metric&cnt=10&appid=" + apiKey);
    client.println("Host: http://api.openweathermap.org");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();

  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }

  delay(2000); // Waitig data

  if (client.available()) {
    jsonData = client.readStringUntil('\n');
    Serial.println(jsonData);

    //parseWeatherJson(jsonData);
  }
}

// Parse forecast data
void parseForecastJson(String jsonData) {
  Serial.println("parsingValues");

  StaticJsonDocument<5000> root;

  deserializeJson(root, jsonData);

  // currentTemp = root["main"]["temp"].as<String>();
  // currentIcon = root["weather"][0]["icon"].as<String>();
  // currentDesc = root["weather"][0]["description"].as<String>();
}

// Print weather icon
void printIcon(int x, int y, String name) {
  if (name == "01d") {
    tft.drawBitmap(x,y,bitmap_01d,50,50,WHITE);
  } else if (name == "01n") {
    tft.drawBitmap(x,y,bitmap_01n,50,50,WHITE);
  } else if (name == "02d") {
    tft.drawBitmap(x,y,bitmap_02d,50,50,WHITE);
  } else if (name == "02n") {
    tft.drawBitmap(x,y,bitmap_02n,50,50,WHITE);
  } else if (name == "03d") {
    tft.drawBitmap(x,y,bitmap_03d,50,50,WHITE);
  } else if (name == "03n") {
    tft.drawBitmap(x,y,bitmap_03n,50,50,WHITE);
  } else if (name == "04d") {
    tft.drawBitmap(x,y,bitmap_04d,50,50,WHITE);
  } else if (name == "04n") {
    tft.drawBitmap(x,y,bitmap_04n,50,50,WHITE);
  } else if (name == "09d") {
    tft.drawBitmap(x,y,bitmap_09d,50,50,WHITE);
  } else if (name == "09n") {
    tft.drawBitmap(x,y,bitmap_09n,50,50,WHITE);
  } else if (name == "10d") {
    tft.drawBitmap(x,y,bitmap_10d,50,50,WHITE);
  } else if (name == "10n") {
    tft.drawBitmap(x,y,bitmap_10n,50,50,WHITE);
  } else if (name == "11d") {
    tft.drawBitmap(x,y,bitmap_11d,50,50,WHITE);
  } else if (name == "11n") {
    tft.drawBitmap(x,y,bitmap_11n,50,50,WHITE);
  } else if (name == "13d") {
    tft.drawBitmap(x,y,bitmap_13d,50,50,WHITE);
  } else if (name == "13n") {
    tft.drawBitmap(x,y,bitmap_13n,50,50,WHITE);
  } else if (name == "50d") {
    tft.drawBitmap(x,y,bitmap_50d,50,50,WHITE);
  } else if (name == "50n") {
    tft.drawBitmap(x,y,bitmap_50n,50,50,WHITE);
  }
}

void getTemp() {
  sensors.requestTemperatures();
  insideTemp = sensors.getTempCByIndex(0);
}

void setBackground() {
  if(hour(currentTime) <= 6) {
    tft.setCursor(5,200);
    tft.println("night");
  } else if (hour(currentTime) <= 12) {
    tft.setCursor(5,200);
    tft.println("morning");
  } else if (hour(currentTime) <= 18) {
    tft.setCursor(5,200);
    tft.println("day");
  } else {
    tft.setCursor(5,200);
    tft.println("evening");
  }
}

// Print Data to LCD
void printData() {
  tft.fillScreen(BLACK);  // clear screen + set background black
  tft.setTextColor(WHITE);

  // make boxes
  tft.drawLine(0,100,480,100,WHITE);
  tft.drawLine(240,0,240,50,WHITE);
  tft.drawLine(240,50,480,50,WHITE);

  // show current temp
  printIcon(5,5,currentIcon);
  tft.setTextSize(3);
  tft.setCursor(70,20);
  tft.println(currentTemp + " " + (char)247 + "C");
  tft.setCursor(0,60);
  tft.println(currentDesc);
  

  // show inside temp
  tft.setCursor(260,5);
  tft.println(insideTemp + " " + (char)247 + "C");

  tft.setCursor(5,300);
  tft.println(hour(currentTime));
  setBackground();
  //tft.println(hour(currentTime));

  // show forecast
}
