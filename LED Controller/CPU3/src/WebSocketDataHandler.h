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
#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include "Streaming.h"
#include "Helpers.h"
#include "DataTypes.h"
#include "Tunes.h"
#include "DataItem/DataItems.h"
#include "Arduino_JSON.h"
#include <mutex>
#include <cstring>

#define MESSAGE_LENGTH 500
#define BLUETOOTH_DEVICE_TIMEOUT 10000

class SettingsWebServerManager;

class WebSocketDataHandlerSender
{
  public:
    virtual void HandleWebSocketDataRequest() = 0;
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
    WebSocketDataProcessor( WebServer &webServer
                          , WebSocketsServer &webSocket )
                          : m_WebServer(webServer)
                          , m_WebSocket(webSocket)
    {
      xTaskCreatePinnedToCore( StaticWebSocketDataProcessor_WebSocket_TxTask,  "WebServer_Task",   10000,  this,  THREAD_PRIORITY_MEDIUM,    &m_WebSocketTaskHandle,    0 );
    }
    virtual ~WebSocketDataProcessor()
    {
      std::lock_guard<std::recursive_mutex> lock(m_Tx_KeyValues_Mutex);
      if(m_WebSocketTaskHandle) vTaskDelete(m_WebSocketTaskHandle);
    }
    void RegisterForWebSocketRxNotification(const String& name, WebSocketDataHandlerReceiver *aReceiver);
    void DeRegisterForWebSocketRxNotification(const String& name, WebSocketDataHandlerReceiver *aReceiver);
    void RegisterForWebSocketTxNotification(const String& name, WebSocketDataHandlerSender *aSender);
    void DeRegisterForWebSocketTxNotification(const String& name, WebSocketDataHandlerSender *aSender);
    bool ProcessSignalValueAndSendToDatalink(const String& signalId, const String& value);
    void UpdateAllDataToClient(uint8_t clientId);
    static void StaticWebSocketDataProcessor_WebSocket_TxTask(void * parameter);
    void TxDataToWebSocket(String key, String value)
    {
      std::lock_guard<std::recursive_mutex> lock(m_Tx_KeyValues_Mutex);
      KVP keyValuePair = {key, value};
      m_Tx_KeyValues.push_back(keyValuePair);
    }
  private:
    WebServer &m_WebServer;
    WebSocketsServer &m_WebSocket;
    TaskHandle_t m_WebSocketTaskHandle;
    std::vector<WebSocketDataHandlerReceiver*> m_MyRxNotifyees = std::vector<WebSocketDataHandlerReceiver*>();
    std::vector<WebSocketDataHandlerSender*> m_MyTxNotifyees = std::vector<WebSocketDataHandlerSender*>();
    std::vector<KVP> m_Tx_KeyValues = std::vector<KVP>();
    std::recursive_mutex m_Tx_KeyValues_Mutex;
    void WebSocketDataProcessor_WebSocket_TxTask();
    void Encode_Signal_Values_To_JSON(std::vector<KVP> &signalValue, String &result);
    void NotifyClient(uint8_t clientID, const String& textString);
    void NotifyClients(const String& textString);

