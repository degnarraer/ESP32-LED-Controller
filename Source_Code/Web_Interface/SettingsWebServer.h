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

//#include "WebSocketDataHandler.h"
#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "DataItem.h"

class SettingsWebServerManager
{  
  public:
    SettingsWebServerManager( String Title
                            , AsyncWebSocket &WebSocket )
                            : m_WebSocket(WebSocket)
    {
      mySenderSemaphore = xSemaphoreCreateRecursiveMutex();
      myReceiverSemaphore = xSemaphoreCreateRecursiveMutex();
      xSemaphoreGive(mySenderSemaphore);
      xSemaphoreGive(myReceiverSemaphore);
    }

    void InitializeMemory()
    {
      /*
      Sound_State_DataHandler = new WebSocketDataHandler<SoundState_t>( GetPointerToDataItemWithName("Sound State"), new String[1]{"Speaker_Image"}, 1, false, 0, false );
      Amplitude_Gain_DataHandler = new WebSocketDataHandler<float>( GetPointerToDataItemWithName("Amplitude Gain"), new String[2]{"Amplitude_Gain_Slider1", "Amplitude_Gain_Slider2"}, 2, true, 0, false );
      FFT_Gain_DataHandler = new WebSocketDataHandler<float>( GetPointerToDataItemWithName("FFT Gain"), new String[2]{"FFT_Gain_Slider1", "FFT_Gain_Slider2"}, 2, true, 0, false );
      SinkSSID_DataHandler = new WebSocketSSIDDataHandler( GetPointerToDataItemWithName("Sink SSID"), new String[1]{"Sink_SSID_Text_Box"}, 1, true, 0, false );
      SourceSSID_DataHandler = new WebSocketSSIDDataHandler( GetPointerToDataItemWithName("Source SSID"), new String[1]{"Source_SSID_Text_Box"}, 1, true, 0, false );
      SourceSSIDs_DataHandler = new WebSocketSSIDArrayDataHandler( GetPointerToDataItemWithName("Found Speaker SSIDS"), new String[1]{"TBD"}, 1, true, 0, false );
      Source_Connection_Status_DataHandler = new WebSocketDataHandler<ConnectionStatus_t>( GetPointerToDataItemWithName("Source Connection Status"), new String[1]{"Source_Connection_Status"}, 1, true, 0, false );
      Source_BT_Reset_DataHandler = new WebSocketDataHandler<bool>( GetPointerToDataItemWithName("Source BT Reset"), new String[1]{"Source_BT_Reset_Toggle_Button"}, 1, true, 0, false );
      Source_BT_ReConnect_DataHandler = new WebSocketDataHandler<bool>( GetPointerToDataItemWithName("Source ReConnect"), new String[1]{"Source_BT_Auto_ReConnect_Toggle_Button"}, 1, true, 0, false );
      Green_Value_DataHandler = new WebSocketDataHandler<uint8_t>( GetPointerToDataItemWithName("Green Value"), new String[1]{"Green_Value_Slider"}, 1, true, 0, false );
      Blue_Value_DataHandler = new WebSocketDataHandler<uint8_t>( GetPointerToDataItemWithName("Blue Value"), new String[1]{"Blue_Value_Slider"}, 1, true, 0, false );
      Red_Value_DataHandler = new WebSocketDataHandler<uint8_t>( GetPointerToDataItemWithName("Red Value"), new String[1]{"Red_Value_Slider"}, 1, true, 0, false );
      Sink_BT_ReConnect_DataHandler = new WebSocketDataHandler<bool>( GetPointerToDataItemWithName("Sink ReConnect"), new String[1]{"Sink_BT_Auto_ReConnect_Toggle_Button"}, 1, true, 0, false );
      Sink_Connection_Status_DataHandler = new WebSocketDataHandler<ConnectionStatus_t>( GetPointerToDataItemWithName("Sink Connection Status"), new String[1]{"Sink_Connection_Status"}, 1, true, 0, false );
      Sink_Connection_Enable_DataHandler = new WebSocketDataHandler<bool>( GetPointerToDataItemWithName("Sink Enable"), new String[1]{"Sink_BT_Enable_Toggle_Button"}, 1, true, 0, false );
      */
    }
    
