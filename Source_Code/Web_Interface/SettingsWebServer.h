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
#ifndef SETTINGS_WEB_SERVER_H
#define SETTINGS_WEB_SERVER_H

#include "WebSocketDataHandler.h"
#include "DataItem.h"

class SettingsWebServerManager
{  
  public:
    SettingsWebServerManager( String Title
                            , AsyncWebSocket &WebSocket
                            , SerialPortMessageManager &CPU1SerialPortMessageManager
                            , SerialPortMessageManager &CPU2SerialPortMessageManager)
                            : m_WebSocket(WebSocket)
                            , m_CPU1SerialPortMessageManager(CPU1SerialPortMessageManager)
                            , m_CPU2SerialPortMessageManager(CPU2SerialPortMessageManager)
    {
    }
    
    virtual ~SettingsWebServerManager()
    {
    }
    
    void SetupSettingsWebServerManager()
    {
      InitializePreferences();
    }

    void BeginWebServer()
    {
      InitWiFiAP();
    }
    
    void InitializePreferences()
    {
      m_Preferences.begin("Settings", false);
      if(m_Preferences.getBool("Pref_Reset", false)) ClearPreferences();
      m_Preferences.putBool("Pref_Reset", false);
    }

    void ClearPreferences()
    {
      m_Preferences.clear();
      ESP_LOGI("Settings Web Server: ClearPreferences", "Preferences Cleared");
    }
    
    void OnEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
    {
      switch (type) 
      {
        case WS_EVT_CONNECT:
          Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
          break;
        case WS_EVT_PONG:
          Serial.printf("WebSocket client #%u Pinged Us!\n", client->id());
          break;
        case WS_EVT_DATA:
          HandleWebSocketMessage(client, arg, data, len);
          break;
        case WS_EVT_ERROR:
          Serial.printf("WebSocket client #%u Error. Closing Connection!\n", client->id());
          break;
        case WS_EVT_DISCONNECT:
          Serial.printf("WebSocket client #%u disconnected. Closing Connection.\n", client->id());
          break;
      }
    } 
  private:
    Preferences m_Preferences;
    SerialPortMessageManager &m_CPU1SerialPortMessageManager;
    SerialPortMessageManager &m_CPU2SerialPortMessageManager;
    AsyncWebSocket &m_WebSocket;
    WebSocketDataProcessor m_WebSocketDataProcessor = WebSocketDataProcessor(m_WebSocket);
    const char* password = "LEDs Rock";
    
    //Amplitude Gain
    const float m_AmplitudeGain_InitialValue = 2.0;
    DataItemWithPreferences <float, 1> m_AmplitudeGain = DataItemWithPreferences<float, 1>( "Amp_Gain", m_AmplitudeGain_InitialValue, RxTxType_Tx_On_Change_With_Heartbeat, UpdateStoreType_On_Rx, 5000, &m_Preferences, m_CPU2SerialPortMessageManager);
    WebSocketDataHandler<float, 1> m_Amplitude_Gain_DataHandler = WebSocketDataHandler<float, 1>( "Amplitude Gain Web Socket Handler", {"Amplitude_Gain"}, m_WebSocketDataProcessor, true, true, m_AmplitudeGain, false );    
    
    //FFT Gain
    const float m_FFTGain_InitialValue = 2.0;
    DataItemWithPreferences <float, 1> m_FFTGain = DataItemWithPreferences<float, 1>( "FFT_Gain", m_FFTGain_InitialValue, RxTxType_Tx_On_Change_With_Heartbeat, UpdateStoreType_On_Rx, 5000, &m_Preferences, m_CPU2SerialPortMessageManager);
    WebSocketDataHandler<float, 1> m_FFT_Gain_DataHandler = WebSocketDataHandler<float, 1>( "FFT Gain Web Socket Handler", {"FFT_Gain"}, m_WebSocketDataProcessor, true, true, m_FFTGain, false );

    //Microphone Enable
    //TBD

    //Input Source
    const SoundInputSource_t m_SoundInputSource_InitialValue = SoundInputSource_t::SoundInputSource_OFF;
    DataItemWithPreferences<SoundInputSource_t, 1> m_SoundInputSource = DataItemWithPreferences<SoundInputSource, 1>( "Input_Source", m_SoundInputSource_InitialValue, RxTxType_Tx_On_Change_With_Heartbeat, UpdateStoreType_On_Rx, 5000, &m_Preferences, m_CPU1SerialPortMessageManager);
    WebSocketDataHandler<SoundInputSource_t, 1> m_SoundInputSource_DataHandler = WebSocketDataHandler<SoundInputSource, 1>( "Sound Input Source Web Socket Handler", {"Sound_Input_Source"}, m_WebSocketDataProcessor, true, true, m_SoundInputSource, false );
    
