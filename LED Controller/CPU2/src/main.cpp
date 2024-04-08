/*
    Light Tower by Rob Shockency
    Copyright (C) 2021 Rob Shockency degnarraer@yahoo.com

    This program is free software: you can redistribute it and/or modifyVision8_LTT
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

#include "Manager.h"
#include "Tunes.h"
#include "esp_log.h"
#include "DataItem.h"

Preferences m_Preferences;

TaskHandle_t ProcessSPI_CPU1_TXTask;
uint32_t ProcessSPI_CPU1_TXTaskLoopCount = 0;

TaskHandle_t ProcessSPI_CPU3_TXTask;
uint32_t ProcessSPI_CPU3_TXTaskLoopCount = 0;

I2S_Device m_I2S_In = I2S_Device( "I2S_In"
                                , I2S_NUM_1
                                , i2s_mode_t(I2S_MODE_SLAVE | I2S_MODE_RX)
                                , I2S_SAMPLE_RATE
                                , I2S_BITS_PER_SAMPLE_16BIT
                                , I2S_CHANNEL_FMT_RIGHT_LEFT
                                , i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB)
                                , I2S_CHANNEL_STEREO
                                , true                               // Use APLL
                                , I2S_BUFFER_COUNT                   // Buffer Count
                                , I2S_SAMPLE_COUNT                   // Buffer Size
                                , I2S1_SCLCK_PIN                     // Serial Clock Pin
                                , I2S1_WD_PIN                        // Word Selection Pin
                                , I2S1_SDIN_PIN                      // Serial Data In Pin
                                , I2S1_SDOUT_PIN );                  // Serial Data Out Pin 

BluetoothA2DPSource a2dp_source;
Bluetooth_Source m_BT_Out( "Bluetooth Source", a2dp_source );
                                            
ContinuousAudioBuffer<AUDIO_BUFFER_SIZE> m_AudioBuffer;                                            

DataSerializer m_DataSerializer;
SerialPortMessageManager m_CPU1SerialPortMessageManager = SerialPortMessageManager("CPU1", Serial1, m_DataSerializer);
SerialPortMessageManager m_CPU3SerialPortMessageManager = SerialPortMessageManager("CPU3", Serial2, m_DataSerializer);


Sound_Processor m_SoundProcessor = Sound_Processor( "Sound Processor"
                                                  , m_AudioBuffer
                                                  , m_CPU1SerialPortMessageManager
                                                  , m_CPU3SerialPortMessageManager );                                            

Manager m_Manager = Manager( "Manager"
                           , m_SoundProcessor
                           , m_CPU1SerialPortMessageManager
                           , m_CPU3SerialPortMessageManager
                           , m_BT_Out
                           , m_I2S_In
                           , m_AudioBuffer
                           , m_Preferences);


int32_t SetBTTxData(uint8_t *Data, int32_t channel_len)
{
  return m_Manager.SetBTTxData(Data, channel_len);
}

static bool ConnectToThisName(const char* aName, esp_bd_addr_t address, int32_t rssi)
{
  return m_BT_Out.ConnectToThisName(aName, address, rssi);
}

void InitializePreferences()
{
  if(!m_Preferences.begin("Settings", false))
  {
    nvs_flash_erase();
    ESP_LOGI("InitializePreferences", "NVS Cleared!");
    nvs_flash_init();
    ESP_LOGI("InitializePreferences", "NVS Initialized");
    ESP.restart();
  }
  else if(m_Preferences.getBool("Pref_Reset", true))
  {
    m_Preferences.clear();
    ESP_LOGI("InitializePreferences", "Preferences Cleared!");
    m_Preferences.putBool("Pref_Reset", false);
  }
}

void OutputSystemStatus()
{
  ESP_LOGE("LED_Controller2", "Xtal Clock Frequency: %i MHz", getXtalFrequencyMhz());
  ESP_LOGE("LED_Controller2", "CPU Clock Frequency: %i MHz", getCpuFrequencyMhz());
  ESP_LOGE("LED_Controller2", "Apb Clock Frequency: %i Hz", getApbFrequency());
  ESP_LOGE("LED_Controller_CPU2", "Total heap: %d", ESP.getHeapSize());
  ESP_LOGE("LED_Controller_CPU2", "Free heap: %d", ESP.getFreeHeap());
  ESP_LOGE("LED_Controller_CPU2", "Total PSRAM: %d", ESP.getPsramSize());
  ESP_LOGE("LED_Controller_CPU2", "Free PSRAM: %d", ESP.getFreePsram());
}

void TestPSRam()
{
  ESP_LOGE("Main", "Used PSRAM: %d", ESP.getPsramSize() - ESP.getFreePsram());  
  byte* psdRamBuffer = (byte*)ps_malloc(500000);
  ESP_LOGE("Main", "Used PSRAM: %d", ESP.getPsramSize() - ESP.getFreePsram());
  free(psdRamBuffer);
  ESP_LOGE("Main", "Used PSRAM: %d", ESP.getPsramSize() - ESP.getFreePsram());
}

void setup() 
{
  //PC Serial Communication
  Serial.flush();
  Serial.begin(500000, SERIAL_8N1);
  Serial.flush();
  
  Serial1.setRxBufferSize(4096);
  Serial1.flush();
  Serial1.begin(500000, SERIAL_8N1, CPU1_RX, CPU1_TX);
  Serial1.flush();

  Serial2.setRxBufferSize(4096);
  Serial2.flush();
  Serial2.begin(500000, SERIAL_8N1, CPU3_RX, CPU3_TX);  
  Serial2.flush();

  //esp_log_level_set("*", ESP_LOG_INFO); // Set global log level
  //esp_log_level_set("tag_name", ESP_LOG_DEBUG); // Set log level for specific tag
  TestPSRam();

  InitializePreferences();
  m_CPU1SerialPortMessageManager.SetupSerialPortMessageManager();
  m_CPU3SerialPortMessageManager.SetupSerialPortMessageManager();
  m_SoundProcessor.SetupSoundProcessor(); 
  m_I2S_In.Setup();
  a2dp_source.set_ssid_callback(ConnectToThisName);
  m_BT_Out.Setup();
  m_BT_Out.SetMusicDataCallback(SetBTTxData);
  m_Manager.Setup();
  
  OutputSystemStatus();
}

void loop()
{
  ESP_LOGI("Loop", "Loop");
  delay(1000);
}