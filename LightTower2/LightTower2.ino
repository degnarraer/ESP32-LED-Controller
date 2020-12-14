    /*
    Light Tower by Rob Shockency
    Copyright (C) 2019 Rob Shockency degnarraer@yahoo.com

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
/**
 * @file LightTower2.ino
 * *

 */

#include <Arduino.h>
#include "VisualizationController.h"

VisualizationController visualizationController;
CalculateFPS calculateFPS("Main Loop", 1000);
TaskScheduler Scheduler;

void setup()
{
  if( true == debugRequired )
  {
    Serial.begin(115200);
  }
  else
  {
    pinMode(0, OUTPUT); 
    pinMode(1, OUTPUT); 
    digitalWrite(0, HIGH);
    digitalWrite(1, HIGH);  
  }
  delay(3000);
  randomSeed(analogRead(4)*1000);
  if(true == debugRequired) Serial.println("Main Program: Setup Started");
  if(true == debugRequired) Serial << "Main Program: TUNES: SAMPLE_RATE: " << SAMPLE_RATE << "\n";
  if(true == debugRequired) Serial << "Main Program: TUNES: FFT_MAX: " << FFT_MAX << "\n";
  if(true == debugRequired) Serial << "Main Program: TUNES: Task Count: " << Scheduler.GetTaskCount() << "\n";
  Scheduler.AddTask(calculateFPS);
  Scheduler.AddTask(visualizationController);
  if(true == debugMode && debugLevel >= 0) Serial.println("Main Program: Setup Complete");
}

void ADC_Handler()
{
  visualizationController.HandleADCInterrupt();
}

void loop()
{
  Scheduler.RunTasks();
}