    //Output Source
    const SoundOutputSource_t m_SoundOuputSource_InitialValue = SoundOutputSource_t::SoundOutputSource_OFF;
    DataItemWithPreferences<SoundOutputSource_t, 1> m_SoundOuputSource = DataItemWithPreferences<SoundOutputSource_t, 1>( "Output_Source", m_SoundOuputSource_InitialValue, RxTxType_Tx_On_Change_With_Heartbeat, UpdateStoreType_On_Rx, 5000, &m_Preferences, m_CPU2SerialPortMessageManager);
    WebSocketDataHandler<SoundOutputSource_t, 1> m_SoundOuputSource_DataHandler = WebSocketDataHandler<SoundOutputSource_t, 1>( "Sound Output Source Web Socket Handler", {"Sound_Output_Source"}, m_WebSocketDataProcessor, true, true, m_SoundOuputSource, false );

    //Bluetooth Sink Enable
    const bool m_BluetoothSinkEnable_InitialValue = false;
    DataItemWithPreferences<bool, 1> m_BluetoothSinkEnable = DataItemWithPreferences<bool, 1>( "BT_Sink_En", m_BluetoothSinkEnable_InitialValue, RxTxType_Tx_On_Change_With_Heartbeat, UpdateStoreType_On_Rx, 5000, &m_Preferences, m_CPU1SerialPortMessageManager);
    WebSocketDataHandler<bool, 1> m_BluetoothSinkEnable_DataHandler = WebSocketDataHandler<bool, 1>( "Bluetooth Sink Enable Web Socket Handler", {"BT_Sink_Enable"}, m_WebSocketDataProcessor, true, true, m_BluetoothSinkEnable, false );

    //Sink Name
    const String m_SinkName_InitialValue = "LED Tower of Power";  
    StringDataItemWithPreferences m_BluetoothSinkName = StringDataItemWithPreferences( "BT_Sink_Name", m_SinkName_InitialValue.c_str(), RxTxType_Tx_On_Change_With_Heartbeat, UpdateStoreType_On_Rx, 5000, &m_Preferences, m_CPU1SerialPortMessageManager);
    WebSocketDataHandler<char, 50> m_BluetoothSinkName_DataHandler = WebSocketDataHandler<char, 50>( "Bluetooth Sink Name Web Socket Handler", {"BT_Sink_Name"}, m_WebSocketDataProcessor, true, true, m_BluetoothSinkName, false );

    //Sink Connection Status
    const ConnectionStatus_t m_SinkConnectionStatus_InitialValue = ConnectionStatus_t::Disconnected;
    DataItem<ConnectionStatus_t, 1> m_SinkConnectionStatus = DataItem<ConnectionStatus_t, 1>( "Sink_Conn_Stat", m_SinkConnectionStatus_InitialValue, RxTxType_Rx_Only, UpdateStoreType_On_Rx, 0, m_CPU1SerialPortMessageManager);
    WebSocketDataHandler<ConnectionStatus_t, 1> m_SinkConnectionStatus_DataHandler = WebSocketDataHandler<ConnectionStatus_t, 1>( "BT Sink Connection Status Web Socket Handler", {"BT_Sink_Connection_Status"}, m_WebSocketDataProcessor, false, true, m_SinkConnectionStatus, false );    
    
    //Bluetooth Sink Auto Reconnect
    const bool m_BluetoothSinkAutoReConnect_InitialValue = false;
    DataItemWithPreferences<bool, 1> m_BluetoothSinkAutoReConnect = DataItemWithPreferences<bool, 1>( "BT_Sink_AR", m_BluetoothSinkAutoReConnect_InitialValue, RxTxType_Tx_On_Change_With_Heartbeat, UpdateStoreType_On_Rx, 5000, &m_Preferences, m_CPU1SerialPortMessageManager);
    WebSocketDataHandler<bool, 1> m_BluetoothSinkAutoReConnect_DataHandler = WebSocketDataHandler<bool, 1>( "Bluetooth Source Enable Web Socket Handler", {"BT_Sink_Auto_ReConnect"}, m_WebSocketDataProcessor, true, true, m_BluetoothSinkAutoReConnect, false );
    
