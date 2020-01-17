/********************************** DEFINES '****************************/
// LCD defines
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// LCD color defines
// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
//#define BLUE    0x001F
//#define RED     0xF800
//#define GREEN   0x07E0
//#define CYAN    0x07FF
//#define MAGENTA 0xF81F
//#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Temperature sensor data pin
#define ONE_WIRE_BUS 24

/******************************* LIBRARIES **********************************/

// Labraries and classes inits
#include <Arduino.h>
#include <SPI.h>

#include <TimeLib.h>
#include <Timezone.h>

#include <Adafruit_GFX.h>  
#include <MCUFRIEND_kbv.h>
#include <JPEGDecoder.h>

#include <OneWire.h> 
#include <DallasTemperature.h>

#include "bitmaps-icons.h"        // weather icons bitmaps
#include "bitmaps-backgrounds.h"  // backgrounds

#include "conf.h"           // weatherbit api stuff

#include "Weatherbit.h"        // ethernet connect + data object

// initialize the library instance:
Weatherbit WB(apiKey, lat, lon);
MCUFRIEND_kbv tft;
OneWire onewire(ONE_WIRE_BUS);
DallasTemperature sensors(&onewire);

/******************************* GLOBAL STUFF **********************************/

unsigned long timeTemp = 0;   // store last millis time

bool light_dark = true;       // Is back gound dark or light? light = false, dark = True

// Room temperature
String insideTemp;

// Set timezone
TimeChangeRule fiWinter = {"SUM", Last, Sun, Oct, 3, 120};
TimeChangeRule fiSummer = {"WIN", Last, Sun, Mar, 3, 180};
Timezone fi(fiSummer, fiWinter);
 
// functions
void printData();
void printIcon(int x, int y, String name);
void getInsideTemp();
void setBackground();
String whatWeekday(long epoch);

void drawArrayJpeg(const uint8_t arrayname[], uint32_t array_size, int xpos, int ypos);
void jpegRender(int xpos, int ypos);

/**************************** JPEG Functions ****************************/

// Return the minimum of two values a and b
#define minimum(a,b)     (((a) < (b)) ? (a) : (b))

//====================================================================================
//   This function opens the array and primes the decoder
//====================================================================================
void drawArrayJpeg(const uint8_t arrayname[], uint32_t array_size, int xpos, int ypos) {

  boolean decoded = JpegDec.decodeArray(arrayname, array_size);

  if (decoded) {
    // render the image onto the screen at given coordinates
    jpegRender(xpos, ypos);
  }
  else {
    Serial.println("Jpeg file format not supported!");
  }
}

//====================================================================================
//   Decode and paint onto the TFT screen
//====================================================================================
void jpegRender(int xpos, int ypos) {

  // retrieve infomration about the image
  uint16_t  *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;
  uint16_t mcu_h = JpegDec.MCUHeight;
  uint32_t max_x = JpegDec.width;
  uint32_t max_y = JpegDec.height;

  // Jpeg images are draw as a set of image block (tiles) called Minimum Coding Units (MCUs)
  // Typically these MCUs are 16x16 pixel blocks
  // Determine the width and height of the right and bottom edge image blocks
  uint32_t min_w = minimum(mcu_w, max_x % mcu_w);
  uint32_t min_h = minimum(mcu_h, max_y % mcu_h);

  // save the current image block size
  uint32_t win_w = mcu_w;
  uint32_t win_h = mcu_h;

  // save the coordinate of the right and bottom edges to assist image cropping
  // to the screen size
  max_x += xpos;
  max_y += ypos;

  // read each MCU block until there are no more
  while ( JpegDec.read()) {

    // save a pointer to the image block
    pImg = JpegDec.pImage;

    // calculate where the image block should be drawn on the screen
    int mcu_x = JpegDec.MCUx * mcu_w + xpos;
    int mcu_y = JpegDec.MCUy * mcu_h + ypos;

    // check if the image block size needs to be changed for the right and bottom edges
    if (mcu_x + mcu_w <= max_x) win_w = mcu_w;
    else win_w = min_w;
    if (mcu_y + mcu_h <= max_y) win_h = mcu_h;
    else win_h = min_h;

    // calculate how many pixels must be drawn
    uint16_t mcu_pixels = win_w * win_h;

    // draw image MCU block only if it will fit on the screen
    if ( ( mcu_x + win_w) <= tft.width() && ( mcu_y + win_h) <= tft.height())
  {
      // Now set a MCU bounding window on the TFT to push pixels into (x, y, x + width - 1, y + height - 1)
      tft.setAddrWindow(mcu_x, mcu_y, mcu_x + win_w - 1, mcu_y + win_h - 1);

      // Write all MCU pixels to the TFT window
      //while (mcu_pixels--) tft.pushColor(*pImg++); // Send MCU buffer to TFT 16 bits at a time
      tft.pushColors(pImg, mcu_pixels, 1);
    }

    // Stop drawing blocks if the bottom of the screen has been reached,
    // the abort function will close the file
    else if ( ( mcu_y + win_h) >= tft.height()) JpegDec.abort();
  }
}

