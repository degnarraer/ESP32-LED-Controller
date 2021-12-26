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
 
#include <arm_common_tables.h>
#include <Arduino.h>
#include "Statistical_Engine.h"


CalculateFPS calculateFPS2("Statistical Engine", 1000);
void StatisticalEngine::Setup()
{
  if(false == m_MemoryIsAllocated) AllocateMemory();
  calculateFPS2.Setup();
}

bool StatisticalEngine::NewDataReady()
{
  bool NewData = false;
  if(true == GetValueFromQueue(m_Right_Band_Values, m_FFT_Right_BandData_Input_Buffer_Queue, GetFFTRightBandDataBufferSize(), false)){ NewData = true; }
  if(true == GetValueFromQueue(&m_Right_Channel_Pow_Normalized, m_Right_Channel_Normalized_Power_Input_Buffer_Queue, GetRightChannelNormalizedPowerSize(), false))NewData = true;
  if(true == GetValueFromQueue(&m_Right_Channel_Db, m_Right_Channel_DB_Input_Buffer_Queue, GetRightChannelDBSize(), false))NewData = true;
  if(true == GetValueFromQueue(&m_Right_Channel_Min, m_Right_Channel_Pow_Min_Input_Buffer_Queue, GetRightChannelPowerMinSize(), false))NewData = true;
  if(true == GetValueFromQueue(&m_Right_Channel_Max, m_Right_Channel_Pow_Max_Input_Buffer_Queue, GetRightChannelPowerMaxSize(), false))NewData = true;
  
  if(true == GetValueFromQueue(m_Left_Band_Values, m_FFT_Left_BandData_Input_Buffer_Queue, GetFFTLeftBandDataBufferSize(), false))NewData = true;
  if(true == GetValueFromQueue(&m_Left_Channel_Pow_Normalized, m_Left_Channel_Normalized_Power_Input_Buffer_Queue, GetLeftChannelNormalizedPowerSize(), false))NewData = true;
  if(true == GetValueFromQueue(&m_Left_Channel_Db, m_Left_Channel_DB_Input_Buffer_Queue, GetLeftChannelDBSize(), false))NewData = true;
  if(true == GetValueFromQueue(&m_Left_Channel_Min, m_Left_Channel_Pow_Min_Input_Buffer_Queue, GetLeftChannelPowerMinSize(), false))NewData = true;
  if(true == GetValueFromQueue(&m_Left_Channel_Max, m_Left_Channel_Pow_Max_Input_Buffer_Queue, GetLeftChannelPowerMaxSize(), false))NewData = true;
  return NewData;
}

bool StatisticalEngine::CanRunMyScheduledTask()
{
  if(true == NewDataReady())
  {
    if(true == calculateFPS2.CanRunMyScheduledTask()) { calculateFPS2.RunMyScheduledTask(); }
    return true;
  }
  else
  {
    return false;
  }
}
void StatisticalEngine::RunMyScheduledTask()
{
  //To allow the original code to work, we combine the left and right channels into an average
  m_Power = (m_Right_Channel_Pow_Normalized + m_Left_Channel_Pow_Normalized) / 2;
  m_PowerDb = (m_Right_Channel_Db + m_Left_Channel_Db ) / 2;

  //m_signalMin;
  //m_signalMax;
  //UpdateSoundState();
  //UpdateBandArray();
}

void StatisticalEngine::AllocateMemory()
{
  Serial << GetTitle() << ": Allocating Memory.\n";
  m_Right_Band_Values = (int16_t*)malloc(m_BandInputByteCount);
  m_Left_Band_Values = (int16_t*)malloc(m_BandInputByteCount);
  
  CreateQueue(m_FFT_Right_BandData_Input_Buffer_Queue, m_BandInputByteCount, 10, true);
  CreateQueue(m_FFT_Left_BandData_Input_Buffer_Queue, m_BandInputByteCount, 10, true);
  
  CreateQueue(m_Right_Channel_Normalized_Power_Input_Buffer_Queue, sizeof(m_Right_Channel_Pow_Normalized), 10, true);
  CreateQueue(m_Right_Channel_DB_Input_Buffer_Queue, sizeof(m_Right_Channel_Db), 10, true);
  CreateQueue(m_Right_Channel_Pow_Min_Input_Buffer_Queue, sizeof(m_Right_Channel_Min), 10, true);
  CreateQueue(m_Right_Channel_Pow_Max_Input_Buffer_Queue, sizeof(m_Right_Channel_Max), 10, true);
  
  CreateQueue(m_Left_Channel_Normalized_Power_Input_Buffer_Queue, sizeof(m_Left_Channel_Pow_Normalized), 10, true);
  CreateQueue(m_Left_Channel_DB_Input_Buffer_Queue, sizeof(m_Left_Channel_Db), 10, true);
  CreateQueue(m_Left_Channel_Pow_Min_Input_Buffer_Queue, sizeof(m_Left_Channel_Min), 10, true);
  CreateQueue(m_Left_Channel_Pow_Max_Input_Buffer_Queue, sizeof(m_Left_Channel_Max), 10, true);
  m_MemoryIsAllocated = true;
}