    virtual ~SettingsWebServerManager()
    {
      /*
      free(Sound_State_DataHandler);
      free(Amplitude_Gain_DataHandler);
      free(FFT_Gain_DataHandler);
      free(SinkSSID_DataHandler);
      free(SourceSSID_DataHandler);
      free(SourceSSIDs_DataHandler);
      free(Source_Connection_Status_DataHandler);
      free(Source_BT_Reset_DataHandler);
      free(Source_BT_ReConnect_DataHandler);
      free(Green_Value_DataHandler);
      free(Blue_Value_DataHandler);
      free(Red_Value_DataHandler);
      free(Sink_BT_ReConnect_DataHandler);
      free(Sink_Connection_Status_DataHandler);
      free(Sink_Connection_Enable_DataHandler);
      */
    }
    
    void SetupSettingsWebServerManager()
    {
      InitWiFiAP();
      InitializeMemory();
      m_CPU1SerialPortMessageManager.SetupSerialPortMessageManager();
      m_CPU2SerialPortMessageManager.SetupSerialPortMessageManager();

      /*
      RegisterAsWebSocketDataSender(Sound_State_DataHandler);
      
      RegisterAsWebSocketDataReceiver(Amplitude_Gain_DataHandler);
      RegisterAsWebSocketDataSender(Amplitude_Gain_DataHandler);
      
      RegisterAsWebSocketDataReceiver(FFT_Gain_DataHandler);
      RegisterAsWebSocketDataSender(FFT_Gain_DataHandler);
      
      RegisterAsWebSocketDataReceiver(SinkSSID_DataHandler);
      RegisterAsWebSocketDataSender(SinkSSID_DataHandler);
    
      RegisterAsWebSocketDataReceiver(SourceSSID_DataHandler);
      RegisterAsWebSocketDataSender(SourceSSID_DataHandler);

      RegisterAsWebSocketDataSender(SourceSSIDs_DataHandler);
      
      RegisterAsWebSocketDataSender(Source_Connection_Status_DataHandler);
      
      RegisterAsWebSocketDataReceiver(Source_BT_Reset_DataHandler);
      RegisterAsWebSocketDataSender(Source_BT_Reset_DataHandler);
      
      RegisterAsWebSocketDataReceiver(Source_BT_ReConnect_DataHandler);
      RegisterAsWebSocketDataSender(Source_BT_ReConnect_DataHandler);

      RegisterAsWebSocketDataReceiver(Sink_Connection_Enable_DataHandler);
      RegisterAsWebSocketDataSender(Sink_Connection_Enable_DataHandler);
      
      RegisterAsWebSocketDataSender(Sink_Connection_Status_DataHandler);
      
      RegisterAsWebSocketDataReceiver(Sink_BT_ReConnect_DataHandler);
      RegisterAsWebSocketDataSender(Sink_BT_ReConnect_DataHandler);

      RegisterAsWebSocketDataReceiver(Red_Value_DataHandler);
      RegisterAsWebSocketDataSender(Red_Value_DataHandler);

      RegisterAsWebSocketDataReceiver(Blue_Value_DataHandler);
      RegisterAsWebSocketDataSender(Blue_Value_DataHandler);

      RegisterAsWebSocketDataReceiver(Green_Value_DataHandler);
      RegisterAsWebSocketDataSender(Green_Value_DataHandler);
      */
    }
    
    void ProcessEventQueue()
    {
      /*
      if(xSemaphoreTake(mySenderSemaphore, portMAX_DELAY) == pdTRUE)
      {
        std::vector<KVP> KeyValuePairs = std::vector<KVP>();
        for(int i = 0; i < m_MySenders.size(); ++i)
        {
          m_MySenders[i]->CheckForNewDataLinkValueAndSendToWebSocket(KeyValuePairs);
        }
        xSemaphoreGive(mySenderSemaphore);
        if(KeyValuePairs.size() > 0)
        {
          NotifyClients(Encode_Widget_Values_To_JSON(KeyValuePairs));
        }
      }
      */
    }
    
