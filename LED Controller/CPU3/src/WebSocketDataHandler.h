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

#include <freertos/portmacro.h>
#include "Streaming.h"
#include "Helpers.h"
#include "DataTypes.h"
#include "Tunes.h"
#include "DataItem/DataItems.h"
#include "ESPAsyncWebServer.h"
#include "AsyncTCP.h"
#include "Arduino_JSON.h"
#include <mutex>
#include <cstring>

#define MESSAGE_LENGTH 500

class SettingsWebServerManager;

class WebSocketDataHandlerSender
{
  public:
    virtual void HandleWebSocketTxNotification() = 0;
};


class WebSocketDataHandlerReceiver
{
  public:
    virtual void HandleWebSocketRxNotification(const String& stringValue) = 0;
    virtual String GetSignal() = 0;
};

class WebSocketDataProcessor
{
  public:
    WebSocketDataProcessor( AsyncWebServer &webServer
                          , AsyncWebSocket &webSocket )
                          : m_WebServer(webServer)
                          , m_WebSocket(webSocket)
    {
      xTaskCreatePinnedToCore( StaticWebSocketDataProcessor_Task,  "WebServer_Task",   10000,  this,  THREAD_PRIORITY_MEDIUM,    &m_WebSocketTaskHandle,    0 );
    }
    virtual ~WebSocketDataProcessor()
    {
      std::lock_guard<std::recursive_mutex> lock(m_Tx_KeyValues_Mutex);
      if(m_WebSocketTaskHandle) vTaskDelete(m_WebSocketTaskHandle);
    }
    void DeRegisterForWebSocketRxNotification(const String& name, WebSocketDataHandlerReceiver *aReceiver);
    void DeDeRegisterForWebSocketRxNotification(const String& name, WebSocketDataHandlerReceiver *aReceiver);
    void RegisterForWebSocketTxNotification(const String& name, WebSocketDataHandlerSender *aSender);
    void DeRegisterForWebSocketTxNotification(const String& name, WebSocketDataHandlerSender *aSender);
    bool ProcessSignalValueAndSendToDatalink(const String& signalId, const String& value);
    void UpdateAllDataToClient(uint8_t clientId);
    //DELETE ME void UpdateDataForSender(WebSocketDataHandlerSender* sender, bool forceUpdate);
    static void StaticWebSocketDataProcessor_Task(void * parameter);
    void TxDataToWebSocket(String key, String value)
    {
      std::lock_guard<std::recursive_mutex> lock(m_Tx_KeyValues_Mutex);
      KVP keyValuePair = {key, value};
      m_Tx_KeyValues.push_back(keyValuePair);
    }
  private:
    AsyncWebServer &m_WebServer;
    AsyncWebSocket &m_WebSocket;
    TaskHandle_t m_WebSocketTaskHandle;
    std::vector<WebSocketDataHandlerReceiver*> m_MyRxNotifyees = std::vector<WebSocketDataHandlerReceiver*>();
    std::vector<WebSocketDataHandlerSender*> m_MyTxNotifyees = std::vector<WebSocketDataHandlerSender*>();
    std::vector<KVP> m_Tx_KeyValues = std::vector<KVP>();
    std::recursive_mutex m_Tx_KeyValues_Mutex;
    void WebSocketDataProcessor_Task();
    void Encode_Signal_Values_To_JSON(std::vector<KVP> &signalValue, String &result);
    void NotifyClient(uint8_t clientID, const String& textString);
    void NotifyClients(const String& textString);

    template<typename T>
    std::vector<T>* AllocateVectorOnHeap(size_t count)
    {
        T* data = static_cast<T*>(heap_caps_malloc(sizeof(T) * count, MALLOC_CAP_SPIRAM));
        if (data)
        {
            return new std::vector<T>(data, data + count);
        }
        return nullptr;
    }
};