void StatisticalEngine::FreeMemory()
{
  Serial << GetTitle() << ": Freeing Memory.\n";
  
  delete m_Right_Band_Values;
  delete m_Left_Band_Values;
  
  vQueueDelete(m_FFT_Right_BandData_Input_Buffer_Queue);
  vQueueDelete(m_FFT_Left_BandData_Input_Buffer_Queue);
  
  vQueueDelete(m_Right_Channel_Normalized_Power_Input_Buffer_Queue);
  vQueueDelete(m_Right_Channel_DB_Input_Buffer_Queue);
  vQueueDelete(m_Right_Channel_Pow_Min_Input_Buffer_Queue);
  vQueueDelete(m_Right_Channel_Pow_Max_Input_Buffer_Queue);
  
  vQueueDelete(m_Left_Channel_Normalized_Power_Input_Buffer_Queue);
  vQueueDelete(m_Left_Channel_DB_Input_Buffer_Queue);
  vQueueDelete(m_Left_Channel_Pow_Min_Input_Buffer_Queue);
  vQueueDelete(m_Left_Channel_Pow_Max_Input_Buffer_Queue);
  m_MemoryIsAllocated = false;
}


void StatisticalEngine::UpdateSoundState()
{
  int delta = 0;
  float gain = 0.0;
  if(m_Power >= SOUND_DETECT_THRESHOLD)
  {
    float  numerator = m_Power - SOUND_DETECT_THRESHOLD;
    float  denomanator = SOUND_DETECT_THRESHOLD;
    if(numerator < 0) numerator = 0;
    gain = (numerator/denomanator);
    delta = m_soundAdder * gain;
  }
  else
  {
    float  numerator = SOUND_DETECT_THRESHOLD - m_Power;
    float  denomanator = SOUND_DETECT_THRESHOLD;
    if(numerator < 0) numerator = 0;
    gain = (numerator/denomanator);
    delta = m_silenceSubtractor * gain;
  }
  m_silenceIntegrator += delta;
  if(m_silenceIntegrator < m_silenceIntegratorMin) m_silenceIntegrator = m_silenceIntegratorMin;
  if(m_silenceIntegrator > m_silenceIntegratorMax) m_silenceIntegrator = m_silenceIntegratorMax;
  if(true == debugMode && debugLevel >= 3) Serial << "Power Db: " << m_PowerDb << "\tGain: " << gain << "\tDelta: " << delta << "\tSilence Integrator: " << m_silenceIntegrator << "\tSound State: " << soundState << "\n";
  if((soundState == SoundState::SilenceDetected || soundState == SoundState::LastingSilenceDetected) && m_silenceIntegrator >= m_soundDetectedThreshold)
  {
    if(true == debugMode && debugLevel >= 1) Serial << "Sound Detected\n";
    soundState = SoundState::SoundDetected;
    m_cb->MicrophoneStateChange(soundState);
  }
  else if(soundState == SoundState::SoundDetected && m_silenceIntegrator <= m_silenceDetectedThreshold)
  {
    if(true == debugMode && debugLevel >= 1) Serial << "Silence Detected\n";
    soundState = SoundState::SilenceDetected;
    m_silenceStartTime = millis();
    m_cb->MicrophoneStateChange(soundState);
  }
  else if(soundState == SoundState::SilenceDetected && millis() - m_silenceStartTime >= 120000)
  {
    if(true == debugMode && debugLevel >= 1) Serial << "Lasting Silence Detected\n";
    soundState = SoundState::LastingSilenceDetected;
    m_cb->MicrophoneStateChange(soundState);
  }
}