    void OnEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
    {
      switch (type) 
      {
        case WS_EVT_CONNECT:
          Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
          break;
        case WS_EVT_PONG:
          break;
        case WS_EVT_DATA:
          //HandleWebSocketMessage(arg, data, len);
          break;
        case WS_EVT_ERROR:
          Serial.printf("WebSocket client #%u Error. Closing Connection!\n", client->id());
          break;
        case WS_EVT_DISCONNECT:
          Serial.printf("WebSocket client #%u disconnected. Closing Connection.\n", client->id());
          break;
      }
    }
    /*
    void RegisterAsWebSocketDataReceiver(WebSocketDataHandlerReceiver *aReceiver)
    {
      if(xSemaphoreTake(myReceiverSemaphore, portMAX_DELAY) == pdTRUE)
      {
        for(int i = 0; i < m_MyReceivers.size(); ++i)
        {
          if(m_MyReceivers[i] == aReceiver)
          {
            xSemaphoreGive(myReceiverSemaphore);
            return;
          }
        }
        m_MyReceivers.push_back(aReceiver);
        xSemaphoreGive(myReceiverSemaphore);
      }
    }

    void DeRegisterAsWebSocketDataReceiver(WebSocketDataHandlerReceiver *aReceiver)
    {
      if(xSemaphoreTake(myReceiverSemaphore, portMAX_DELAY) == pdTRUE)
      {
        for(int i = 0; i < m_MyReceivers.size(); ++i)
        {
          if(m_MyReceivers[i] == aReceiver)
          {
            m_MyReceivers.erase(m_MyReceivers.begin() + i);
            xSemaphoreGive(myReceiverSemaphore);
            return;
          }
        }
        xSemaphoreGive(myReceiverSemaphore);
      }
    }
    
    void RegisterAsWebSocketDataSender(WebSocketDataHandlerSender *aSender)
    {
      if(xSemaphoreTake(mySenderSemaphore, portMAX_DELAY) == pdTRUE)
      {
        for(int i = 0; i < m_MySenders.size(); ++i)
        {
          if(m_MySenders[i] == aSender)
          {
            xSemaphoreGive(mySenderSemaphore);
            return;
          }
        }
        m_MySenders.push_back(aSender);
        xSemaphoreGive(mySenderSemaphore);
      }
    }
    
    void DeRegisterAsWebSocketDataSender(WebSocketDataHandlerSender *aSender)
    {
      if(xSemaphoreTake(mySenderSemaphore, portMAX_DELAY) == pdTRUE)
      {
        for(int i = 0; i < m_MySenders.size(); ++i)
        {
          if(m_MySenders[i] == aSender)
          {
            m_MySenders.erase(m_MySenders.begin() + i);
            xSemaphoreGive(mySenderSemaphore);
            return;
          }
        }
        xSemaphoreGive(mySenderSemaphore);
      }
    }
    */
  private:
    SemaphoreHandle_t mySenderSemaphore;
    SemaphoreHandle_t myReceiverSemaphore;
    AsyncWebSocket &m_WebSocket;
    const char* ssid = "LED Tower of Power";
    const char* password = "LEDs Rock";
    String message = "";

    //std::vector<WebSocketDataHandlerReceiver*> m_MyReceivers = std::vector<WebSocketDataHandlerReceiver*>();
    //std::vector<WebSocketDataHandlerSender*> m_MySenders = std::vector<WebSocketDataHandlerSender*>();

    DataSerializer m_DataSerializer;  
    SerialPortMessageManager m_CPU1SerialPortMessageManager = SerialPortMessageManager("CPU1", Serial1, m_DataSerializer);
    SerialPortMessageManager m_CPU2SerialPortMessageManager = SerialPortMessageManager("CPU2", Serial2, m_DataSerializer);
    
    DataItem <float, 1> AmplitudeGain = DataItem<float, 1>( "Amplitude Gain"
                                                          , 0, RxTxType_Tx_On_Change
                                                          , 0
                                                          , m_CPU2SerialPortMessageManager);
    
    DataItem <float, 1> FFTGain = DataItem<float, 1>( "FFT Gain"
                                                     , 0
                                                     , RxTxType_Tx_On_Change
                                                     , 0
                                                     , m_CPU2SerialPortMessageManager);
    
