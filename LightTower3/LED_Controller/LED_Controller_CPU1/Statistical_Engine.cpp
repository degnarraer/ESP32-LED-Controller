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


void StatisticalEngine::Setup()
{
  if(false == m_MemoryIsAllocated) AllocateMemory();
  SetupQueueManager();
  unsigned long currentTime = millis();
  m_NewBandDataCurrentTime = currentTime;
  m_NewMaxBandSoundDataCurrentTime = currentTime;
  m_NewSoundDataCurrentTime = currentTime;
}

bool StatisticalEngine::NewSoundDataReady()
{
  unsigned long currentTime = millis();
  size_t PSF_Size = uxQueueMessagesWaiting(GetQueueHandleRXForDataItem("Processed_Frame"));
  if(PSF_Size > 0)ESP_LOGV("Statistical_Engine", "New Sound Data Messages Waiting: %i", PSF_Size);
  if( 0 < PSF_Size )
  {
    ESP_LOGD("Statistical_Engine", "New Sound Data Ready");
    m_NewSoundDataCurrentTime = currentTime;
    m_NewSoundDataReady = true;
    m_NewSoundDataTimedOut = false;
    return true;
  }
  else if(currentTime - m_NewSoundDataCurrentTime >= m_NewSoundDataTimeOut && false == m_NewSoundDataTimedOut)
  {
    ESP_LOGW("Statistical_Engine", "New Sound Data Timeout");
    m_NewSoundDataReady = true;
    m_NewSoundDataTimedOut = true;
    return true;
  }
  else
  {
    m_NewSoundDataReady = false;
    return false;
  }
}

bool StatisticalEngine::NewBandDataReady()
{
  unsigned long currentTime = millis();
  size_t R_BANDS_Size = uxQueueMessagesWaiting(GetQueueHandleRXForDataItem("R_BANDS"));
  size_t L_BANDS_Size = uxQueueMessagesWaiting(GetQueueHandleRXForDataItem("L_BANDS"));
  ESP_LOGD("NewBandDataReady", "New Band Sound Data Messages Waiting: %i | %i", R_BANDS_Size, L_BANDS_Size);
  bool A = R_BANDS_Size > 0;
  bool B = L_BANDS_Size > 0;
  if( true == A && true == B )
  {
    ESP_LOGD("Statistical_Engine", "NewBandDataReady");
    m_NewBandDataCurrentTime = currentTime;
    m_NewBandDataReady = true;
    m_NewBandDataTimedOut = false;
    return true;
  }
  else if(currentTime - m_NewBandDataCurrentTime >= m_NewBandDataTimeOut && false == m_NewBandDataTimedOut)
  {
    ESP_LOGW("Statistical_Engine", "New Band Data Timeout");
    m_NewBandDataReady = true;
    m_NewBandDataTimedOut = true;
    return true;
  }
  else
  {
    m_NewBandDataReady = false;
    return false;
  }
}

bool StatisticalEngine::NewMaxBandSoundDataReady()
{
  unsigned long currentTime = millis();
  size_t R_MAXBAND_Size = uxQueueMessagesWaiting(GetQueueHandleRXForDataItem("R_MAXBAND"));
  size_t L_MAXBAND_Size = uxQueueMessagesWaiting(GetQueueHandleRXForDataItem("L_MAXBAND"));
  ESP_LOGV("NewMaxBandSoundDataReady", "New Max Band Sound Messages Waiting: %i | %i", R_MAXBAND_Size, L_MAXBAND_Size);
  bool A = (R_MAXBAND_Size > 0);
  bool B = (L_MAXBAND_Size > 0);
  if( true == A && true == B )
  {
    ESP_LOGD("Statistical_Engine", "New Max Band Sound Data Ready");
    m_NewMaxBandSoundDataCurrentTime = currentTime;
    m_NewMaxBandSoundDataReady = true;
    m_NewMaxBandSoundDataTimedOut = false;
    return true;
  }
  else if(currentTime - m_NewMaxBandSoundDataCurrentTime >= m_NewMaxBandSoundDataTimeOut && false == m_NewMaxBandSoundDataTimedOut)
  {
    ESP_LOGW("Statistical_Engine", "New Max Band Sound Data Timeout");
    m_NewMaxBandSoundDataReady = true;
    m_NewMaxBandSoundDataTimedOut = true;
    return true;
  }
  else
  {
    m_NewMaxBandSoundDataReady = false;
    return false;
  }
}