template<typename T, size_t COUNT>
class WebSocketDataHandler: public WebSocketDataHandlerReceiver
                          , public WebSocketDataHandlerSender
                          , public NewRxTxValueCalleeInterface<T>
                          , public DataTypeFunctions
{
  public:
    WebSocketDataHandler()
    {
    }
    WebSocketDataHandler( const WebSocketDataHandler &t )
                        : m_WebSocketDataProcessor(t.m_WebSocketDataProcessor)
                        , m_RequiresWebSocketRxNotification(t.m_RequiresWebSocketRxNotification)
                        , m_RequiresWebSocketTxNotification(t.m_RequiresWebSocketTxNotification)
                        , m_DataItem(t.m_DataItem)
                        , m_Debug(t.m_Debug)
                        , m_Name(t.m_Name)
                        , m_Signal(t.m_Signal)
    {
      if(m_RequiresWebSocketRxNotification) m_WebSocketDataProcessor.DeRegisterForWebSocketRxNotification(m_Name, this);
      if(m_RequiresWebSocketTxNotification)
      {
        m_WebSocketDataProcessor.RegisterForWebSocketTxNotification(m_Name, this);
        m_DataItem.RegisterForNewRxValueNotification(this);
      }
    }
    WebSocketDataHandler( WebSocketDataProcessor &webSocketDataProcessor
                        , const bool &isReceiver
                        , const bool &isSender
                        , LocalDataItem<T, COUNT> &dataItem
                        , const bool &debug )
                        : m_WebSocketDataProcessor(webSocketDataProcessor)
                        , m_RequiresWebSocketRxNotification(isReceiver)
                        , m_RequiresWebSocketTxNotification(isSender)
                        , m_DataItem(dataItem)
                        , m_Debug(debug)
                        , m_Name(dataItem.GetName() + " Web Socket")
                        , m_Signal(dataItem.GetName())
    {
      if(m_RequiresWebSocketRxNotification) m_WebSocketDataProcessor.DeRegisterForWebSocketRxNotification(m_Name, this);
      if(m_RequiresWebSocketTxNotification)
      {
        m_WebSocketDataProcessor.RegisterForWebSocketTxNotification(m_Name, this);
        m_DataItem.RegisterForNewRxValueNotification(this);
      }
    }
    
    virtual ~WebSocketDataHandler()
    {
      if(m_RequiresWebSocketRxNotification) m_WebSocketDataProcessor.DeDeRegisterForWebSocketRxNotification(m_Name, this);
      if(m_RequiresWebSocketTxNotification)
      {
        m_WebSocketDataProcessor.DeRegisterForWebSocketTxNotification(m_Name, this);
        m_DataItem.DeRegisterForNewRxValueNotification(this);
      }
    }
    
    bool NewRxValueReceived(const NewRxValue_Caller_Interface<T>* sender, const T* values, size_t count) override
    {
      if(sender == &m_DataItem)
      {
        ESP_LOGI( "NewRxValueReceived", "New RX Datalink Value");
        m_WebSocketDataProcessor.TxDataToWebSocket(m_Signal, m_DataItem.GetValueAsString());
        m_Last_Update_Time = millis();
        return true;
      }
      else
      {
        ESP_LOGW( "NewRxValueReceived", "WARNING! Rx value from unknown sender");
        return false;
      }
    }
    
    String GetSignal()
    {
      return m_Signal;
    }

    virtual String GetName() const
    {
      return m_Name;
    }

    
    virtual void HandleWebSocketTxNotification() override
    {
      m_WebSocketDataProcessor.TxDataToWebSocket(m_Signal, m_DataItem.GetValueAsString());
    }
    
    virtual void HandleWebSocketRxNotification(const String& stringValue) override
    {
      ESP_LOGI( "WebSocketDataHandler: HandleWebSocketRxNotification"
              , "\"%s\" WebSocket Rx Signal: \"%s\" Value: \"%s\""
              , m_Name.c_str()
              , m_Signal.c_str()
              , stringValue.c_str());
      m_DataItem.SetValueFromString(stringValue);
    }

    void TxDataToWebSocket(String key, String value)
    {
      m_WebSocketDataProcessor.TxDataToWebSocket(key, value);
    }
  protected:
    WebSocketDataProcessor &m_WebSocketDataProcessor;
    const bool &m_RequiresWebSocketRxNotification;
    const bool &m_RequiresWebSocketTxNotification;
    LocalDataItem<T, COUNT> &m_DataItem;
    const bool &m_Debug;
    const String m_Name;
    const String m_Signal;
    size_t m_ChangeCount = 0;

    bool ValueChanged()
    {
      size_t newChangeCount = m_DataItem.GetChangeCount();
      bool valueChanged = newChangeCount != m_ChangeCount;
      m_ChangeCount = newChangeCount;
      return valueChanged;
    }
  private:
    uint64_t m_Last_Update_Time = millis();    
};

