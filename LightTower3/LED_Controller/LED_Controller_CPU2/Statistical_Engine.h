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

#ifndef StatisticalEngine_H
#define StatisticalEngine_H

#define STATISTICAL_ENGINE_MEMORY_DEBUG false
#define STATISTICAL_ENGINE_DATA_DEBUG false

#include <limits.h>
#include "TaskInterface.h"
#include "Streaming.h"
#include "Tunes.h"
#include "Helpers.h"

enum SoundState
{
  LastingSilenceDetected,
  SilenceDetected,
  SoundDetected,
};

enum BandDataType
{
  INSTANT,
  AVERAGE
};

enum DataChannelType
{
  MICROPHONE_STANDARD,
  MICROPHONE_AUTOGAIN,
  MICROPHONE_GAIN_VALUE,
  FFT_GAIN_VALUE
};

struct MinMax
{
  float min = 0;
  float max = 0;
};

typedef float db;
struct MinMaxDb
{
  db min = 0.0;
  db max = 0.0;
};

class MicrophoneMeasureCalleeInterface
{
public:
    virtual void MicrophoneStateChange(SoundState) = 0;
};

class MicrophoneMeasureCallerInterface
{
public:
    void ConnectMicrophoneMeasureCallerInterfaceCallback(MicrophoneMeasureCalleeInterface *cb)
    {
        m_cb = cb;
    }
    MicrophoneMeasureCalleeInterface *m_cb;
};

class StatisticalEngine : public NamedItem
                        , public Task
                        , public MicrophoneMeasureCallerInterface
                        , public CommonUtils
                        , public QueueManager
{
  public:
    StatisticalEngine()
      : NamedItem("StatisticalEngine")
      , Task(GetTitle())
      , QueueManager(GetTitle() + "_QueueManager", m_ConfigCount)
      , m_Power(0)
      , m_PowerDb(0){}
    virtual ~StatisticalEngine()
    {
      FreeMemory();
    }
    SoundState GetSoundState();

    void SetProcessFFTStatus(bool value) {m_ProcessFFT = value; }
    bool GetProcessFFTStatus() {return m_ProcessFFT; }
    void AllocateMemory();
    void FreeMemory();
  
    //Main Data Interface
    int GetFFTBinIndexForFrequency(float freq);
    float GetFreqForBin(unsigned int bin);
    int GetFFTData(int position);  
  
    //QueueManager
    DataItemConfig_t* GetDataItemConfig() { return m_ItemConfig; }
    size_t GetDataItemConfigCount() { return m_ConfigCount; }

    //Power Getters
    float GetNormalizedSoundPower();
    
    //Band Data Getters
    unsigned int GetNumberOfBands() { return m_NumBands; }
    int GetBandValue(unsigned int band, unsigned int depth);
    float GetBandAverage(unsigned band, unsigned int depth);
    int GetBandAverageForABandOutOfNBands(unsigned band, unsigned int depth, unsigned int TotalBands);
  
  private:

    
    static const size_t m_ConfigCount = 4;
    DataItemConfig_t m_ItemConfig[m_ConfigCount]
    {
      { "R_FFT",     DataType_Int16_t,                32,   Transciever_RX },
      { "L_FFT",     DataType_Int16_t,                32,   Transciever_RX },
      { "R_PSD",     DataType_ProcessedSoundData_t,   1,    Transciever_RX },
      { "L_PSD",     DataType_ProcessedSoundData_t,   1,    Transciever_RX }
    };

    bool m_ProcessFFT = true;
    //BAND Circular Buffer
    bool m_NewBandDataReady = false;
    static const unsigned int m_NumBands = 32; //Need way to set this
    int BandValues[m_NumBands][BAND_SAVE_LENGTH];
    int currentBandIndex = -1;
    int BandRunningAverageValues[m_NumBands][BAND_SAVE_LENGTH];
    int currentAverageBandIndex = -1;
    bool NewBandDataReady();
    void UpdateBandArray();
    void UpdateRunningAverageBandArray();

    //Task Interface
    void Setup();
    void RunMyPreTask(){}
    bool CanRunMyScheduledTask();
    void RunMyScheduledTask();
    void RunMyPostTask(){}
    
    //Sound State Output
    bool m_NewSoundDataReady = false;
    float m_Power;
    float m_PowerDb;
    int m_signalMin;
    int m_signalMax;
    float m_AmpGain = 1.0;
    float m_FFTGain = 1.0;
    bool NewSoundDataReady();
    void UpdateSoundState();

 //Sound Detection
  private:
    bool m_MemoryIsAllocated = false;
    size_t m_BandInputByteCount = sizeof(int16_t) * m_NumBands;
    
    //Right Channel Input Sound Data
    int16_t* m_Right_Band_Values;
    ProcessedSoundData_t m_Right_Channel_Processed_Sound_Data;

    //Left Channel Input Sound Data
    int16_t* m_Left_Band_Values;
    ProcessedSoundData_t m_Left_Channel_Processed_Sound_Data;

    //Sound Detection
    const int     m_silenceDetectedThreshold = silenceDetectedThreshold;
    const int     m_soundDetectedThreshold = soundDetectedThreshold;
    const int     m_silenceIntegratorMax = silenceIntegratorMax;
    int           m_silenceIntegrator = 0;
    const int     m_silenceIntegratorMin = 0;  
    long          m_silenceStartTime;
    unsigned long m_startMicros;
    unsigned long m_previousMicros;
    unsigned long m_currentMicros;
    unsigned long m_finalMicros;
    const int     m_soundAdder = soundAdder;
    const int     m_silenceSubtractor = silenceSubtractor;
    SoundState    soundState = SoundState::SilenceDetected;
};

#endif