    //Bluetooth Source Enable
    const bool m_BluetoothSourceEnable_InitialValue = false;
    DataItemWithPreferences<bool, 1> m_BluetoothSourceEnable = DataItemWithPreferences<bool, 1>( "BT_Source_En", m_BluetoothSourceEnable_InitialValue, RxTxType_Tx_On_Change_With_Heartbeat, UpdateStoreType_On_Tx, 5000, &m_Preferences, m_CPU2SerialPortMessageManager);
    WebSocketDataHandler<bool, 1> m_BluetoothSourceEnable_DataHandler = WebSocketDataHandler<bool, 1>( "Bluetooth Source Enable Web Socket Handler", {"BT_Source_Enable"}, m_WebSocketDataProcessor, true, true, m_BluetoothSourceEnable, false );

    //Target Device
    CompatibleDevice_t m_TargetCompatibleDevice_InitialValue = {"", ""};
    DataItem<CompatibleDevice_t, 1> m_TargetCompatibleDevice = DataItem<CompatibleDevice_t, 1>( "Target_Device", m_TargetCompatibleDevice_InitialValue, RxTxType_Tx_On_Change_With_Heartbeat, UpdateStoreType_On_Rx, 5000, m_CPU2SerialPortMessageManager);
    WebSocket_Compatible_Device_DataHandler m_TargetCompatibleDevice_DataHandler = WebSocket_Compatible_Device_DataHandler("BT Target Device Web Socket Data Handler", {"BT_Source_Target_Device"}, m_WebSocketDataProcessor, true, true, m_TargetCompatibleDevice, false );

    //Scanned Device
    BT_Device_Info_With_Time_Since_Update_t m_ScannedDevice_InitialValue = {"", "", 0, 0};
    DataItem<BT_Device_Info_With_Time_Since_Update_t, 1> m_ScannedDevice = DataItem<BT_Device_Info_With_Time_Since_Update_t, 1>( "Scan_BT_Device", m_ScannedDevice_InitialValue, RxTxType_Rx_Only, UpdateStoreType_On_Rx, 0, m_CPU2SerialPortMessageManager);
    WebSocket_BT_Info_ArrayDataHandler m_ScannedDevice_DataHandler = WebSocket_BT_Info_ArrayDataHandler( "Scan BT Device Web Socket Data Handler", {"BT_Source_Target_Devices"}, m_WebSocketDataProcessor, true, true, m_ScannedDevice, false );
    
    //Bluetooth Source Auto Reconnect
    const bool m_BluetoothSourceAutoReConnect_InitialValue = false;
    DataItemWithPreferences<bool, 1> m_BluetoothSourceAutoReConnect = DataItemWithPreferences<bool, 1>( "BT_Source_AR", m_BluetoothSourceAutoReConnect_InitialValue, RxTxType_Tx_On_Change_With_Heartbeat, UpdateStoreType_On_Rx, 5000, &m_Preferences, m_CPU2SerialPortMessageManager);
    WebSocketDataHandler<bool, 1> m_BluetoothSourceAutoReConnect_DataHandler = WebSocketDataHandler<bool, 1>( "Bluetooth Source Auto Reconnect Web Socket Handler", {"BT_Source_Auto_Reconnect"}, m_WebSocketDataProcessor, true, true, m_BluetoothSourceAutoReConnect, false );

    //Source Connection Status
    const ConnectionStatus_t m_SourceConnectionStatus_InitialValue = ConnectionStatus_t::Disconnected;
    DataItem<ConnectionStatus_t, 1> m_SourceConnectionStatus = DataItem<ConnectionStatus_t, 1>( "Src_Conn_Stat", m_SourceConnectionStatus_InitialValue, RxTxType_Rx_Only, UpdateStoreType_On_Rx, 0, m_CPU2SerialPortMessageManager);
    WebSocketDataHandler<ConnectionStatus_t, 1> m_SourceConnectionStatus_DataHandler = WebSocketDataHandler<ConnectionStatus_t, 1>( "Source Connection Status Web Socket Handler", {"BT_Source_Connection_Status"}, m_WebSocketDataProcessor, false, true, m_SourceConnectionStatus, false );    