class WebSocket_String_DataHandler: public WebSocketDataHandler<char, DATAITEM_STRING_LENGTH>
{
  public:
    WebSocket_String_DataHandler( WebSocketDataProcessor &WebSocketDataProcessor
                                , const bool &IsReceiver
                                , const bool &IsSender
                                , LocalDataItem<char, DATAITEM_STRING_LENGTH> &DataItem
                                , const bool Debug )
                                : WebSocketDataHandler<char, DATAITEM_STRING_LENGTH>( WebSocketDataProcessor
                                                                                    , IsReceiver
                                                                                    , IsSender
                                                                                    , DataItem
                                                                                    , Debug)
    {
    }
    
    virtual ~WebSocket_String_DataHandler()
    {
    }
};

class WebSocket_Compatible_Device_DataHandler: public WebSocketDataHandler<CompatibleDevice_t, 1>
{
  public:
    WebSocket_Compatible_Device_DataHandler( WebSocketDataProcessor &WebSocketDataProcessor
                                           , const bool &IsReceiver
                                           , const bool &IsSender
                                           , DataItem<CompatibleDevice_t, 1> &DataItem
               
                                           , const bool Debug )
                                           : WebSocketDataHandler<CompatibleDevice_t, 1>( WebSocketDataProcessor
                                                                                        , IsReceiver
                                                                                        , IsSender
                                                                                        , DataItem
                                                                                        , Debug)
    {
    }
    
    virtual ~WebSocket_Compatible_Device_DataHandler()
    {
    }
  protected:
  
  /*//DELETE ME 
    virtual void HandleWebSocketTxNotification(std::vector<KVP> &signalValues, bool forceUpdate = false) override
    {
      if( forceUpdate || ValueChanged() )
      {
        KVP signalValue;
        KVP compatibleDevice;
        CompatibleDevice_t currentValue;
        m_DataItem.GetValue(&currentValue, 1);
        std::vector<KVP> compatibleDevices;
        compatibleDevice.Key = String(currentValue.address);
        compatibleDevice.Value = String(currentValue.name);
        compatibleDevices.push_back(compatibleDevice);
        signalValue.Key = m_Signal;
        signalValue.Value = Encode_Compatible_Device_To_JSON(compatibleDevices);
        if(signalValue.Value.length())
        {
          ESP_LOGI( "HandleWebSocketTxNotification", "\"%s\": Tx: \"%s\"", m_Name.c_str(), signalValue.Value.c_str());
          signalValues.push_back(signalValue);
        }
      }
    }
    void HandleWebSocketRxNotification(const String& stringValue) override
    {
      ESP_LOGI("WebSocket_Compatible_Device_DataHandler: HandleWebSocketRxNotification", "New Signal Value for \"%s\": \"%s\"", m_Signal.c_str(), stringValue.c_str());
      JSONVar jSONObject = JSON.parse(stringValue);
      if (JSON.typeof(jSONObject) == "undefined")
      {
        ESP_LOGE("WebSocket_Compatible_Device_DataHandler: HandleWebSocketRxNotification", "ERROR! unable to parse JSON.");
      }
      String name = jSONObject["NAME"];
      String address = jSONObject["ADDRESS"];
      CompatibleDevice_t newValue(name.c_str(), address.c_str());
      m_DataItem.SetValue(&newValue, 1);
    }
    */
  private:
    String Encode_Compatible_Device_To_JSON(std::vector<KVP> &keyValuePair)
    {
      JSONVar jSONVars;
      for(int i = 0; i < keyValuePair.size(); ++i)
      { 
        JSONVar compatibleDeviceValues;
        compatibleDeviceValues["ADDRESS"] = keyValuePair[i].Key;
        compatibleDeviceValues["NAME"] = keyValuePair[i].Value;
        jSONVars["CompatibleDevice" + String(i)] = compatibleDeviceValues;
      }
      return JSON.stringify(jSONVars);
    }
};

