/*
    Light Tower by Rob Shockency
    Copyright (C) 2021 Rob Shockency degnarraer@yahoo.com

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

#pragma once

#include "arduinoFFT.h"
#include "FFT_Calculator.h"
#include "Amplitude_Calculator.h"
#include <DataTypes.h>
#include <Helpers.h>
#include "Tunes.h"
#include "Streaming.h"
#include "float.h"
#include "AudioBuffer.h"
#include "DataItem/DataItems.h"

class Sound_Processor: public NamedItem
                     , public CommonUtils
                     , public SetupCallerInterface
{
  public:
    Sound_Processor( String Title
                   , ContinuousAudioBuffer<AUDIO_BUFFER_SIZE> &AudioBuffer
                   , SerialPortMessageManager &CPU1SerialPortMessageManager
                   , SerialPortMessageManager &CPU3SerialPortMessageManager
                   , IPreferences& preferences );
    virtual ~Sound_Processor();
    void Setup();
    
  private:
    ContinuousAudioBuffer<AUDIO_BUFFER_SIZE> &m_AudioBuffer;
    Amplitude_Calculator m_RightSoundData = Amplitude_Calculator(AMPLITUDE_BUFFER_FRAME_COUNT, BitLength_16);
    Amplitude_Calculator m_LeftSoundData = Amplitude_Calculator(AMPLITUDE_BUFFER_FRAME_COUNT, BitLength_16);
    FFT_Calculator m_R_FFT = FFT_Calculator(FFT_SIZE, I2S_SAMPLE_RATE, BitLength_16);
    FFT_Calculator m_L_FFT = FFT_Calculator(FFT_SIZE, I2S_SAMPLE_RATE, BitLength_16);

    
    SerialPortMessageManager &m_CPU1SerialPortMessageManager;
    SerialPortMessageManager &m_CPU3SerialPortMessageManager;
    IPreferences& m_Preferences;

    const float m_Amplitude_Gain_InitialValue = 1.1;
    DataItemWithPreferences<float, 1> m_Amplitude_Gain = DataItemWithPreferences<float, 1>( "Amp_Gain"
                                                                                          , m_Amplitude_Gain_InitialValue
                                                                                          , RxTxType_Rx_Echo_Value
                                                                                          , 5000
                                                                                          , &m_Preferences
                                                                                          , &m_CPU3SerialPortMessageManager
                                                                                          , NULL
                                                                                          , this );

    const float m_FFT_Gain_InitialValue = 1.1;
    DataItemWithPreferences<float, 1> m_FFT_Gain = DataItemWithPreferences<float, 1>( "FFT_Gain"
                                                                                    , m_FFT_Gain_InitialValue
                                                                                    , RxTxType_Rx_Echo_Value
                                                                                    , 5000
                                                                                    , &m_Preferences
                                                                                    , &m_CPU3SerialPortMessageManager
                                                                                    , NULL
                                                                                    , this );
    
    MaxBandSoundData_t m_R_Max_Band_InitialValue = MaxBandSoundData_t();
    DataItem<MaxBandSoundData_t, 1> m_R_Max_Band = DataItem<MaxBandSoundData_t, 1>( "R_Max_Band"
                                                                                  , m_R_Max_Band_InitialValue
                                                                                  , RxTxType_Tx_On_Change
                                                                                  , 0
                                                                                  , &m_CPU1SerialPortMessageManager
                                                                                  , NULL
                                                                                  , this );
    float m_R_Bands_InitialValue = 0.0;
    DataItem<float, 32> m_R_Bands = DataItem<float, 32>( "R_Bands"
                                                       , m_R_Bands_InitialValue
                                                       , RxTxType_Tx_On_Change
                                                       , 0
                                                       , &m_CPU1SerialPortMessageManager
                                                       , NULL
                                                       , this );
    
    MaxBandSoundData_t m_L_Max_Band_InitialValue = MaxBandSoundData_t();
    DataItem<MaxBandSoundData_t, 1> m_L_Max_Band = DataItem<MaxBandSoundData_t, 1>( "L_Max_Band"
                                                                                  , m_L_Max_Band_InitialValue
                                                                                  , RxTxType_Tx_On_Change
                                                                                  , 0
                                                                                  , &m_CPU1SerialPortMessageManager
                                                                                  , NULL
                                                                                  , this );
    
    float m_L_Bands_InitialValue = 0.0;
    DataItem<float, 32> m_L_Bands = DataItem<float, 32>( "L_Bands"
                                                       , m_L_Bands_InitialValue
                                                       , RxTxType_Tx_On_Change
                                                       , 0
                                                       , &m_CPU1SerialPortMessageManager
                                                       , NULL
                                                       , this );
    
    //DB Conversion taken from INMP441 Datasheet
    float m_IMNP441_1PA_Offset = 94;          //DB Output at 1PA
    float m_IMNP441_1PA_Value = 420426.0;     //Digital output at 1PA
    
    uint32_t m_24BitLength = 1 << 24;     //Used for Amplitude of 24 bit MIC values
    uint32_t m_16BitLength = 1 << 16;     //Used for Amplitude of 16 bit FFT values
    uint64_t m_32BitLength = 1ULL << 32;  //Used for Amplitude of 32 bit FFT values
    
    TaskHandle_t m_ProcessSoundPowerTask;
    static void Static_Calculate_Power(void * parameter);
    void Calculate_Power();
    TaskHandle_t m_ProcessFFTTask;
    static void Static_Calculate_FFTs(void * parameter);
    void Calculate_FFTs();
    void Update_Right_Bands_And_Send_Result();
    void Update_Left_Bands_And_Send_Result();

    void AssignToBands(float* Band_Data, FFT_Calculator* FFT_Calculator, int16_t FFT_Size);
    float GetFreqForBin(int bin);
    int GetBinForFrequency(float Frequency);
    int16_t m_AudioBinLimit;
};