/**************************** STARTUP SETUP *****************************/

void setup() {
  // start serial port:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  WB.init();
  WB.update();

  // LCD INIT
  uint16_t ID = tft.readID(); 

  if (ID == 0xD3D3) ID = 0x9481; // write-only shield

  tft.begin(ID);
  tft.fillScreen(BLACK);
  tft.setRotation(1);

  // Start temp sensor
  sensors.begin();

  setBackground();
}

/******************************* MAIN LOOP ****************************/

// MAIN LOOP
void loop() {
  if(millis() >= (timeTemp + 3600000)){    // 1h = 3600000
    WB.update();
    //setBackground();
    timeTemp = millis();
  }

  getInsideTemp();

  printData();

  delay(10000);
}

/****************************** FUCTIONS *******************************/

void getInsideTemp() {
  sensors.requestTemperatures();
  insideTemp = sensors.getTempCByIndex(0);
}

void setBackground() {            
  time_t utc = WB.getEpochNow();
  time_t localTime = fi.toLocal(utc);

  if(hour(localTime) <= 6) {            // Yo
    drawArrayJpeg(yo, sizeof(yo),0,0);
    light_dark = false;
  } else if (hour(localTime) <= 12) {   // Aamu
    drawArrayJpeg(aamu, sizeof(aamu),0,0);
    light_dark = true;
  } else if (hour(localTime) <= 18) {    // Päivä
    drawArrayJpeg(paiva, sizeof(paiva),0,0);
    light_dark = true;
  } else if (hour(localTime) <= 24){     // Ilta
    drawArrayJpeg(ilta, sizeof(ilta),0,0);
    light_dark = false;
  }
}

