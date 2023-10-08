#include "main.h"

void setup()
{
  Serial.begin(115200);
  while (!Serial)
  {
    ;
  }

  Serial.println("Serial monitor online!");
  if (!ina219.begin())
  {
    Serial.println("Failed to find INA219 chip");
    while (1)
    {
      delay(10);
    }
  }
  strip.setBrightness(DEFBRIGTHNESS);
  Serial.println("Measuring voltage and current with INA219 ...");
  strip.begin();
}

void loop()
{
  //ßSerial.print("Start loop");
  static unsigned long ulLastUpdate = 0;
  static unsigned long ulLastCircle = 0;
  static bool bPixelOff = false;

  if (millis() - ulLastUpdate > (unsigned long)DEFUPDATEINTERVAL || ulLastUpdate == 0)
  {
    checkParameters();
    ulLastUpdate = millis();
  }

  if ((int)current_mA >= (int)DEFTHRESHOLDLOAD)
  {
    if ((millis() - ulLastCircle) > (unsigned long)DEFUPDATETIMECIRVLE)
    {
      recolorNeopixel(false);
      iPosition++;
      iPosition %= (int)DEFNUMLEDS;
      iHue += 2;
      iHue %= (int)DEFMAXHUE;
      ulLastCircle = millis();
    }
    if (bPixelOff)
      bPixelOff = false;
  }

  if ((int)current_mA > (int)DEFTHRESHOLDLOADED && (int)current_mA < (int)DEFTHRESHOLDLOAD)
  {
    if ((millis() - ulLastCircle) > (unsigned long)DEFUPDATETIMECIRVLE)
    {
      recolorNeopixel(true);
      iPosition++;
      iPosition %= (int)DEFNUMLEDS;
      iHue += 2;
      iHue %= (int)DEFMAXHUE;
      ulLastCircle = millis();
    }
    if (bPixelOff)
      bPixelOff = false;
  }

  if (!bPixelOff && (int)current_mA <= (int)DEFTHRESHOLDLOADED)
  {
    strip.clear();
    strip.show();
    bPixelOff = true;
  }
}

void checkParameters()
{
  Serial.print("Start measuring");
  shuntvoltage = abs(ina219.getShuntVoltage_mV());
  busvoltage = abs(ina219.getBusVoltage_V());
  current_mA = abs(ina219.getCurrent_mA());
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);

  Serial.print("Bus Voltage:   ");
  Serial.print(busvoltage);
  Serial.println(" V");
  Serial.print("Shunt Voltage: ");
  Serial.print(shuntvoltage);
  Serial.println(" mV");
  Serial.print("Load Voltage:  ");
  Serial.print(loadvoltage);
  Serial.println(" V");
  Serial.print("Current:       ");
  Serial.print(current_mA);
  Serial.println(" mA");
  Serial.print("Power:         ");
  Serial.print(power_mW);
  Serial.println(" mW");
  Serial.println("");
}

void recolorNeopixel(bool bLoaded)
{
  if (!bLoaded)
  {
    for (int i = 0; i < (int)DEFNUMLEDS; i++)
      strip.setPixelColor((i + iPosition) % (int)DEFNUMLEDS, getPixelColorHsv(i, iHue, 255, strip.gamma8(i * (255 / (int)DEFNUMLEDS))));
  }
  else
  {
    for (int i = 0; i < (int)DEFNUMLEDS; i++)
      strip.setPixelColor((i + iPosition) % (int)DEFNUMLEDS, getPixelColorHsv(i, 512, 255, strip.gamma8(i * (255 / (int)DEFNUMLEDS))));
  }
  strip.show();
}

uint32_t getPixelColorHsv(
    uint16_t n, uint16_t h, uint8_t s, uint8_t v)
{

  uint8_t r, g, b;

  if (!s)
  {
    // Monochromatic, all components are V
    r = g = b = v;
  }
  else
  {
    uint8_t sextant = h >> 8;
    if (sextant > 5)
      sextant = 5; // Limit hue sextants to defined space

    g = v; // Top level

    // Perform actual calculations

    /*
       Bottom level:
       --> (v * (255 - s) + error_corr + 1) / 256
    */
    uint16_t ww; // Intermediate result
    ww = v * (uint8_t)(~s);
    ww += 1;       // Error correction
    ww += ww >> 8; // Error correction
    b = ww >> 8;

    uint8_t h_fraction = h & 0xff; // Position within sextant
    uint32_t d;                    // Intermediate result

    if (!(sextant & 1))
    {
      // r = ...slope_up...
      // --> r = (v * ((255 << 8) - s * (256 - h)) + error_corr1 + error_corr2) / 65536
      d = v * (uint32_t)(0xff00 - (uint16_t)(s * (256 - h_fraction)));
      d += d >> 8; // Error correction
      d += v;      // Error correction
      r = d >> 16;
    }
    else
    {
      // r = ...slope_down...
      // --> r = (v * ((255 << 8) - s * h) + error_corr1 + error_corr2) / 65536
      d = v * (uint32_t)(0xff00 - (uint16_t)(s * h_fraction));
      d += d >> 8; // Error correction
      d += v;      // Error correction
      r = d >> 16;
    }

    // Swap RGB values according to sextant. This is done in reverse order with
    // respect to the original because the swaps are done after the
    // assignments.
    if (!(sextant & 6))
    {
      if (!(sextant & 1))
      {
        uint8_t tmp = r;
        r = g;
        g = tmp;
      }
    }
    else
    {
      if (sextant & 1)
      {
        uint8_t tmp = r;
        r = g;
        g = tmp;
      }
    }
    if (sextant & 4)
    {
      uint8_t tmp = g;
      g = b;
      b = tmp;
    }
    if (sextant & 2)
    {
      uint8_t tmp = r;
      r = b;
      b = tmp;
    }
  }
  return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}