    DataItem <SSID_Info_With_LastUpdateTime_t, 1> m_SSIDWLUT = DataItem<SSID_Info_With_LastUpdateTime_t, 1>( "Available SSID"
                                                                                                           , SSID_Info_With_LastUpdateTime_t("\0", "\0", 0, 0)
                                                                                                           , RxTxType_Rx
                                                                                                           , 0
                                                                                                           , m_CPU2SerialPortMessageManager);

    DataItem<ConnectionStatus_t, 1> m_ConnectionStatus = DataItem<ConnectionStatus_t, 1>( "Connection Status"
                                                                                        , Disconnected
                                                                                        , RxTxType_Rx
                                                                                        , 0
                                                                                        , m_CPU2SerialPortMessageManager);
    
    
    
    //DataItem <bool, 1> SourceReConnect = DataItem<bool, 1>("Source Reconnect", 0, RxTxType_Rx, 1000, m_DataSerializer, m_CPU1SerialPortMessageManager);
    //DataItem <bool, 1> SourceBTReset = DataItem<bool, 1>("Source BT Reset", 0, RxTxType_Rx, 1000, m_DataSerializer, m_CPU2SerialPortMessageManager);
    //DataItem <bool, 1> SinkEnable = DataItem<bool, 1>("Sink Enable", 0, RxTxType_Rx, 1000, m_DataSerializer, m_CPU2SerialPortMessageManager);
    //DataItem <bool, 1> SinkReConnect = DataItem<bool, 1>("Sink ReConnect", 0, RxTxType_Rx, 1000, m_DataSerializer, m_CPU2SerialPortMessageManager);
    
    //DataItem <DataType_SoundState_t, 1> SoundState  = DataItem<DataType_SoundState_t, 1>("Sound State", LastingSilenceDetected, RxTxType_Rx, 1000, m_DataSerializer, m_CPU1SerialPortMessageManager);
    //DataItem <DataType_ConnectionStatus_t, 1> SourceConnectionStatus = DataItem<DataType_ConnectionStatus_t, 1>("Source Connection Status", Disconnected, RxTxType_Rx, 1000, m_DataSerializer, m_CPU1SerialPortMessageManager);
    //DataItem <DataType_ConnectionStatus_t, 1> SinkConnectionStatus = DataItem<DataType_ConnectionStatus_t, 1>("Sink Connection Status", 0, RxTxType_Rx, 1000, m_DataSerializer, m_CPU1SerialPortMessageManager);
    //DataItem <DataType_SSID_Info_With_LastUpdateTime_t, 1> FoundSpeakerSSIDS = DataItem<DataType_SSID_Info_With_LastUpdateTime_t, 1>("Found Speaker SSIDS", 0, RxTxType_Rx, 1000, m_DataSerializer, m_CPU1SerialPortMessageManager);
    //DataItem <DataType_SSID_Info_t, 1> SinkSSID = DataItem<DataType_SSID_Info_t, 1>("Sink SSID", 0, RxTxType_Rx, 1000, m_DataSerializer, m_CPU1SerialPortMessageManager);
    //DataItem <DataType_SSID_Info_t, 1> TargetSpeakerSSID = DataItem<DataType_SSID_Info_t, 1>("Target Speaker", 0, RxTxType_Rx, 1000, m_DataSerializer, m_CPU2SerialPortMessageManager);
    //DataItem <DataType_SSID_Info_t, 1> SourceSSID = DataItem<DataType_SSID_Info_t, 1>("Source SSID", 0, RxTxType_Rx, 1000, m_DataSerializer, m_CPU1SerialPortMessageManager);
    