void StatisticalEngine::UpdateBandArray()
{
  ++currentBandIndex;
  if(currentBandIndex >= BAND_SAVE_LENGTH)
  {
    currentBandIndex = 0;
    if(true == debugMode && debugLevel >= 2) Serial << "Band Array Rollover\n";
  }
  for(int i = 0; i < m_NumBands; ++i)
  {
    BandValues[i][currentBandIndex] = 0;
  }
  for(int i = 0; i < m_NumBands; ++i)
  {
    BandValues[i][currentBandIndex] = (m_Right_Band_Values[i] + m_Left_Band_Values[i]) / 2;
  }
  if(currentBandIndex >= BAND_SAVE_LENGTH - 1)
  {
    UpdateRunningAverageBandArray();
  }
  //if(true == debugMode && debugLevel >= 2) 
  {
    Serial << "BAND VALUES: ";
    for(int i = 0; i < m_NumBands; i++)
    {
      Serial << BandValues[i][currentBandIndex] << "\t";
    }
    Serial << "\n";
  }
}

void StatisticalEngine::UpdateRunningAverageBandArray()
{
  ++currentAverageBandIndex;
  if(currentAverageBandIndex >= BAND_SAVE_LENGTH)
  {
    if(true == debugMode && debugLevel >= 2) Serial << "Band Running Average Array Rollover\n";
    currentAverageBandIndex = 0;
  }
  for(int i = 0; i < m_NumBands; ++i)
  {
    BandRunningAverageValues[i][currentAverageBandIndex] = GetBandAverage(i, BAND_SAVE_LENGTH);
  }
  if(true == debugMode && debugLevel >= 2) 
  {
    Serial << "BAND AVG VALUES: ";
    for(int i = 0; i < m_NumBands; i++)
    {
      Serial << BandRunningAverageValues[0][currentAverageBandIndex] << "\t";
    }
    Serial << "\n";
  }
}

SoundState StatisticalEngine::GetSoundState()
{
  return soundState;
}

float StatisticalEngine::GetFreqForBin(unsigned int bin)
{
  if(bin > BINS) bin = BINS;
  if(bin < 0) bin = 0;
  return FFT_BIN(bin, SAMPLE_RATE, FFT_MAX);
}

int StatisticalEngine::GetBandValue(unsigned int band, unsigned int depth)
{
  int result;
  if(band < m_NumBands && depth < BAND_SAVE_LENGTH)
  {
    int position = 0;
    if (depth <= currentBandIndex)
    {
      position = currentBandIndex - depth;
    }
    else
    {
      position = BAND_SAVE_LENGTH - (depth - currentBandIndex);
    }
    result = BandValues[band][position];
    if(true == debugMode && debugLevel >= 5) Serial << "Band: " << band << " " << "Depth: " << depth << " " << "Result: " << result << "\n";
    return result;
  }
  else
  {
    if(true == debugMode) Serial << "!!ERROR: Bin Array Out of Bounds\n";
    return 0;
  }
}

float StatisticalEngine::GetBandAverage(unsigned int band, unsigned int depth)
{
  int total = 0;
  unsigned int count = 0;
  for(int i = 0; i < BAND_SAVE_LENGTH && i <= depth; ++i)
  {
    total += GetBandValue(band, i);
    ++count;
  }
  float result = total / count;
  if(true == debugMode && debugLevel >= 5) Serial << "GetBandAverage Band: " << band << "\tDepth: " << depth << "\tResult: " << result <<"\n";
  return result;
}
int StatisticalEngine::GetBandAverageForABandOutOfNBands(unsigned band, unsigned int depth, unsigned int TotalBands)
{
  assert(band < TotalBands);
  assert(TotalBands <= m_NumBands);
  assert(TotalBands > 0);
  int bandSeparation = m_NumBands / TotalBands;
  int startBand = band * bandSeparation;
  int endBand = startBand + bandSeparation;
  float result = 0.0;
  for(int b = startBand; b < endBand; ++b)
  {
    result += GetBandAverage(b, depth);
  }
  if(true == debugVisualization) Serial << "Separation:" << bandSeparation << "\tStart:" << startBand << "\tEnd:" << endBand << "\tResult:" << result << "\n";
  return (int)round(result);
}

float StatisticalEngine::GetNormalizedSoundPower()
{ 
  if(true == debugSoundPower) Serial << "StatisticalEngine: Get Sound Power: " << m_Power << "\n";
  return m_Power;
}