bool StatisticalEngine::CanRunMyScheduledTask()
{
  bool result = true == NewSoundDataReady() || true == NewBandDataReady() || NewMaxBandSoundDataReady();
  return result;
}

void StatisticalEngine::RunMyScheduledTask()
{
  if(true == m_NewSoundDataReady)
  {
    pthread_mutex_lock(&m_ProcessedSoundDataLock);
    if(false == m_NewSoundDataTimedOut)
    {
      ProcessedSoundFrame_t PSF;
      if(true == GetValueFromQueue(&PSF, GetQueueHandleRXForDataItem("Processed_Frame"), GetTotalByteCountForDataItem("Processed_Frame"), false, false))
      {
        m_Right_Channel_Processed_Sound_Data = PSF.Channel1;
        m_Left_Channel_Processed_Sound_Data = PSF.Channel2;
        
        //To allow the original code to work, we combine the left and right channels into an average
        m_Power = (m_Right_Channel_Processed_Sound_Data.NormalizedPower + m_Left_Channel_Processed_Sound_Data.NormalizedPower) / 2.0;
        m_signalMin = (m_Right_Channel_Processed_Sound_Data.Minimum + m_Left_Channel_Processed_Sound_Data.Minimum) / 2.0;
        m_signalMax = (m_Right_Channel_Processed_Sound_Data.Maximum + m_Left_Channel_Processed_Sound_Data.Maximum) / 2.0;
        ESP_LOGV("Statistical_Engine", "New SoundData Ready: %d | %f | %d", m_signalMin, m_Power, m_signalMax);
        UpdateSoundState();
      }
      else
      {
        m_Power = 0;
        m_signalMin = 0;
        m_signalMax = 0;
        UpdateSoundState();
      }
    }
    else
    {
      m_Power = 0;
      m_signalMin = 0;
      m_signalMax = 0;
      UpdateSoundState();
    }
    pthread_mutex_unlock(&m_ProcessedSoundDataLock);
  }

  if(true == m_NewBandDataReady)
  {
    pthread_mutex_lock(&m_BandValuesLock);
    if(true == m_NewBandDataTimedOut)
    {
      memset(m_Right_Band_Values, 0.0, sizeof(m_Right_Band_Values));
      memset(m_Left_Band_Values, 0.0, sizeof(m_Left_Band_Values));
    }
    else
    {
      GetValueFromQueue(m_Right_Band_Values, GetQueueHandleRXForDataItem("R_BANDS"), GetTotalByteCountForDataItem("R_BANDS"), false, false);
      GetValueFromQueue(m_Left_Band_Values, GetQueueHandleRXForDataItem("L_BANDS"), GetTotalByteCountForDataItem("L_BANDS"), false, false);
      UpdateBandArray();
    }       
    pthread_mutex_unlock(&m_BandValuesLock);
  }
  
  if(true == m_NewMaxBandSoundDataReady)
  {
    pthread_mutex_lock(&m_MaxBinSoundDataLock);
    if(true == m_NewMaxBandSoundDataTimedOut)
    {
      m_Right_MaxBandSoundData.MaxBandNormalizedPower = 0.0;
      m_Right_MaxBandSoundData.MaxBandIndex = 0;
      m_Left_MaxBandSoundData.MaxBandNormalizedPower = 0.0;
      m_Left_MaxBandSoundData.MaxBandIndex = 0;
    }
    else
    {
      GetValueFromQueue(&m_Right_MaxBandSoundData, GetQueueHandleRXForDataItem("R_MAXBAND"), GetTotalByteCountForDataItem("R_MAXBAND"), false, false);
      GetValueFromQueue(&m_Left_MaxBandSoundData, GetQueueHandleRXForDataItem("L_MAXBAND"), GetTotalByteCountForDataItem("L_MAXBAND"), false, false);
    }
    pthread_mutex_unlock(&m_MaxBinSoundDataLock);
  }
}

