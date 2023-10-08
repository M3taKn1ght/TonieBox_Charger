/****** INCLUDES ******/
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_INA219.h>
#include <Adafruit_NeoPixel.h>

/****** DEFINES ******/
/****** TIMER ******/
#define DEFUPDATEINTERVAL 2000
#define DEFUPDATETIMECIRVLE 100
#define DEFBRIGTHNESS       100
/****** NEOPIXEL ******/
#ifdef D1MINI
    #define DEFNEOPIN 0     //Pin D3
#endif
#ifdef ARDUINO_UNO
    #define DEFNEOPIN 8     //Pin D3
#endif

#define DEFNUMLEDS 32   //Number of LEDs
#define DEFMAXHUE 256 * 6
/****** INA219 ******/
#define DEFTHRESHOLDLOADED 50
#define DEFTHRESHOLDLOAD 400

/****** FUNCTIONS ******/
/*
 * Function: setup
 * Description: setup-function
 * Return: void
 */
void setup();

/*
 * Function: loop
 * Description: loop-function
 * Return: void
 */
void loop();

/*
 * Function: checkParameters
 * Description: Get latest paramaters from INA219
 * Return: void
 */
void checkParameters();

/*
 * Function: recolorNeopixel
 * Description: Get latest paramaters from INA219
 * IN bLoaded: Bool to that indicate loaded or not
 * Return: void
 */
void recolorNeopixel(bool bLoaded = false);

/*
 * Function from https://github.com/RoboUlbricht/arduinoslovakia/blob/master/neopixel/hsv_rainbow_circle_rotate/hsv.h
 * Based from: RoboUlbricht
 */
uint32_t getPixelColorHsv(uint16_t n, uint16_t h, uint8_t s, uint8_t v);

Adafruit_INA219 ina219;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(DEFNUMLEDS, DEFNEOPIN, NEO_GRB + NEO_KHZ800);
float shuntvoltage = 0;
float busvoltage = 0;
float current_mA = 0;
float loadvoltage = 0;
float power_mW = 0;
int iPosition = 0;
int iHue = 0;