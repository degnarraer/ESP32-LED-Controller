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
#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "Arduino.h"
#include "HTTP_Method.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "AsyncTCP.h"
#include <Arduino_JSON.h>
#include <DataTypes.h>
#include <Helpers.h>
#include "Streaming.h"

class SettingsWebServerManager: public QueueManager
{
  
  struct JSON_Data_Value
  {
    String Name;
    String Value;
  };

  public:
    SettingsWebServerManager( String Title
                            , AsyncWebSocket &WebSocket )
                            : QueueManager(Title + "Queue Manager", GetDataItemConfigCount())
                            , m_WebSocket(WebSocket)
    {
    
    }
    virtual ~SettingsWebServerManager()
    {
    
    }
    
    void SetupSettingsWebServerManager()
    {
      SetupQueueManager();
      InitWiFiAP();
    }
    
    void ProcessEventQueue()
    {      
      //SOUND STATE TX QUEUE
      if(true == GetValueFromTXQueue(&Sound_State, "Sound State", sizeof(Sound_State), false, false))
      {
        Speaker_Image = String(Sound_State);
        struct JSON_Data_Value Values[1] = { 
                                             { "Speaker_Image", Speaker_Image },
                                           };
        NotifyClients(Encode_JSON_Data_Values_To_JSON(Values, sizeof(Values)/sizeof(Values[0])));
      }
      
      //Amplitude Gain TX QUEUE
      if(true == GetValueFromTXQueue(&Amplitude_Gain, "Amplitude Gain", sizeof(Amplitude_Gain), false, false))
      {
        Amplitude_Gain_Slider = String(Amplitude_Gain);
        struct JSON_Data_Value Values[1] = { 
                                             { "Amplitude_Gain_Slider", Amplitude_Gain_Slider },
                                           };
        NotifyClients(Encode_JSON_Data_Values_To_JSON(Values, sizeof(Values)/sizeof(Values[0])));
      }
      
      //FFT Gain TX QUEUE
      if(true == GetValueFromTXQueue(&Amplitude_Gain, "FFT Gain", sizeof(FFT_Gain), false, false))
      {
        FFT_Gain_Slider = String(FFT_Gain);
        struct JSON_Data_Value Values[1] = { 
                                             { "FFT_Gain_Slider", FFT_Gain_Slider },
                                           };
        NotifyClients(Encode_JSON_Data_Values_To_JSON(Values, sizeof(Values)/sizeof(Values[0])));
      }
    }
    
    void OnEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
    {
      switch (type) {
        case WS_EVT_CONNECT:
          Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
          break;
        case WS_EVT_DISCONNECT:
          Serial.printf("WebSocket client #%u disconnected\n", client->id());
          break;
        case WS_EVT_DATA:
          HandleWebSocketMessage(arg, data, len);
          break;
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
          break;
      }
    }
    
  private:
    AsyncWebSocket &m_WebSocket; 
    
    // Replace with your network credentials
    const char* ssid = "LED Tower of Power";
    const char* password = "LEDs Rock";

    String message = "";

    //Amplitude Gain Value and Widget Name Values
    float Amplitude_Gain;
    String Amplitude_Gain_Slider = "1.0";

    //FFT Gain Value and Widget Name Values
    float FFT_Gain;
    String FFT_Gain_Slider = "1.0";

    //Sound State Value and Widget Name Values
    SoundState_t Sound_State;
    String Speaker_Image = "0";

    uint32_t Red_Value;
    String Red_Value_Slider = "0";
    
    uint32_t Blue_Value;
    String Blue_Value_Slider = "0";
    
    uint32_t Green_Value;
    String Green_Value_Slider = "0";
  

