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

#include "SerialMessageManager.h"

void Named_Object_Caller_Interface::RegisterForNewRxValueNotification(Named_Object_Callee_Interface* NewCallee)
{
	ESP_LOGD("RegisterForNewRxValueNotification", "Try Registering Callee");
	bool IsFound = false;
	for (Named_Object_Callee_Interface* callee : m_NewValueCallees)
	{
		if(NewCallee == callee)
		{
			ESP_LOGE("RegisterForNewRxValueNotification", "ERROR! A callee with this name already exists.");
			IsFound = true;
			break;
		}
	}
	if(false == IsFound)
	{
		ESP_LOGD("RegisterForNewRxValueNotification", "Callee Registered");
		m_NewValueCallees.push_back(NewCallee);
	}
}

void Named_Object_Caller_Interface::DeRegisterForNewRxValueNotification(Named_Object_Callee_Interface* Callee)
{
	ESP_LOGD("DeRegisterForNewRxValueNotification", "Try DeRegister For New Value Notification");
	auto it = std::find(m_NewValueCallees.begin(), m_NewValueCallees.end(), Callee);
	if (it != m_NewValueCallees.end()) {
		ESP_LOGD("RegisterForNewRxValueNotification", "Callee DeRegistered");
		m_NewValueCallees.erase(it);
	}
}

void Named_Object_Caller_Interface::Call_Named_Object_Callback(const String& name, void* object, const size_t changeCount)
{
	ESP_LOGD("Call_Named_Object_Callback", "Notify Callee: \"%s\"", name.c_str());
	bool found = false;
	for (Named_Object_Callee_Interface* callee : m_NewValueCallees)
	{
		if (callee) 
		{
			if (callee->GetName().equals(name))
			{
				found = true;
				ESP_LOGD("Call_Named_Object_Callback", "Callee Found: \"%s\"", name.c_str());
				callee->New_Object_From_Sender(this, object, changeCount);
				break;
			}
		}
	}
	if(!found) ESP_LOGE("NewRxValueReceived", "ERROR! Rx Value Callee Not Found Found: \"%s\"", name.c_str());
}

void SerialPortMessageManager::SetupSerialPortMessageManager()
{
	if(xTaskCreatePinnedToCore( StaticSerialPortMessageManager_RxTask, m_Name.c_str(), 10000, this,  THREAD_PRIORITY_RT,  &m_RXTaskHandle,  m_CoreId ) != pdPASS)
	ESP_LOGE("SetupSerialPortMessageManager", "ERROR! Error creating the RX Task.");
	else ESP_LOGD("SetupSerialPortMessageManager", "RX Task Created.");
	
	if(xTaskCreatePinnedToCore( StaticSerialPortMessageManager_TxTask, m_Name.c_str(), 10000, this,  THREAD_PRIORITY_RT,  &m_TXTaskHandle,  m_CoreId ) != pdPASS)
	ESP_LOGE("SetupSerialPortMessageManager", "ERROR! Error creating the TX Task.");
	else ESP_LOGD("SetupSerialPortMessageManager", "TX Task Created.");
	
	m_TXQueue = xQueueCreate(MaxQueueCount, sizeof(char) * MaxMessageLength );
	if(nullptr == m_TXQueue)
	{
		ESP_LOGE("SetupSerialPortMessageManager", "ERROR! Error creating the TX Queue.");
	}
	else ESP_LOGD("SetupSerialPortMessageManager", "TX Queue Created.");
}

bool SerialPortMessageManager::QueueMessageFromData(const String& Name, DataType_t DataType, void* Object, size_t Count, size_t ChangeCount)
{
	bool result = false;
	if(nullptr == Object || 0 == Name.length() || 0 == Count)
	{
		ESP_LOGE("QueueMessageFromData", "ERROR! Invalid Data.");
	}
	else
	{
		if(mp_DataSerializer)
		{
			if(this->GetName().equals("Amp_Gain"))
			{
				ESP_LOGI("QueueMessageFromData", "Serializing Data for: \"%s\" Data Type: \"%i\", Pointer: \"%p\" Count: \"%i\" Change Count: \"%i\" ", Name.c_str(), DataType, static_cast<void*>(Object), Count, ChangeCount);
			}
			String message = mp_DataSerializer->SerializeDataToJson(Name, DataType, Object, Count, ChangeCount);
			result = QueueMessage( message.c_str() );
		}
		else
		{
			ESP_LOGE("QueueMessageFromData", "ERROR! Null Pointer.");
		}
	}
	return result;
}