// Print weather icon
void printIcon(int x, int y, String name) {
  if (name == "c01d") {  // 01d
    tft.drawBitmap(x,y,bitmap_01d,50,50,WHITE);
  } else if (name == "c01n") {   // 01n
    tft.drawBitmap(x,y,bitmap_01n,50,50,WHITE);
  } else if (name == "c02d") {   // 02d
    tft.drawBitmap(x,y,bitmap_02d,50,50,WHITE);
  } else if (name == "c02n") {   // 02n
    tft.drawBitmap(x,y,bitmap_02n,50,50,WHITE);
  } else if (name == "c03d") {   // 03d
    tft.drawBitmap(x,y,bitmap_03d,50,50,WHITE);
  } else if (name == "c03n") {   // 03n
    tft.drawBitmap(x,y,bitmap_03n,50,50,WHITE);
  } else if (name == "c04d") {   // 04d
    tft.drawBitmap(x,y,bitmap_04d,50,50,WHITE);
  } else if (name == "c04n") {   // 04n
    tft.drawBitmap(x,y,bitmap_04n,50,50,WHITE);
  } else if (name == "d01d" || name == "d02d" || name == "d03d" || name == "r01d" || name == "r02d" || name == "r03d" || name == "f01d") {   // 09d
    tft.drawBitmap(x,y,bitmap_09d,50,50,WHITE);
  } else if (name == "d01n" || name == "d02n" || name == "d03n" || name == "r01n" || name == "r02n" || name == "r03n" || name == "f01d") {   // 09n
    tft.drawBitmap(x,y,bitmap_09n,50,50,WHITE);
  } else if (name == "r04d" || name == "r05d" || name == "r06d") {   // 10d
    tft.drawBitmap(x,y,bitmap_10d,50,50,WHITE);
  } else if (name == "r04n" || name == "r05n" || name == "r06n") {   // 10n
    tft.drawBitmap(x,y,bitmap_10n,50,50,WHITE);
  } else if (name == "t01d" || name == "t02d" || name == "t03d" || name == "t04d" || name == "t05d") {   // 11d
    tft.drawBitmap(x,y,bitmap_11d,50,50,WHITE);
  } else if (name == "t01n" || name == "t02n" || name == "t03n" || name == "t04n" || name == "t05n") {   // 11n
    tft.drawBitmap(x,y,bitmap_11n,50,50,WHITE);
  } else if (name == "s01d" || name == "s02d" || name == "s03d" || name == "s04d" || name == "s05d" || name == "s06d") {   // 13d
    tft.drawBitmap(x,y,bitmap_13d,50,50,WHITE);
  } else if (name == "s01n" || name == "s02n" || name == "s03n" || name == "s04n" || name == "s05n" || name == "s06n") {   // 13n
    tft.drawBitmap(x,y,bitmap_13n,50,50,WHITE);
  } else if (name == "a01d" || name == "a02d" || name == "a03d" || name == "a04d" || name == "a05d" || name == "a06d") {   // 50d
    tft.drawBitmap(x,y,bitmap_50d,50,50,WHITE);
  } else if (name == "a01n" || name == "a02n" || name == "a03n" || name == "a04n" || name == "a05n" || name == "a06n") {   // 50n
    tft.drawBitmap(x,y,bitmap_50n,50,50,WHITE);
  }
}

String whatWeekday(long epoch){
  time_t utc = epoch;
  time_t localTime = fi.toLocal(utc);

  int temp = weekday(localTime);

  if(temp == 1){
    return "Sun";
  } else if (temp == 2){
    return "Mon";
  } else if (temp == 3){
    return "Tue";
  } else if (temp == 4){
    return "Wed";
  } else if (temp == 5){
    return "Thu";
  } else if (temp == 6){
    return "Fri";
  } else if (temp == 7){
    return "Sat";
  }
}

// Print Data to LCD
void printData() {    
  String icon0Temp = WB.getDay0Icon();
  String icon1Temp = WB.getDay1Icon();
  String icon2Temp = WB.getDay2Icon();

  if(light_dark){
    tft.setTextColor(BLACK);
  } else {
    tft.setTextColor(WHITE);
  }
  
  // show current temp
  printIcon(70,160,icon0Temp);
  tft.setTextSize(3);
  tft.setCursor(130,175);
  tft.println(WB.getAppTempNow() + " " + (char)247 + "C");
  tft.setTextSize(2);
  tft.setCursor(280,170);
  tft.println(WB.getDay0AppTempMax() + " " + (char)247 + "C");
  tft.setCursor(280,190);
  tft.println(WB.getDay0AppTempMin() + " " + (char)247 + "C");

  // forecast day 1
  printIcon(10,260,icon1Temp);
  tft.setTextSize(2);
  tft.setCursor(100,230);
  tft.println(whatWeekday(WB.getDay1Epoch()));
  tft.setCursor(70,260);
  tft.println(WB.getDay1AppTempMax() + " " + (char)247 + "C");
  tft.setCursor(70,290);
  tft.println(WB.getDay1AppTempMin() + " " + (char)247 + "C");

  // forecast day 2
  printIcon(250,260,icon2Temp);
  tft.setTextSize(2);
  tft.setCursor(340,230);
  tft.println(whatWeekday(WB.getDay2Epoch()));
  tft.setCursor(310,260);
  tft.println(WB.getDay2AppTempMax() + " " + (char)247 + "C");
  tft.setCursor(310,290);
  tft.println(WB.getDay1AppTempMin() + " " + (char)247 + "C");

  // show inside temp
  tft.setTextSize(2);
  tft.setCursor(250,5);
  tft.println(insideTemp + " " + (char)247 + "C");
}
