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

#include <Serial_Datalink_Core.h>

SerialDataLinkCore::SerialDataLinkCore(String Title): NamedItem(Title)
{
}
SerialDataLinkCore::~SerialDataLinkCore()
{

}

void SerialDataLinkCore::Setup()
{
  hSerial.begin(500000, SERIAL_8N1, 16, 17); // pins 16 rx2, 17 tx2, 19200 bps, 8 bits no parity 1 stop bit        
  delay(500);

  DataItemConfig_t* ConfigFile = GetConfig();
  m_DataItemCount = GetConfigCount();
  size_t ConfigBytes = sizeof(DataItem_t) * m_DataItemCount;
  Serial << GetTitle() << ": Allocating " << m_DataItemCount << " DataItem's for a total of " << ConfigBytes << " bytes of Memory\n";
  m_DataItem = (DataItem_t*)malloc(ConfigBytes);
  for(int i = 0; i < m_DataItemCount; ++i)
  {
    void* Object;
    QueueHandle_t QueueHandle = NULL;
    switch(ConfigFile[i].DataType)
    {
      case DataType_Int16_t:
      {
        size_t bytes = sizeof(int16_t) * ConfigFile[i].Count;
        Serial << GetTitle() << ": Allocating " << ConfigFile[i].Count << " " << DataTypeStrings[ConfigFile[i].DataType] << " for a total of " << bytes << " bytes of Memory\n";
        Object = (int16_t*)malloc(bytes);
        QueueHandle = xQueueCreate(QUEUE_SIZE, bytes );
        if(QueueHandle == NULL){ Serial << "Error creating " << ConfigFile[i].Name << " queue\n"; }
        else { Serial << GetTitle() << ": " << bytes << " bytes wide by " << QUEUE_SIZE << " bytes deep Queue Created for a total of " << bytes * QUEUE_SIZE << " bytes\n"; }
      }
      break;
      case DataType_Int32_t:
      {
        size_t bytes = sizeof(int32_t) * ConfigFile[i].Count;
        Serial << GetTitle() << ": Allocating " << ConfigFile[i].Count << " " << DataTypeStrings[ConfigFile[i].DataType] << " for a total of " << bytes << " bytes of Memory\n";
        Object = (int32_t*)malloc(bytes);
        QueueHandle = xQueueCreate(QUEUE_SIZE, bytes );
        if(QueueHandle == NULL){ Serial << "Error creating " << ConfigFile[i].Name << " queue\n"; }
        else { Serial << GetTitle() << ": " << bytes << " bytes wide by " << QUEUE_SIZE << " bytes deep Queue Created for a total of " << bytes * QUEUE_SIZE << " bytes\n"; }
      }
      break;
      case DataType_Uint16_t:
      {
        size_t bytes = sizeof(uint16_t) * ConfigFile[i].Count;
        Serial << GetTitle() << ": Allocating " << ConfigFile[i].Count << " " << DataTypeStrings[ConfigFile[i].DataType] << " for a total of " << bytes << " bytes of Memory\n";
        Object = (uint16_t*)malloc(bytes);
        QueueHandle = xQueueCreate(QUEUE_SIZE, bytes );
        if(QueueHandle == NULL){ Serial << "Error creating " << ConfigFile[i].Name << " queue\n"; }
        else { Serial << GetTitle() << ": " << bytes << " bytes wide by " << QUEUE_SIZE << " bytes deep Queue Created for a total of " << bytes * QUEUE_SIZE << " bytes\n"; }
      }
      break;
      case DataType_Uint32_t:
      {
        size_t bytes = sizeof(uint32_t) * ConfigFile[i].Count;
        Serial << GetTitle() << ": Allocating " << ConfigFile[i].Count << " " << DataTypeStrings[ConfigFile[i].DataType] << " for a total of " << bytes << " bytes of Memory\n";
        Object = (uint32_t*)malloc(bytes);
        QueueHandle = xQueueCreate(QUEUE_SIZE, bytes );
        if(QueueHandle == NULL){ Serial << "Error creating " << ConfigFile[i].Name << " queue\n"; }
        else { Serial << GetTitle() << ": " << bytes << " bytes wide by " << QUEUE_SIZE << " bytes deep Queue Created for a total of " << bytes * QUEUE_SIZE << " bytes\n"; }
      }
      break;
      case DataType_String:
      {
        size_t bytes = sizeof(String) * ConfigFile[i].Count;
        Serial << GetTitle() << ": Allocating " << ConfigFile[i].Count << " " << DataTypeStrings[ConfigFile[i].DataType] << " for a total of " << bytes << " bytes of Memory\n";
        Object = (String*)malloc(bytes);
        QueueHandle = xQueueCreate(QUEUE_SIZE, bytes );
        if(QueueHandle == NULL){ Serial << "Error creating " << ConfigFile[i].Name << " queue\n"; }
        else { Serial << GetTitle() << ": " << bytes << " bytes wide by " << QUEUE_SIZE << " bytes deep Queue Created for a total of " << bytes * QUEUE_SIZE << " bytes\n"; }
      }
      break;
      default:
        Serial << "Error, unsupported data type";
      break;
    }
    Serial << GetTitle() << ": Try Saving DataItem " << i+1 << " of " << m_DataItemCount << "\n"; 
    m_DataItem[i].Name = ConfigFile[i].Name;
    m_DataItem[i].DataType = ConfigFile[i].DataType;
    m_DataItem[i].Count = ConfigFile[i].Count;
    m_DataItem[i].QueueHandle = QueueHandle;
	m_DataItem[i].TransceiverConfig = ConfigFile[i].TransceiverConfig;
    m_DataItem[i].Object = Object;
    Serial << GetTitle() << ": Successfully Saved DataItem " << i+1 << " of " << m_DataItemCount << "\n"; 
  }
}

