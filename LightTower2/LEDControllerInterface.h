/*
    Light Tower by Rob Shockency
    Copyright (C) 2020 Rob Shockency degnarraer@yahoo.com

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version of the License, or
    (at your option) any later version. 3

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LEDControllerInterface_H
#define LEDControllerInterface_H

#define FASTLED_ALLOW_INTERRUPTS 1
#define FASTLED_INTERRUPT_RETRY_COUNT 10
#include <FastLED.h>
#include "Tunes.h"
#include "Streaming.h"

typedef CRGBArray<NUMLEDS> LEDStrip;
typedef struct { CRGB Pixel[SCREEN_WIDTH][SCREEN_HEIGHT]; } Pixels;

class LEDController
{
  public:
    LEDController()
    {
      FastLED.addLeds<WS2812, DATA_PIN_STRIP1, GRB>(m_LEDStrip[0], NUMLEDS);
      FastLED.addLeds<WS2812, DATA_PIN_STRIP2, GRB>(m_LEDStrip[1], NUMLEDS);
      FastLED.addLeds<WS2812, DATA_PIN_STRIP3, GRB>(m_LEDStrip[2], NUMLEDS);
      FastLED.addLeds<WS2812, DATA_PIN_STRIP4, GRB>(m_LEDStrip[3], NUMLEDS);
      FastLED.setBrightness(255);
    }
    void Setup()
    {
      
    }
    void UpdateLEDs( Pixels &Pixels )
    {
      if(true == debugLEDs) Serial << "******LED Controller LEDs******\n";
      for(int h = 0; h < SCREEN_HEIGHT; ++ h)
      {
        for(int w = 0; w < SCREEN_WIDTH; ++w)
        {
          CRGB bufColor = Pixels.Pixel[w][h];
          m_LEDStrip[w][h] = Pixels.Pixel[w][h];
          if(true == debugLEDs) Serial << bufColor[0] << ":" << bufColor[1] << ":" << bufColor[2] << " \t";
        }
        if(true == debugLEDs) Serial << "\n";
      }
      FastLED.show();
    }
    void TurnOffLEDs()
    {
      FastLED.setBrightness(0);
      if(true == debugMode && debugLevel >= 2) Serial << "Brightness set to 0.\n";
    }
    void TurnOnLEDs(unsigned int level)
    {
      FastLED.setBrightness(255*(double)level/(double)100);
      if(true == debugMode && debugLevel >= 2) Serial << "Brightness set to " << level << ".\n";
    }
  private:
    LEDStrip m_LEDStrip[NUMSTRIPS];
};

class LEDControllerInterface: public LEDController
{
  public:
    LEDControllerInterface(){}
};

#endif
