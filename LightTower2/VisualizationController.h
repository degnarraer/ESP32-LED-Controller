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
/**
 * @file LightTower2.ino
 * *

 */
 
#ifndef VisualizationController_H
#define VisualizationController_H

#include "Statistical_Engine.h"
#include "Streaming.h"
#include "Visualizations.h"
#include "LEDControllerInterface.h"
#include "TaskInterface.h"

class VisualizationController: public Task
                             , MicrophoneMeasureCalleeInterface
                             , ADCInterruptHandler
{
  public:
    VisualizationController(): Task("VisualizationController"){}
    void HandleADCInterrupt() { m_StatisticalEngine.HandleADCInterrupt(); }
  protected:
    //MicrophoneMeasureCalleeInterface
    void MicrophoneStateChange(SoundState){}
    
    //Task Interface
    void Setup();
    bool CanRunTask(){ return true; }
    void RunTask();
  private:
    StatisticalEngine m_StatisticalEngine;
    TaskScheduler m_Scheduler;
};

#endif
