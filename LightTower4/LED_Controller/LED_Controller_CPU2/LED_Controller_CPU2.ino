
#include <BluetoothA2DPSource.h>
#include "Manager.h"
#include "Tunes.h"

TaskHandle_t ManagerTask;
TaskHandle_t SoundProcessorTask;
TaskHandle_t ProcessSoundPowerTask;
TaskHandle_t FilterAndDownSampleTask;
TaskHandle_t ProcessMaxBandTask;
TaskHandle_t SerialDataLinkTXTask;
TaskHandle_t SerialDataLinkRXTask;

I2S_Device m_I2S_In = I2S_Device( "I2S_In"
                                 , I2S_NUM_0
                                 , i2s_mode_t(I2S_MODE_SLAVE | I2S_MODE_RX)
                                 , I2S_SAMPLE_RATE
                                 , I2S_BITS_PER_SAMPLE_32BIT
                                 , I2S_CHANNEL_FMT_RIGHT_LEFT
                                 , i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB)
                                 , I2S_CHANNEL_STEREO
                                 , true                               // Use APLL
                                 , I2S_BUFFER_COUNT                   // Buffer Count
                                 , I2S_SAMPLE_COUNT                    // Buffer Size
                                 , 12                                 // Serial Clock Pin
                                 , 13                                 // Word Selection Pin
                                 , 14                                 // Serial Data In Pin
                                 , I2S_PIN_NO_CHANGE );               // Serial Data Out Pin 
                      
I2S_Device m_I2S_Out = I2S_Device( "I2S_Out"
                                  , I2S_NUM_1                        // I2S Interface
                                  , i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_TX)
                                  , I2S_SAMPLE_RATE
                                  , I2S_BITS_PER_SAMPLE_32BIT
                                  , I2S_CHANNEL_FMT_RIGHT_LEFT
                                  , i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB)
                                  , I2S_CHANNEL_STEREO
                                  , true                             // Use APLL
                                  , I2S_BUFFER_COUNT                 // Buffer Count
                                  , I2S_SAMPLE_COUNT                  // Buffer Size
                                  , 25                               // Serial Clock Pin
                                  , 26                               // Word Selection Pin
                                  , I2S_PIN_NO_CHANGE                // Serial Data In Pin
                                  , 33 );  

HardwareSerial m_hSerial = Serial2;
SerialDataLink m_SerialDataLink = SerialDataLink("Serial Datalink", m_hSerial);
Sound_Processor m_SoundProcessor = Sound_Processor("Sound Processor", m_SerialDataLink);
Manager m_Manager = Manager("Manager", m_SoundProcessor, m_SerialDataLink, m_I2S_In, m_I2S_Out);