    //Source Reset
    const bool m_SourceReset_InitialValue = false;
    DataItem<bool, 1> m_SourceReset = DataItem<bool, 1>( "BT_Src_Reset", m_SourceReset_InitialValue, RxTxType_Tx_On_Change_With_Heartbeat, UpdateStoreType_On_Rx, 5000, m_CPU2SerialPortMessageManager);
    WebSocketDataHandler<bool, 1> m_SourceReset_DataHandler = WebSocketDataHandler<bool, 1>( "Source Reset Web Socket Handler", {"BT_Source_Reset"}, m_WebSocketDataProcessor, true, true, m_SourceReset, false );    

    void HandleWebSocketMessage(AsyncWebSocketClient *client, void *arg, uint8_t *data, size_t len)
    {
      AwsFrameInfo *info = (AwsFrameInfo*)arg;
      data[len] = 0;
      String WebSocketData = String((char*)data);
      Serial.println(WebSocketData.c_str());
      if ( WebSocketData.equals("Hello I am here!") )
      {
        ESP_LOGI("SettingsWebServer: HandleWebSocketMessage", "New Client Message: \"Hello I am here!\"");
        m_WebSocketDataProcessor.UpdateAllDataToClient(client->id());
      }
      else if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
      {
        data[len] = 0;
        ESP_LOGD("SettingsWebServer: HandleWebSocketMessage", "WebSocket Data from Client: %s", WebSocketData.c_str());
        JSONVar MyDataObject = JSON.parse(WebSocketData);
        if (JSON.typeof(MyDataObject) == "undefined")
        {
          ESP_LOGE("SettingsWebServer: HandleWebSocketMessage", "Parsing Web Socket Data failed!");
          return;
        }
        if( true == MyDataObject.hasOwnProperty("WidgetValue") )
        {
          if( true == MyDataObject["WidgetValue"].hasOwnProperty("Id") && 
              true == MyDataObject["WidgetValue"].hasOwnProperty("Value") )
          {
            const String WidgetId = String( (const char*)MyDataObject["WidgetValue"]["Id"]);
            const String Value = String( (const char*)MyDataObject["WidgetValue"]["Value"]);
            if(m_WebSocketDataProcessor.ProcessWebSocketValueAndSendToDatalink(WidgetId, Value))
            {
              ESP_LOGD("SettingsWebServer: HandleWebSocketMessage", "Known Widget: %s", WidgetId.c_str());
            }
            else
            {
              ESP_LOGE("SettingsWebServer: HandleWebSocketMessage", "Unknown Widget: %s", WidgetId.c_str());
            }
          }
          else
          {
            ESP_LOGE("SettingsWebServer: HandleWebSocketMessage", "Misconfigured Widget Value Data: %s", WebSocketData.c_str());
          }
        }
        else
        {
          ESP_LOGE("SettingsWebServer: HandleWebSocketMessage", "Unsupported Web Socket Data: $s", WebSocketData.c_str());
        }
      }
    }
    
    // Initialize WiFi Client
    void InitWiFiClient()
    {
      WiFi.mode(WIFI_STA);
      WiFi.begin(m_BluetoothSinkName.GetValueAsString("").c_str(), password);
      ESP_LOGI("SettingsWebServer: InitWifiClient", "Connecting to WiFi ..");
      while (WiFi.status() != WL_CONNECTED) {
        ESP_LOGI("SettingsWebServer: InitWifiClient", "Connecting...");
        delay(1000);
      }
      IPAddress ipAddress = WiFi.localIP();
      ESP_LOGI( "SettingsWebServer: InitWifiClient"
              , "Connected! IP Address: %i.%i.%i.%i"
              , ipAddress[0]
              , ipAddress[1]
              , ipAddress[2]
              , ipAddress[3] );
    }
    
    void InitWiFiAP()
    {
        // Setup ESP32 as Access Point
      IPAddress Ip(192, 168, 0, 1);
      IPAddress NMask(255, 255, 255, 0);
      
      WiFi.softAPConfig(Ip, Ip, NMask);
      WiFi.softAP(m_BluetoothSinkName.GetValueAsString("").c_str(), password);
      IPAddress ipAddress = WiFi.softAPIP();
      ESP_LOGI( "SettingsWebServer: InitWifiClient"
              , "Connected! IP Address: %i.%i.%i.%i"
              , ipAddress[0]
              , ipAddress[1]
              , ipAddress[2]
              , ipAddress[3] );
    }
};


#endif