QueueHandle_t SerialDataLinkCore::GetQueueHandleForDataItem(String Name)
{
	if(NULL != m_DataItem)
	{
		for(int i = 0; i < m_DataItemCount; ++i)
		{
			if(Name == m_DataItem[i].Name)
			{
				return m_DataItem[i].QueueHandle;
			}
		}
	}
	return NULL;
}

void SerialDataLinkCore::CheckForNewSerialData()
{
  if(true == hSerial.available())
  {
    byte ch;
    ch = hSerial.read();
    m_InboundStringData += (char)ch;
    if (ch=='\n') 
    {
      m_InboundStringData.trim();
      Serial << "Data Received from CPU 2: " << m_InboundStringData << "\n";
      m_InboundStringData = "";
    } 
  }
}

void SerialDataLinkCore::ProcessEventQueue()
{
  if(NULL != m_DataItem)
  {
    for(int i = 0; i < m_DataItemCount; ++i)
    {
      if(NULL != m_DataItem[i].QueueHandle)
      {
        uint8_t queueCount = uxQueueMessagesWaiting(m_DataItem[i].QueueHandle);
        for(int j = 0; j < queueCount; ++j)
        {
			if(Transciever_TX == m_DataItem[i].TransceiverConfig || Transciever_TXRX == m_DataItem[i].TransceiverConfig)
			{
				switch(m_DataItem[i].DataType)
				{
					case DataType_Int16_t:
						EncodeAndTransmitData<int16_t>(m_DataItem[i].Name, m_DataItem[i].Object, m_DataItem[i].Count);
					break;
					case DataType_Int32_t:
						EncodeAndTransmitData<int32_t>(m_DataItem[i].Name, m_DataItem[i].Object, m_DataItem[i].Count);
					break;
					case DataType_Uint16_t:
						EncodeAndTransmitData<uint16_t>(m_DataItem[i].Name, m_DataItem[i].Object, m_DataItem[i].Count);
					break;
					case DataType_Uint32_t:
						EncodeAndTransmitData<uint32_t>(m_DataItem[i].Name, m_DataItem[i].Object, m_DataItem[i].Count);
					break;
					case DataType_String:
						//EncodeAndTransmitData<String>(m_DataItem[i].Name, m_DataItem[i].Object, m_DataItem[i].Count);
					break;
					default:
					  Serial << "Error, unsupported data type";
					break;
				}
			}
        }
      }
    } 
  }
}