bool SerialPortMessageManager::QueueMessage(const String& message)
{
	bool result = false;
	if(m_TXQueue)
	{	
		if( message.length() > 0 &&                   // Check if the message is not empty
            message.length() <= MaxMessageLength &&   // Check if the message length is within bounds
            message.indexOf('\0') != -1 &&            // Check if the message is null terminated
            xQueueSend(m_TXQueue, message.c_str(), portMAX_DELAY) == pdTRUE )
		{
			ESP_LOGD("QueueMessage", "\"%s\" Queued Message: \"%s\"", m_Name.c_str(), message.c_str());
			result = true;
		}
		else
		{
			ESP_LOGW("QueueMessage", "WARNING! \"%s\" Unable to Queue Message.", m_Name.c_str());
		}
	}
	else
	{
		ESP_LOGE("QueueMessage", "ERROR! NULL Queue.");
	}
	return result;
}

void SerialPortMessageManager::SerialPortMessageManager_RxTask()
{
    ESP_LOGD("SetupSerialPortMessageManager", "Starting RX Task.");
    const TickType_t xFrequency = 25;
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    while (true)
    {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        
        if (mp_Serial && mp_DataSerializer)
        {
            while (mp_Serial->available())
            {
                char character = mp_Serial->read();
                if (m_message.length() >= MaxMessageLength)
                {
                    ESP_LOGE("SerialPortMessageManager", "ERROR! Message RX Overrun: \"%s\".", m_message.c_str());
                    m_message.clear();
                    break;
                }

                m_message += character;

                if (m_message.endsWith("\n"))
                {
                    m_message.trim();
                    ESP_LOGD("SerialPortMessageManager", "\"%s\" Message Rx: \"%s\"", m_Name.c_str(), m_message.c_str());

                    NamedObject_t NamedObject;
                    if (mp_DataSerializer->DeSerializeJsonToNamedObject(m_message.c_str(), NamedObject))
                    {
                        ESP_LOGD("SerialPortMessageManager", "\"%s\" DeSerialized Named object: \"%s\" Address: \"%p\"", m_Name.c_str(), NamedObject.Name.c_str(), static_cast<void*>(NamedObject.Object));
                        this->Call_Named_Object_Callback(NamedObject.Name, NamedObject.Object, NamedObject.ChangeCount);
                    }
                    else
                    {
                        ESP_LOGW("SerialPortMessageManager", "WARNING! \"%s\" DeSerialized Named object failed", m_Name.c_str());
                    }

                    m_message.clear();
                }
            }
        }
        else
        {
            ESP_LOGE("SerialPortMessageManager_RxTask", "ERROR! Null Pointer.");
            vTaskDelay(100);
        }
    }
}

void SerialPortMessageManager::SerialPortMessageManager_TxTask()
{
	ESP_LOGD("SetupSerialPortMessageManager", "Starting TX Task.");
	const TickType_t xFrequency = 25;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	while(true)
	{
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
		if(NULL != m_TXQueue)
		{
			size_t QueueCount = uxQueueMessagesWaiting(m_TXQueue);
			if(QueueCount) ESP_LOGD("SerialPortMessageManager_TxTask", "Queue Count: %i", QueueCount);
			for(size_t i = 0; i < QueueCount; ++i)
			{
				char message[MaxMessageLength] = "\0";
				if ( xQueueReceive(m_TXQueue, message, 0) == pdTRUE )
				{
					if (strlen(message) > MaxMessageLength)
                    {
                        ESP_LOGW("SerialPortMessageManager_TxTask", "\"%s\" WARNING! Message exceeds MaxMessageLength. Truncating.",m_Name.c_str());
                        message[MaxMessageLength - 1] = '\0';
                    }
					ESP_LOGD("SerialPortMessageManager_TxTask", "\"%s\" Data TX: Address: \"%p\" Message: \"%s\"",m_Name.c_str(), static_cast<void*>(message), message);
					mp_Serial->println(message);
				}
				else
				{
					ESP_LOGE("SerialPortMessageManager_TxTask", "\"%s\" ERROR! Unable to Send Message.",m_Name.c_str());
				}
			}
		}
		else
		{
			ESP_LOGE("SerialPortMessageManager_TxTask", "ERROR! NULL TX Queue.");
		}
	}
}