void StatisticalEngine::AllocateMemory()
{
  ESP_LOGI("Statistical_Engine", "%s: Allocating Memory.", GetTitle().c_str());
  m_MemoryIsAllocated = true;
}

void StatisticalEngine::FreeMemory()
{
  ESP_LOGI("Statistical_Engine", "%s: Freeing Memory.", GetTitle().c_str());
  m_MemoryIsAllocated = false;
}

void StatisticalEngine::UpdateSoundState()
{
  pthread_mutex_lock(&m_ProcessedSoundDataLock);
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
    ESP_LOGD("Statistical_Engine", "Sound Detected.");
    soundState = SoundState::SoundDetected;
    m_cb->MicrophoneStateChange(soundState);
  }
  else if(soundState == SoundState::SoundDetected && m_silenceIntegrator <= m_silenceDetectedThreshold)
  {
    ESP_LOGD("Statistical_Engine", "Silence Detected.");
    soundState = SoundState::SilenceDetected;
    m_silenceStartTime = millis();
    m_cb->MicrophoneStateChange(soundState);
  }
  else if(soundState == SoundState::SilenceDetected && millis() - m_silenceStartTime >= 120000)
  {
    ESP_LOGD("Statistical_Engine", "Lasting Silence Detected.");
    soundState = SoundState::LastingSilenceDetected;
    m_cb->MicrophoneStateChange(soundState);
  }
  pthread_mutex_unlock(&m_ProcessedSoundDataLock);
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
  if(true == debugSetBandValueStatisticalEngine) 
  {
    Serial << "SET BAND VALUES: ";
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

float StatisticalEngine::GetBandValue(unsigned int band, unsigned int depth)
{
  float result = 0.0;
  pthread_mutex_lock(&m_BandValuesLock);
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
    if(true == debugGetBandValueStatisticalEngine) Serial << "Band: " << band << " " << "Depth: " << depth << " " << "Result: " << result << "\n";
  }
  else
  {
    if(true == debugMode) Serial << "!!ERROR: Bin Array Out of Bounds\n";
    result = 0.0;
  }
  pthread_mutex_unlock(&m_BandValuesLock);
  return result;
}

float StatisticalEngine::GetBandAverage(unsigned int band, unsigned int depth)
{
  float total = 0.0;
  float result = 0.0;
  pthread_mutex_lock(&m_BandValuesLock);
  unsigned int count = 0;
  for(int i = 0; i < BAND_SAVE_LENGTH && i <= depth; ++i)
  {
    total += GetBandValue(band, i);
    ++count;
  }
  result = total / count;
  if(true == debugMode && debugLevel >= 5) Serial << "GetBandAverage Band: " << band << "\tDepth: " << depth << "\tResult: " << result <<"\n";
  pthread_mutex_unlock(&m_BandValuesLock);
  return result;
}
float StatisticalEngine::GetBandAverageForABandOutOfNBands(unsigned band, unsigned int depth, unsigned int TotalBands)
{
  assert(band < TotalBands);
  assert(TotalBands <= m_NumBands);
  assert(TotalBands > 0);
  float result = 0.0;
  pthread_mutex_lock(&m_BandValuesLock);
  int bandSeparation = m_NumBands / TotalBands;
  int startBand = band * bandSeparation;
  int endBand = startBand + bandSeparation;
  for(int b = startBand; b < endBand; ++b)
  {
    result += GetBandAverage(b, depth);
  }
  if(true == debugVisualization) Serial << "Separation:" << bandSeparation << "\tStart:" << startBand << "\tEnd:" << endBand << "\tResult:" << result << "\n";
  pthread_mutex_unlock(&m_BandValuesLock);
  return result;
}

float StatisticalEngine::GetNormalizedSoundPower()
{ 
  float Result = 0;
  pthread_mutex_lock(&m_ProcessedSoundDataLock);
  Result = m_Power;
  pthread_mutex_unlock(&m_ProcessedSoundDataLock);
  return Result;
}
