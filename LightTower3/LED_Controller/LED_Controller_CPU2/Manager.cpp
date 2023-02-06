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

#include "Manager.h"

Manager::Manager( String Title
                , Sound_Processor &SoundProcessor
                , SPIDataLinkToCPU1 &SPIDataLinkToCPU1
                , SPIDataLinkToCPU3 &SPIDataLinkToCPU3
                , Bluetooth_Source &BT_Out
                , I2S_Device &I2S_In
                , ContinuousAudioBuffer<AUDIO_BUFFER_SIZE> &AudioBuffer )
                : NamedItem(Title)
                , m_SoundProcessor(SoundProcessor)
                , m_SPIDataLinkToCPU1(SPIDataLinkToCPU1)
                , m_SPIDataLinkToCPU3(SPIDataLinkToCPU3)
                , m_BT_Out(BT_Out)
                , m_I2S_In(I2S_In)
                , m_AudioBuffer(AudioBuffer)
{
}
Manager::~Manager()
{
}

void Manager::Setup()
{
  m_Preferences.begin("My Settings", false);
  m_Preferences.clear();
  esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, ESP_PWR_LVL_P9); //Set Bluetooth Power to Max
  m_SoundProcessor.SetupSoundProcessor();
  m_AudioBuffer.Initialize();
  m_I2S_In.StartDevice();

  m_Preferences.putString("Target Speaker SSID", "JBL Flip 6");
  m_Preferences.putBool("Reset Bluetooth", true);
  m_Preferences.putBool("Auto ReConnect", true);
  m_Preferences.putBool("SSP Enabled", false);
  
  m_BT_Out.StartDevice( m_Preferences.getString("Target Speaker SSID", "JBL Flip 6").c_str()
                      , m_Preferences.getBool("Reset Bluetooth", false)
                      , m_Preferences.getBool("Auto ReConnect", false)
                      , m_Preferences.getBool("SSP Enabled", false) );
}

void Manager::ProcessEventQueue20mS()
{
  m_I2S_In.ProcessEventQueue();
  MoveDataBetweenCPU1AndCPU3();
  AmplitudeGain_RX();
  FFTGain_RX();
  ResetBluetooth_RX();
  AutoReConnect_RX();
  SpeakerSSID_RX();
}

void Manager::ProcessEventQueue1000mS()
{
  AmplitudeGain_TX();
  FFTGain_TX();
  ResetBluetooth_TX();
  AutoReConnect_TX();
  SpeakerSSID_TX();
}

void Manager::MoveDataBetweenCPU1AndCPU3()
{
  const uint8_t count = 2;
  String Signals[count] = { "Source Is Connected"
                          , "Sound State" };
  
                      
  for(int i = 0; i < count; ++i)
  {
    MoveDataFromQueueToQueue( "Manager1: " + Signals[i]
                            , m_SPIDataLinkToCPU1.GetQueueHandleRXForDataItem(Signals[i].c_str())
                            , m_SPIDataLinkToCPU3.GetQueueHandleTXForDataItem(Signals[i].c_str())
                            , m_SPIDataLinkToCPU1.GetTotalByteCountForDataItem(Signals[i].c_str())
                            , 0
                            , false );
  }
  m_SPIDataLinkToCPU3.TriggerEarlyDataTransmit();
}

//I2S_Device_Callback
void Manager::I2SDataReceived(String DeviceTitle, uint8_t *Data, uint32_t channel_len)
{
}

//Bluetooth Source Callback
int32_t Manager::SetBTTxData(uint8_t *Data, int32_t channel_len)
{
  size_t ByteReceived = m_I2S_In.ReadSoundBufferData(Data, channel_len);
  assert(0 == ByteReceived % sizeof(uint32_t)); 
  size_t FrameCount = ByteReceived / sizeof(uint32_t);
  m_AudioBuffer.Push((Frame_t*)Data, FrameCount);
  return ByteReceived;
}

