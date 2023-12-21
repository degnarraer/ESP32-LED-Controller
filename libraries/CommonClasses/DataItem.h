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

#include <vector>
#include "DataSerializer.h"

class NewValueCallBack
{
	public:
		NewValueCallBack()
		{
			
		}
		virtual ~NewValueCallBack()
		{
			
		}
		virtual void NewValueReceived() = 0;
};

class SerialPortMessageManager
{
	public:
		SerialPortMessageManager(String taskName, HardwareSerial &serial)
						 : m_Serial(serial)
						 , m_TaskName(taskName)
		{
			xTaskCreatePinnedToCore( StaticSerialPortMessageManager_RXLoop, m_TaskName.c_str(), 5000, this,  configMAX_PRIORITIES - 1,  &m_RXTaskHandle,  0 );
			xTaskCreatePinnedToCore( StaticSerialPortMessageManager_TXLoop, m_TaskName.c_str(), 5000, this,  configMAX_PRIORITIES - 1,  &m_TXTaskHandle,  0 );
		}
		virtual ~SerialPortMessageManager()
		{
			if(m_RXTaskHandle) vTaskDelete(m_RXTaskHandle);
			if(m_TXTaskHandle) vTaskDelete(m_TXTaskHandle);
		}
		void SetupSerialPortMessageManager()
		{
			m_TXQueue = xQueueCreate(10, sizeof(String) );
			if(m_TXQueue == NULL)
			{
				ESP_LOGE("SerialPortMessageManager", "ERROR! Error creating the TX Queue.");
			}
			else
			{
				ESP_LOGI("SerialPortMessageManager", "Created the TX Queue.");
			}
		}
		void SendMessage(String *message)
		{
			if(NULL != m_TXQueue)
			{
				if(xQueueSend(m_TXQueue, message, 0) != pdTRUE)
				{
					ESP_LOGW("SerialPortMessageManager", "WARNING! Unable to Send Message.");
				}
			}
			else
			{
				ESP_LOGE("SerialPortMessageManager", "Error! NULL Queue!");
			}
		}
	private:
		HardwareSerial &m_Serial;
		String m_TaskName = "";
		TaskHandle_t m_RXTaskHandle;
		TaskHandle_t m_TXTaskHandle;
		std::vector<NewValueCallBack*> m_NewValueCallBacks = std::vector<NewValueCallBack*>();
		QueueHandle_t m_TXQueue;
		static void StaticSerialPortMessageManager_RXLoop(void *Parameters)
		{
			SerialPortMessageManager* aSerialPortMessageManager = (SerialPortMessageManager*)Parameters;
			aSerialPortMessageManager->SerialPortMessageManager_RXLoop();
		}
		void SerialPortMessageManager_RXLoop()
		{
			const TickType_t xFrequency = 20;
			TickType_t xLastWakeTime = xTaskGetTickCount();
			String message = "";
			char character;
			while(true)
			{
				vTaskDelayUntil( &xLastWakeTime, xFrequency );
				while (m_Serial.available())
				{
					character = m_Serial.read();
					if(character == '\n')
					{
						message = m_TaskName + " Debug: " + message;
						Serial.println(message.c_str());
						message = "";
					}
					else
					{
						message.concat(character);
					}
				}
			}
		}
		
		static void StaticSerialPortMessageManager_TXLoop(void *Parameters)
		{
			SerialPortMessageManager* aSerialPortMessageManager = (SerialPortMessageManager*)Parameters;
			aSerialPortMessageManager->SerialPortMessageManager_TXLoop();
		}
		void SerialPortMessageManager_TXLoop()
		{
			const TickType_t xFrequency = 20;
			TickType_t xLastWakeTime = xTaskGetTickCount();
			while(true)
			{
				vTaskDelayUntil( &xLastWakeTime, xFrequency );
				if(NULL != m_TXQueue)
				{
					size_t QueueCount = uxQueueMessagesWaiting(m_TXQueue);
					if(QueueCount > 0)
					{
						ESP_LOGE("SerialPortMessageManager", "Queue Count: %i", QueueCount);
						for(int i = 0; i < QueueCount; ++i)
						{
							String* pmessage;
							if ( xQueueReceive(m_TXQueue, &pmessage, 0) == pdTRUE )
							{
								//String message = printf("%s",(*(&(pmessage[0]))));
								Serial.println(printf("%s",(*(&(pmessage[0])))));
								free(pmessage);
							}
							else
							{
								ESP_LOGE("SerialPortMessageManager", "ERROR! Unable to Send Message.");
							}
						}
					}
				}
				else
				{
					ESP_LOGE("SerialPortMessageManager", "ERROR! NULL TX Queue.");
				}
			}
		}
};

enum TXType_t
{
	TXType_PERIODIC = 0,
	TXType_ON_UPDATE,
	TXType_COUNT
};

template <typename T>
class DataItem: public NewValueCallBack
{
	
	public:
		DataItem( String name, T initialValue, TXType_t txType, uint16_t rate, DataSerializer &dataSerializer, SerialPortMessageManager &serialPortMessageManager )
			    : m_Name(name)
				, m_Value(initialValue)
				, m_TXType(txType)
				, m_Rate(rate)
				, m_DataSerializer(dataSerializer)
				, m_SerialPortMessageManager(serialPortMessageManager)
		{
		}
		virtual ~DataItem()
		{
			if(m_TXTaskHandle) vTaskDelete(m_TXTaskHandle);
		}
		void EnableDatalinkCommunication(bool enable)
		{
			if(enable)
			{
				xTaskCreatePinnedToCore( StaticDataItem_TX, m_Name.c_str(), 1000, this,  configMAX_PRIORITIES - 1,  &m_TXTaskHandle,  0 );	
			}
			else
			{
				if(m_TXTaskHandle) vTaskDelete(m_TXTaskHandle);
			}
		}
		void SetValue(T value)
		{
			m_Value = value;
			if(m_TXType == TXType_ON_UPDATE)
			{
				//DO SOMETHING
			}
		}
	private:
		String m_Name = "";
		T m_Value;
		TXType_t m_TXType = TXType_PERIODIC;
		uint16_t m_Rate = 0;
		DataSerializer &m_DataSerializer;
		SerialPortMessageManager &m_SerialPortMessageManager;
		TaskHandle_t m_TXTaskHandle;
		static void StaticDataItem_TX(void *Parameters)
		{
			DataItem* aDataItem = (DataItem*)Parameters;
			aDataItem->DataItem_TX();
		}	
		void DataItem_TX()
		{
			if(m_TXType == TXType_ON_UPDATE)
			{
				m_Rate = 5000;
			}
			const TickType_t xFrequency = m_Rate;
			TickType_t xLastWakeTime = xTaskGetTickCount();
			while(true)
			{
				vTaskDelayUntil( &xLastWakeTime, xFrequency );
				Serial.println(m_Name + ": TX Periodic Data");
				String *message = new String("Hello World");
				m_SerialPortMessageManager.SendMessage(message);
			}
			
		}
		void NewValueReceived()
		{
			
		}
};