    /*
    //Sound State Value and Widget Name Values
    /*
    SoundState_t Sound_State;
    WebSocketDataHandler<SoundState_t> *Sound_State_DataHandler;

    //Amplitude Gain Value and Widget Name Values
    WebSocketDataHandler<float> *Amplitude_Gain_DataHandler;
    
    //FFT Gain Value and Widget Name Values
    WebSocketDataHandler<float> *FFT_Gain_DataHandler;

    //Sink SSID Value and Widget Name Values
    WebSocketSSIDDataHandler *SinkSSID_DataHandler;
    
    //Source SSID Value and Widget Name Values
    WebSocketSSIDDataHandler *SourceSSID_DataHandler;

    //Source SSID Compatible SSIDs
    WebSocketSSIDArrayDataHandler *SourceSSIDs_DataHandler;

    WebSocketDataHandler<ConnectionStatus_t> *Source_Connection_Status_DataHandler;
    WebSocketDataHandler<bool> *Source_BT_Reset_DataHandler;
    WebSocketDataHandler<bool> *Source_BT_ReConnect_DataHandler;
    
    WebSocketDataHandler<bool> *Sink_Connection_Enable_DataHandler;
    WebSocketDataHandler<ConnectionStatus_t> *Sink_Connection_Status_DataHandler;
    WebSocketDataHandler<bool> *Sink_BT_ReConnect_DataHandler;

    //Red Value and Widget Name Values
    WebSocketDataHandler<uint8_t> *Red_Value_DataHandler;
    
    //Blue Value and Widget Name Values
    WebSocketDataHandler<uint8_t> *Blue_Value_DataHandler;
    
    //Red Value and Widget Name Values
    WebSocketDataHandler<uint8_t> *Green_Value_DataHandler;
    
    String Encode_Widget_Values_To_JSON(std::vector<KVP> &KeyValuePairs)
    {
      JSONVar JSONVars;
      for(int i = 0; i < KeyValuePairs.size(); ++i)
      {
        JSONVar SettingValues;
        SettingValues["Id"] = KeyValuePairs[i].Key;
        SettingValues["Value"] = KeyValuePairs[i].Value;
        JSONVars["WidgetValue" + String(i)] = SettingValues; 
      }
      String Result = JSON.stringify(JSONVars);
      return Result;
    }
    
    void NotifyClients(String TextString)
    {
      if(0 < TextString.length())
      {
        m_WebSocket.textAll(TextString.c_str(), TextString.length());
      }
    }
    */

    /*
    void HandleWebSocketMessage(void *arg, uint8_t *data, size_t len)
    {
      AwsFrameInfo *info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
      {
        data[len] = 0;
        String WebSocketData = String((char*)data);
        Serial << "WebSocket Data from Client: " << WebSocketData << "\n";
        JSONVar MyDataObject = JSON.parse(WebSocketData);
        if (JSON.typeof(MyDataObject) == "undefined")
        {
          Serial.println("Parsing Web Socket Data failed!");
          return;
        }
        if( true == MyDataObject.hasOwnProperty("WidgetValue") )
        {
          if( true == MyDataObject["WidgetValue"].hasOwnProperty("Id") && 
              true == MyDataObject["WidgetValue"].hasOwnProperty("Value") )
          {
            const String WidgetId = String( (const char*)MyDataObject["WidgetValue"]["Id"]);
            const String Value = String( (const char*)MyDataObject["WidgetValue"]["Value"]);
            bool WidgetFound = false;
            if(xSemaphoreTake(myReceiverSemaphore, portMAX_DELAY) == pdTRUE)
            {
              for(int i = 0; i < m_MyReceivers.size(); ++i)
              {
                if(true == m_MyReceivers[i]->ProcessWebSocketValueAndSendToDatalink(WidgetId, Value))
                {
                  WidgetFound = true;
                }
              }
              xSemaphoreGive(myReceiverSemaphore);
            }
            
            if(!WidgetFound)
            {
              Serial.println("Unknown Widget: " + WidgetId);
            }
          }
          else
          {
            Serial.println("Misconfigured Widget Value Data: " + MyDataObject["DataValue"]);
          }
        }
        else
        {
          Serial.println("Unsupported Web Socket Data: " + WebSocketData);
        }
      }
    }
    */
    // Initialize WiFi Client
    void InitWiFiClient()
    {
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, password);
      Serial.print("Connecting to WiFi ..");
      while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
      }
      Serial.println(WiFi.localIP());
    }
    
    void InitWiFiAP()
    {
        // Setup ESP32 as Access Point
      IPAddress Ip(192, 168, 0, 1);
      IPAddress NMask(255, 255, 255, 0);
      
      WiFi.softAPConfig(Ip, Ip, NMask);
      WiFi.softAP(ssid, password);
      Serial.println(WiFi.softAPIP());  //Show ESP32 IP on serial
    }
};


#endif