    template<typename T>
    std::vector<T>* AllocateVectorOnHeap(size_t count)
    {
        T* data = static_cast<T*>(malloc(sizeof(T) * count));
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
                          , public Rx_Value_Callee_Interface<T>
                          , public DataTypeFunctions
{
  public:
    WebSocketDataHandler()
    {
    }
    WebSocketDataHandler( const WebSocketDataHandler &t )
                        : m_WebSocketDataProcessor(t.m_WebSocketDataProcessor)
                        , m_DataItem(t.m_DataItem)
                        , m_Name(t.m_Name)
                        , m_Signal(t.m_Signal)
    {
      m_WebSocketDataProcessor.RegisterForWebSocketRxNotification(m_Name, this);
      m_WebSocketDataProcessor.RegisterForWebSocketTxNotification(m_Name, this);
      auto dataItemPtr = dynamic_cast<DataItem<T,COUNT>*>(&m_DataItem);
      if (dataItemPtr)
      {
        dataItemPtr->RegisterForNewRxValueNotification(this);
      }
    }
    WebSocketDataHandler( WebSocketDataProcessor &webSocketDataProcessor
                        , LocalDataItem<T, COUNT> &dataItem )
                        : m_WebSocketDataProcessor(webSocketDataProcessor)
                        , m_DataItem(dataItem)
                        , m_Name(dataItem.GetName() + " Web Socket")
                        , m_Signal(dataItem.GetName())
    {
      m_WebSocketDataProcessor.RegisterForWebSocketRxNotification(m_Name, this);
      m_WebSocketDataProcessor.RegisterForWebSocketTxNotification(m_Name, this);
      auto dataItemPtr = dynamic_cast<DataItem<T,COUNT>*>(&m_DataItem);
      if (dataItemPtr)
      {
        dataItemPtr->RegisterForNewRxValueNotification(this);
      }
    }
    
    virtual ~WebSocketDataHandler()
    {
      m_WebSocketDataProcessor.DeRegisterForWebSocketRxNotification(m_Name, this);
      m_WebSocketDataProcessor.DeRegisterForWebSocketTxNotification(m_Name, this);
      auto dataItemPtr = dynamic_cast<DataItem<T,COUNT>*>(&m_DataItem);
      if (dataItemPtr)
      {
        dataItemPtr->DeRegisterForNewRxValueNotification(this);
      }
    }

    virtual bool NewRxValueReceived(const T* values, size_t changeCount) override
    {
      ESP_LOGD( "NewRxValueReceived", "New DataItem Rx Value");
      bool success = false;
      if(IsChangeCountGreater(m_DataItem.GetChangeCount()))
      {
        m_WebSocketDataProcessor.TxDataToWebSocket(m_Signal, m_DataItem.GetValueAsString());
        success = true;
        ESP_LOGI( "NewRxValueReceived", "\"%s\": New Verified DataItem Rx Value: Sent to Web Socket", m_Signal.c_str());
      }
      return success;
    }
    
    String GetSignal()
    {
      return m_Signal;
    }

    virtual String GetName() const
    {
      return m_Name;
    }

    
    virtual void HandleWebSocketDataRequest() override
    {
        m_WebSocketDataProcessor.TxDataToWebSocket(m_Signal, m_DataItem.GetValueAsString());
        m_ChangeCount = m_DataItem.GetChangeCount();
    }
    
    virtual void HandleWebSocketRxNotification(const String& stringValue) override
    {
      ESP_LOGI( "Web Socket Rx"
              , "\"%s\" WebSocket Rx Signal: \"%s\" Value: \"%s\""
              , m_Name.c_str()
              , m_Signal.c_str()
              , stringValue.c_str());
      m_DataItem.SetValueFromString(stringValue);
      m_ChangeCount = m_DataItem.GetChangeCount();
    }

    void TxDataToWebSocket(String key, String value)
    {
      m_WebSocketDataProcessor.TxDataToWebSocket(key, value);
      m_ChangeCount = m_DataItem.GetChangeCount();
    }

  protected:
    WebSocketDataProcessor &m_WebSocketDataProcessor;
    LocalDataItem<T, COUNT> &m_DataItem;
    size_t m_ChangeCount;
    const String m_Name;
    const String m_Signal;
		bool IsChangeCountGreater(size_t changeCount)
		{
			return (changeCount > m_ChangeCount) || (m_ChangeCount - changeCount > (SIZE_MAX / 2));
		}
};

class WebSocket_String_DataHandler: public WebSocketDataHandler<char, DATAITEM_STRING_LENGTH>
{
  public:
    WebSocket_String_DataHandler( WebSocketDataProcessor &WebSocketDataProcessor
                                , LocalDataItem<char, DATAITEM_STRING_LENGTH> &DataItem )
                                : WebSocketDataHandler<char, DATAITEM_STRING_LENGTH>( WebSocketDataProcessor
                                                                                    , DataItem )
    {
    }
    
    virtual ~WebSocket_String_DataHandler()
    {
    }
};


class BT_Device_Info_With_Time_Since_Update_WebSocket_DataHandler: public WebSocketDataHandler<BT_Device_Info_With_Time_Since_Update, 1>
{
  public:
    BT_Device_Info_With_Time_Since_Update_WebSocket_DataHandler( WebSocketDataProcessor &WebSocketDataProcessor
                                                               , LocalDataItem<BT_Device_Info_With_Time_Since_Update, 1> &DataItem )
                                                               : WebSocketDataHandler<BT_Device_Info_With_Time_Since_Update, 1>( WebSocketDataProcessor
                                                                                                                               , DataItem )
    {
      m_ActiveDevicesMutex = xSemaphoreCreateRecursiveMutex();
      CreateTasks();
    }
    
    virtual ~BT_Device_Info_With_Time_Since_Update_WebSocket_DataHandler()
    {
      DestroyTasks();
    }
    
    void CreateTasks()
    {
      if( xTaskCreatePinnedToCore( Static_UpdateActiveCompatibleDevices, "Update Active Devices", 5000,  this, THREAD_PRIORITY_MEDIUM, &m_ActiveDeviceUpdateTask, 0 ) != pdTRUE )
      {
        ESP_LOGE("CreateTasks", "ERROR! Unable to create task.");
      }
    }

    void DestroyTasks()
    {
      if(m_ActiveDeviceUpdateTask) vTaskDelete(m_ActiveDeviceUpdateTask);
    }

    bool NewRxValueReceived(const BT_Device_Info_With_Time_Since_Update* values, size_t changeCount) override
    {
      ESP_LOGI( "NewRxValueReceived", "New DataItem Rx Value");
      bool success = false;
      if(this->IsChangeCountGreater(m_DataItem.GetChangeCount()))
      {
        ActiveCompatibleDeviceReceived(values[0]);
        SendActiveCompatibleDevicesToWebSocket();
        success = true;
      }
      m_ChangeCount = m_DataItem.GetChangeCount();
      return success;
    }

    void ActiveCompatibleDeviceReceived(BT_Device_Info_With_Time_Since_Update device)
    {
      if (xSemaphoreTakeRecursive(m_ActiveDevicesMutex, portMAX_DELAY))
      {
        auto it = std::find(m_ActiveDevices.begin(), m_ActiveDevices.end(), device);
        if (it != m_ActiveDevices.end())
        {
          *it = device;
          ESP_LOGI("ScannedDevice_ValueChanged", "Existing Scanned Device Update: \"%s\"", device.toString().c_str());
        }
        else 
        {
          if(device.timeSinceUpdate < BLUETOOTH_DEVICE_TIMEOUT)
          {
            ESP_LOGI("ScannedDevice_ValueChanged", "New Scanned Device: \"%s\"", device.toString().c_str());
            m_ActiveDevices.push_back(device);
            SendActiveCompatibleDevicesToWebSocket();
          }
        }
      }
      xSemaphoreGiveRecursive(m_ActiveDevicesMutex);
    }

    static void Static_UpdateActiveCompatibleDevices(void * parameter)
    {
      BT_Device_Info_With_Time_Since_Update_WebSocket_DataHandler *handler = (BT_Device_Info_With_Time_Since_Update_WebSocket_DataHandler*)parameter;
      const TickType_t xFrequency = 1000;
      TickType_t xLastWakeTime = xTaskGetTickCount();
      while(true)
      {
        vTaskDelayUntil( &xLastWakeTime, xFrequency );
        handler->CleanActiveCompatibleDevices();
      }
    }

    void CleanActiveCompatibleDevices()
    {
        ESP_LOGV("UpdateActiveCompatibleDevices", "Cleaning Stale Devices.");
        if (xSemaphoreTakeRecursive(m_ActiveDevicesMutex, portMAX_DELAY) == pdTRUE)
        {
            for (auto it = m_ActiveDevices.begin(); it != m_ActiveDevices.end();)
            {
                BT_Device_Info_With_Time_Since_Update* device = static_cast<BT_Device_Info_With_Time_Since_Update*>(&(*it));
                if (device->timeSinceUpdate > BLUETOOTH_DEVICE_TIMEOUT)
                {
                    ESP_LOGI("UpdateActiveCompatibleDevices", "Removing Device: \"%s\"", device->toString().c_str());
                    it = m_ActiveDevices.erase(it);
                    SendActiveCompatibleDevicesToWebSocket();
                }
                else
                {
                    ++it;
                }
            }
            xSemaphoreGiveRecursive(m_ActiveDevicesMutex);
        }
        else
        {
            ESP_LOGE("UpdateActiveCompatibleDevices", "Failed to take mutex.");
        }
    }

    void SendActiveCompatibleDevicesToWebSocket()
    {
      if (xSemaphoreTakeRecursive(m_ActiveDevicesMutex, portMAX_DELAY))
      {
        String value;
        for (auto it = m_ActiveDevices.begin(); it != m_ActiveDevices.end();)
        {
            BT_Device_Info_With_Time_Since_Update* device = static_cast<BT_Device_Info_With_Time_Since_Update*>(&(*it));
            value += "{" + device->toString() + "}";
        }
        ESP_LOGI("SendActiveCompatibleDevicesToWebSocket", "JSON: %s", value.c_str());
        String key = this->GetName();
        this->TxDataToWebSocket(key, value);
      }
      xSemaphoreGiveRecursive(m_ActiveDevicesMutex);
    }
    private:
      std::vector<BT_Device_Info_With_Time_Since_Update> m_ActiveDevices;
      SemaphoreHandle_t m_ActiveDevicesMutex;
      TaskHandle_t m_ActiveDeviceUpdateTask;
};