class WebSocket_ActiveCompatibleDevice_ArrayDataHandler: public WebSocketDataHandler<ActiveCompatibleDevice_t, 1>
{
  public:
    WebSocket_ActiveCompatibleDevice_ArrayDataHandler( WebSocketDataProcessor &WebSocketDataProcessor
                                                     , const bool &IsReceiver
                                                     , const bool &IsSender
                                                     , DataItem<ActiveCompatibleDevice_t, 1> &DataItem
                                                     , const bool Debug )
                                                     : WebSocketDataHandler<ActiveCompatibleDevice_t, 1>( WebSocketDataProcessor
                                                                                                        , IsReceiver
                                                                                                        , IsSender
                                                                                                        , DataItem
                                                                                                        , Debug)
    {
    }
    
    virtual ~WebSocket_ActiveCompatibleDevice_ArrayDataHandler()
    {
    }

  protected:
  /*//DELETE ME 
    virtual void HandleWebSocketTxNotification(std::vector<KVP> &signalValues, bool forceUpdate = false) override
    { 
      bool valueChanged = ValueChanged();
      unsigned long elapsedTime = 0;
      unsigned long currentMillis = millis();
      bool updated = false;
      if(valueChanged)
      {
        ActiveCompatibleDevice_t activeCompatibleDevice;
        m_DataItem.GetValue(&activeCompatibleDevice, 1);
        for(size_t i = 0; i < m_ActiveCompatibleDevices.size(); ++i)
        {
          // Check if the address matches
          if(strcmp(m_ActiveCompatibleDevices[i].address, activeCompatibleDevice.address) == 0)
          {
            // Update device details
            strcpy(m_ActiveCompatibleDevices[i].name, activeCompatibleDevice.name);
            m_ActiveCompatibleDevices[i].rssi = activeCompatibleDevice.rssi;
            m_ActiveCompatibleDevices[i].timeSinceUpdate = activeCompatibleDevice.timeSinceUpdate;
            m_ActiveCompatibleDevices[i].lastUpdateTime = currentMillis;
            updated = true;
            LogDetails(m_ActiveCompatibleDevices[i], "Updated Device");
          }
        }
        // Add new device if not found and conditions met
        if( !updated && ACTIVE_NAME_TIMEOUT >= activeCompatibleDevice.timeSinceUpdate + ACTIVE_NAME_TIMEOUT/2 )
        {
          // Create new device and add to the vector
          ActiveCompatibleDevice_t NewDevice = ActiveCompatibleDevice_t(activeCompatibleDevice.name,
                                                                        activeCompatibleDevice.address,
                                                                        activeCompatibleDevice.rssi,
                                                                        currentMillis,
                                                                        activeCompatibleDevice.timeSinceUpdate);
          m_ActiveCompatibleDevices.push_back(NewDevice);
          updated = true;
          LogDetails(activeCompatibleDevice, "Added Device");
        }
      }
      //Updated Elapsed Time Since Update for all Active Devices
      for(size_t i = 0; i < m_ActiveCompatibleDevices.size(); ++i)
      {
        // Get previousMillis for comparison
        unsigned long previousMillis = m_ActiveCompatibleDevices[i].lastUpdateTime;
        // Calculate elapsed time
        if (currentMillis >= previousMillis) {
            elapsedTime = currentMillis - previousMillis;
        } 
        else {
            elapsedTime = (ULONG_MAX - previousMillis) + currentMillis + 1;
        }
        m_ActiveCompatibleDevices[i].timeSinceUpdate += elapsedTime;
      
        // Remove timed out device
        if(ACTIVE_NAME_TIMEOUT < m_ActiveCompatibleDevices[i].timeSinceUpdate)
        {
            ESP_LOGI("HandleWebSocketTxNotification", "Name Timedout: %s", m_ActiveCompatibleDevices[i].name);
            m_ActiveCompatibleDevices.erase(m_ActiveCompatibleDevices.begin() + i);
            --i;
            updated = true;
        }
      }
      
      // Update signalValues if forceUpdate or changes detected
      if(forceUpdate || updated)
      {
        if(m_ActiveCompatibleDevices.size())
        {
          KVP signalValue;
          std::vector<KVT> activeCompatibleDevicesKVT;
          for(size_t i = 0; i < m_ActiveCompatibleDevices.size(); ++i)
          {
              KVT activeCompatibleDeviceKVT;
              activeCompatibleDeviceKVT.Key = String(m_ActiveCompatibleDevices[i].address);
              activeCompatibleDeviceKVT.Value1 = String(m_ActiveCompatibleDevices[i].name);
              activeCompatibleDeviceKVT.Value2 = String(m_ActiveCompatibleDevices[i].rssi).c_str();
              activeCompatibleDevicesKVT.push_back(activeCompatibleDeviceKVT);
          }
          signalValue.Key = m_Signal;
          signalValue.Value = Encode_SSID_Values_To_JSON(activeCompatibleDevicesKVT);
          ESP_LOGI("HandleWebSocketTxNotification", "Encoding Result: \"%s\"", signalValue.Value.c_str());
          if(!signalValue.Value.isEmpty())
          {
              signalValues.push_back(signalValue);
          }
        }
      }
    }
    */
    virtual void HandleWebSocketRxNotification(const String& stringValue) override
    {
      ESP_LOGE("HandleWebSocketRxNotification", "ERROR! This function is not supported yet.");
    }
  private:
    //Datalink
    std::vector<ActiveCompatibleDevice_t> m_ActiveCompatibleDevices;
    String Encode_SSID_Values_To_JSON(std::vector<KVT> &ssidKVT)
    {
      JSONVar JSONVars;
      for(int i = 0; i < ssidKVT.size(); ++i)
      { 
        JSONVar compatibleDeviceValues;
        compatibleDeviceValues["ADDRESS"] = ssidKVT[i].Key;
        compatibleDeviceValues["NAME"] = ssidKVT[i].Value1;
        compatibleDeviceValues["RSSI"] = ssidKVT[i].Value2;
        JSONVars["ActiveCompatibleDevice" + String(i)] = compatibleDeviceValues;
      }
      return JSON.stringify(JSONVars);
    }
    void LogDetails(ActiveCompatibleDevice_t &device, const String &context)
    {
      // Log device details
      ESP_LOGI( "HandleWebSocketTxNotification",
                "%s: %s: \n************* \nDevice Name: %s \nAddress: %s \nRSSI: %i \nUpdate Time: %lu \nTime Since Update: %lu \nChange Count: %i",
                context.c_str(),
                this->m_Name.c_str(),
                device.name,
                device.address,
                device.rssi,
                device.lastUpdateTime,
                device.timeSinceUpdate,
                m_ChangeCount );
    }
};