void Manager::AmplitudeGain_RX()
{
  //Set Amplitude Gain from Amplitude Gain RX QUEUE
  float Value;
  static bool AmplitudeGainPullErrorHasOccured = false;
  if(true == m_SPIDataLinkToCPU3.GetValueFromRXQueue(&Value, "Amplitude Gain", false, 0, AmplitudeGainPullErrorHasOccured))
  {
    if(false == AreEqual(Value, m_SoundProcessor.GetGain()))
    {
      m_SoundProcessor.SetGain(Value);
      AmplitudeGain_TX();
    }
  }
}
void Manager::AmplitudeGain_TX()
{
  float Value = m_SoundProcessor.GetGain();
  static bool AmplitudeGainPushErrorHasOccured = false;
  m_SPIDataLinkToCPU3.PushValueToTXQueue(&Value, "Amplitude Gain", 0, AmplitudeGainPushErrorHasOccured);
}

void Manager::FFTGain_RX()
{
  //Set FFT Gain from FFT Gain RX QUEUE
  float Value;
  static bool FFTGainPullErrorHasOccured = false;
  if(true == m_SPIDataLinkToCPU3.GetValueFromRXQueue(&Value, "FFT Gain", false, 0, FFTGainPullErrorHasOccured))
  {
    if(false == AreEqual(Value, m_SoundProcessor.GetFFTGain()))
    {
      m_SoundProcessor.SetFFTGain(Value);
      FFTGain_TX();
    }
  }
}
void Manager::FFTGain_TX()
{
  Serial << "FFT Gain\n";
  float Value = m_SoundProcessor.GetFFTGain();
  static bool FFTGainPushErrorHasOccured = false;
  m_SPIDataLinkToCPU3.PushValueToTXQueue(&Value, "FFT Gain", 0, FFTGainPushErrorHasOccured);
}

void Manager::ResetBluetooth_RX()
{
  bool NVMValue = m_Preferences.getBool("Reset Bluetooth", true);
  bool DatalinkValue;
  static bool ResetBluetoothPullErrorHasOccured = false;
  if(true == m_SPIDataLinkToCPU3.GetValueFromRXQueue(&DatalinkValue, "Reset Bluetooth", false, 0, ResetBluetoothPullErrorHasOccured))
  {
    if(NVMValue != DatalinkValue)
    {
      Serial << "Reset Bluetooth Value Changed\n";
      m_Preferences.putBool("Reset Bluetooth", DatalinkValue);
      ResetBluetooth_TX();
    }
  }
}
void Manager::ResetBluetooth_TX()
{
  bool NVMValue = m_Preferences.getBool("Reset Bluetooth", true);
  static bool ResetBluetoothPushErrorHasOccured = false;
  m_SPIDataLinkToCPU3.PushValueToTXQueue(&NVMValue, "Reset Bluetooth", 0, ResetBluetoothPushErrorHasOccured);
}

void Manager::AutoReConnect_RX()
{
  bool NVMValue = m_Preferences.getBool("Auto ReConnect", true);
  bool DatalinkValue;
  static bool AutoReConnectPullErrorHasOccured = false;
  if(true == m_SPIDataLinkToCPU3.GetValueFromRXQueue(&DatalinkValue, "Auto ReConnect", false, 0, AutoReConnectPullErrorHasOccured))
  {
    if(NVMValue != DatalinkValue)
    {
      Serial << "Auto ReConnect Value Changed\n";
      m_Preferences.putBool("Auto ReConnect", DatalinkValue);
      AutoReConnect_TX();
    }
  }
}
void Manager::AutoReConnect_TX()
{
  bool NVMValue = m_Preferences.getBool("Auto ReConnect", true);
  static bool AutoReConnectPushErrorHasOccured = false;
  m_SPIDataLinkToCPU3.PushValueToTXQueue(&NVMValue, "Auto ReConnect", 0, AutoReConnectPushErrorHasOccured);
}

void Manager::SpeakerSSID_RX()
{

}
void Manager::SpeakerSSID_TX()
{
  
}