void setup() {

  //ESP32 Serial Communication
  m_hSerial.end();
  m_hSerial.setRxBufferSize(1024);
  m_hSerial.begin(9600, SERIAL_8N1, 16, 17); // pins 16 rx2, 17 tx2, 19200 bps, 8 bits no parity 1 stop bit
  m_hSerial.updateBaudRate(400000); //For whatever reason, if I set it to 400000 in setup, it crashes a lot of the time.
  m_hSerial.flush();

  //PC Serial Communication
  Serial.end();
  Serial.begin(9600, SERIAL_8N1, 16, 17); // pins 16 rx2, 17 tx2, 9600 bps, 8 bits no parity 1 stop bit
  Serial.updateBaudRate(500000); //For whatever reason, if I set it to 500000 in setup, it crashes a lot of the time.
  Serial.flush();

  Serial << "Serial Datalink Configured\n";
  Serial << "Xtal Clock Frequency: " << getXtalFrequencyMhz() << " MHz\n";
  Serial << "CPU Clock Frequency: " << getCpuFrequencyMhz() << " MHz\n";
  Serial << "Apb Clock Frequency: " << getApbFrequency() << " Hz\n";
 
  m_I2S_Out.Setup();
  m_I2S_In.Setup();
  m_Manager.Setup();
  m_SerialDataLink.SetupSerialDataLink();

  xTaskCreatePinnedToCore
  (
    ManagerTaskLoop,                // Function to implement the task
    "ManagerTask",                  // Name of the task
    4000,                           // Stack size in words
    NULL,                           // Task input parameter
    configMAX_PRIORITIES - 1,       // Priority of the task
    &ManagerTask,                   // Task handle.
    1                               // Core where the task should run
  );
  
  xTaskCreatePinnedToCore
  (
    SoundProcessorTaskLoop,         // Function to implement the task
    "SoundProcessorTask",           // Name of the task
    4000,                           // Stack size in words
    NULL,                           // Task input parameter
    configMAX_PRIORITIES - 1,       // Priority of the task
    &SoundProcessorTask,            // Task handle.
    0                               // Core where the task should run
  );
  
  xTaskCreatePinnedToCore
  (
    ProcessSoundPowerTaskLoop,      // Function to implement the task
    "ProcessSoundPowerTask",        // Name of the task
    4000,                           // Stack size in words
    NULL,                           // Task input parameter
    configMAX_PRIORITIES - 1,       // Priority of the task
    &ProcessSoundPowerTask,         // Task handle.
    0                               // Core where the task should run
  );
  
  xTaskCreatePinnedToCore
  (
    FilterAndDownSampleTaskLoop,    // Function to implement the task
    "FilterAndDownSampleTask",      // Name of the task
    4000,                           // Stack size in words
    NULL,                           // Task input parameter
    configMAX_PRIORITIES - 1,       // Priority of the task
    &FilterAndDownSampleTask,       // Task handle.
    0                               // Core where the task should run
  );
  
  xTaskCreatePinnedToCore
  (
    ProcessMaxBandTaskLoop,         // Function to implement the task
    "ProcessMaxBandTask",           // Name of the task
    4000,                           // Stack size in words
    NULL,                           // Task input parameter
    configMAX_PRIORITIES - 1,       // Priority of the task
    &ProcessMaxBandTask,            // Task handle.
    0                               // Core where the task should run
  );
  
  xTaskCreatePinnedToCore
  (
    SerialDataLinkTXTaskLoop,       // Function to implement the task
    "SerialDataLinkTXTask",         // Name of the task
    4000,                           // Stack size in words
    NULL,                           // Task input parameter
    configMAX_PRIORITIES - 2,       // Priority of the task
    &SerialDataLinkTXTask,          // Task handle.
    1                               // Core where the task should run
  );

  xTaskCreatePinnedToCore
  (
    SerialDataLinkRXTaskLoop,       // Function to implement the task
    "SerialDataLinkRXTask",         // Name of the task
    2000,                           // Stack size in words
    NULL,                           // Task input parameter
    configMAX_PRIORITIES - 2,       // Priority of the task
    &SerialDataLinkRXTask,          // Task handle.
    1                               // Core where the task should run
  );
  Serial << "Free Heap: " << ESP.getFreeHeap() << "\n";
}
void loop()
{
  // put your main code here, to run repeatedly:
}

void ManagerTaskLoop(void * parameter)
{
  while(true)
  {
    yield();
    m_Manager.ProcessEventQueue();
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

void SoundProcessorTaskLoop(void * parameter)
{
  while(true)
  {
    yield();
    m_SoundProcessor.ProcessEventQueue();
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

void ProcessSoundPowerTaskLoop(void * parameter)
{
  while(true)
  {
    yield();
    m_SoundProcessor.ProcessSoundPower();
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

void FilterAndDownSampleTaskLoop(void * parameter)
{
  while(true)
  {
    yield();
    m_SoundProcessor.FilterAndDownSample();
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

void ProcessMaxBandTaskLoop(void * parameter)
{
  while(true)
  {
    yield();
    m_SoundProcessor.ProcessMaxBand();
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

void SerialDataLinkRXTaskLoop(void * parameter)
{
  while(true)
  {
    yield();
    m_SerialDataLink.ProcessDataRXEventQueue();
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

void SerialDataLinkTXTaskLoop(void * parameter)
{
  while(true)
  {
    yield();
    m_SerialDataLink.ProcessDataTXEventQueue();
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}