    //QueueManager Interface
    static const size_t m_WebServerConfigCount = 4;
    DataItemConfig_t m_ItemConfig[m_WebServerConfigCount]
    {
      { "Source Is Connected",      DataType_bool_t,        1,    Transciever_TX,     20 },
      { "Sound State",              DataType_SoundState_t,  1,    Transciever_TX,     20 },
      { "Amplitude Gain",           DataType_Float_t,       1,    Transciever_TXRX,   20 },
      { "FFT Gain",                 DataType_Float_t,       1,    Transciever_TXRX,   20 },
    };
    DataItemConfig_t* GetDataItemConfig() { return m_ItemConfig; }
    size_t GetDataItemConfigCount() { return m_WebServerConfigCount; }
    
    //Get Slider Values
    String Encode_JSON_Data_Values_To_JSON(struct JSON_Data_Value *DataValues, size_t Count)
    {
      JSONVar JSONVars;
      for(int i = 0; i < Count; ++i)
      {
          JSONVar SettingValues;
          SettingValues["Name"] = DataValues[i].Name;
          SettingValues["Value"] = DataValues[i].Value;
          JSONVars["DataValue" + String(i)] = SettingValues;
      }
      return JSON.stringify(JSONVars);
    }
    
    void NotifyClients(String TextString)
    {
      m_WebSocket.textAll(TextString);
    }
    
    void HandleWebSocketMessage(void *arg, uint8_t *data, size_t len)
    {
      AwsFrameInfo *info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
      {
        data[len] = 0;
        String message = String((char*)data);
        Serial.println(message);
        if (true == message.equals("Get All Values"))
        {
          Serial.println("Sending All Value");
          struct JSON_Data_Value Values[6] = { 
                                               { "Amplitude_Gain_Slider", Amplitude_Gain_Slider },
                                               { "FFT_Gain_Slider", FFT_Gain_Slider },
                                               { "Red_Value_Slider", Red_Value_Slider },
                                               { "Green_Value_Slider", Green_Value_Slider },
                                               { "Blue_Value_Slider", Blue_Value_Slider },
                                               { "Sound_State", Speaker_Image },
                                             };
          NotifyClients(Encode_JSON_Data_Values_To_JSON(Values, sizeof(Values)/sizeof(Values[0])));
        }
        else
        {
          JSONVar MyObject = JSON.parse(message);
          if (JSON.typeof(MyObject) == "undefined") {
            Serial.println("Parsing input failed!");
            return;
          }
          if (MyObject.hasOwnProperty("Name") && MyObject.hasOwnProperty("Value"))
          {
            String Name = String( (const char*)MyObject["Name"]);
            String Value = String( (const char*)MyObject["Value"]);
            
            if(Name.equals("Amplitude_Gain_Slider"))
            {
              Amplitude_Gain_Slider = Value;
              Amplitude_Gain = Amplitude_Gain_Slider.toFloat();
              PushValueToRXQueue(&Amplitude_Gain, "Amplitude Gain", false);
            }
            else if(Name.equals("FFT_Gain_Slider"))
            {
              FFT_Gain_Slider = Value;
              FFT_Gain = FFT_Gain_Slider.toFloat();
              PushValueToRXQueue(&FFT_Gain, "FFT Gain", false);
            }
            else if(Name.equals("Red_Value_Slider"))
            {
              Red_Value_Slider = Value;
              Red_Value = Red_Value_Slider.toInt();
              PushValueToRXQueue(&FFT_Gain, "FFT Gain", false);
            }
            else if(Name.equals("Green_Value_Slider"))
            {
              Green_Value_Slider = Value;
              Green_Value = Green_Value_Slider.toInt();
              PushValueToRXQueue(&FFT_Gain, "FFT Gain", false);
            }
            else if(Name.equals("Blue_Value_Slider"))
            {
              Blue_Value_Slider = Value;
              Blue_Value = Blue_Value_Slider.toInt();
              PushValueToRXQueue(&FFT_Gain, "FFT Gain", false);
            }
            else
            {
              Serial.println("Failed to Parse " + Name);
            }
          }
        }
      }
    }
    
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