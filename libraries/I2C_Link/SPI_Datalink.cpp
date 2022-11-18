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
#include "SPI_Datalink.h"
#include "pthread.h"

void SPI_Datalink_Master::Setup_SPI_Master()
{	
    for (uint8_t i = 0; i < N_MASTER_QUEUES; ++i) 
	{
		spi_rx_buf[i] = m_SPI_Master.allocDMABuffer(SPI_MAX_DATA_BYTES);
		spi_tx_buf[i] = m_SPI_Master.allocDMABuffer(SPI_MAX_DATA_BYTES);
		if(NULL == spi_rx_buf[i] || NULL == spi_tx_buf[i])
		{
			ESP_LOGE("SPI_Datalink", "Error Creating DMA Buffers!");
		}
	}
	m_SPI_Master.setDMAChannel(m_DMA_Channel);
	m_SPI_Master.setMaxTransferSize(SPI_MAX_DATA_BYTES);
	m_SPI_Master.setDataMode(SPI_MODE0);
	m_SPI_Master.setFrequency(CLOCK_SPEED);
	m_SPI_Master.setDutyCyclePos(DUTY_CYCLE_POS);
	m_SPI_Master.setQueueSize(N_MASTER_QUEUES);
	Begin();
}
void SPI_Datalink_Master::ProcessDataTXEventQueue()
{
	if(NULL != m_DataItems)
	{
		if(true == m_TransmitQueuedDataFlag)
		{
			TransmitQueuedData();
		}
		size_t MessageCount = 0;
		size_t MaxTransmits = 5;
		for(int i = 0; i < m_DataItemsCount; ++i)
		{
			if(NULL != m_DataItems[i].QueueHandle_TX)
			{
				MessageCount += uxQueueMessagesWaiting(m_DataItems[i].QueueHandle_TX);
			}
		}
		
		while(MessageCount > 0 && MaxTransmits > 0)
		{
			for(int i = 0; i < m_DataItemsCount; ++i)
			{
				if(NULL != m_DataItems[i].QueueHandle_TX)
				{
					if(uxQueueMessagesWaiting(m_DataItems[i].QueueHandle_TX) > 0)
					{
						if ( xQueueReceive(m_DataItems[i].QueueHandle_TX, m_DataItems[i].DataBuffer, portMAX_DELAY) == pdTRUE )
						{
							EncodeAndTransmitData(m_DataItems[i].Name, m_DataItems[i].DataType, m_DataItems[i].DataBuffer, m_DataItems[i].Count);
							--MessageCount;
						}
					}
				}
			}
			--MaxTransmits;
		}
	}
}

bool SPI_Datalink_Master::Begin() 
{ 
	return m_SPI_Master.begin(HSPI, m_SCK, m_MISO, m_MOSI, m_SS);
}
bool SPI_Datalink_Master::End()
{
	return m_SPI_Master.end();
}

void SPI_Datalink_Master::EncodeAndTransmitData(String Name, DataType_t DataType, void* Object, size_t Count)
{
	size_t CurrentIndex = m_Queued_Transactions % N_MASTER_QUEUES;
	memset(spi_rx_buf[CurrentIndex], 0, SPI_MAX_DATA_BYTES);
	memset(spi_tx_buf[CurrentIndex], 0, SPI_MAX_DATA_BYTES);
	String DataToSend = SerializeDataToJson(Name, DataType, Object, Count);
	size_t DataToSendLength = strlen(DataToSend.c_str());
	size_t PadCount = 0;
	if(0 != DataToSendLength % 4)
	{
		PadCount = 4 - DataToSendLength % 4;
	}
	for(int i = 0; i < PadCount; ++i)
	{
		DataToSend += "\0";
	}
	DataToSendLength += PadCount;
	ESP_LOGV("SPI_Datalink", "TX: %s", DataToSend.c_str());
	assert(DataToSendLength < SPI_MAX_DATA_BYTES);
	memcpy(spi_tx_buf[CurrentIndex], DataToSend.c_str(), DataToSendLength);
	if(m_Queued_Transactions - m_Queued_TransactionsReset >= N_MASTER_QUEUES)
	{
		TransmitQueuedData();
	}
	delay(10); //WITHOUT THIS WE SEND GARBAGE DATA
	m_SPI_Master.queue(spi_tx_buf[CurrentIndex], spi_rx_buf[CurrentIndex], DataToSendLength);
	++m_Queued_Transactions;
}

void SPI_Datalink_Slave::Setup_SPI_Slave()
{
	ESP_LOGE("SPI_Datalink", "Configuring SPI Slave");
    for (uint8_t i = 0; i < N_SLAVE_QUEUES; ++i) 
	{
		spi_rx_buf[i] = m_SPI_Slave.allocDMABuffer(SPI_MAX_DATA_BYTES);
		spi_tx_buf[i] = m_SPI_Slave.allocDMABuffer(SPI_MAX_DATA_BYTES);
		if(NULL == spi_rx_buf[i] || NULL == spi_tx_buf[i])
		{
			ESP_LOGE("SPI_Datalink", "Error Creating DMA Buffers!");
		}
	}
	m_SPI_Slave.setDMAChannel(m_DMA_Channel);
	m_SPI_Slave.setMaxTransferSize(SPI_MAX_DATA_BYTES);
	m_SPI_Slave.setQueueSize(N_SLAVE_QUEUES);
	m_SPI_Slave.setDataMode(SPI_MODE0);

	m_SPI_Slave.begin(HSPI, m_SCK, m_MISO, m_MOSI, m_SS);
	ESP_LOGE("SPI_Datalink", "SPI Slave Configured");
}
void SPI_Datalink_Slave::RegisterForDataTransferNotification(SPI_Slave_Notifier *Notifiee)
{
	m_Notifiee = Notifiee;
}

void SPI_Datalink_Slave::ProcessDataRXEventQueue()
{
	const size_t received_transactions = m_SPI_Slave.available();
    for (size_t q = 0; q < received_transactions; ++q)
	{
		if(NULL != m_Notifiee)
		{
			size_t CurrentIndex = m_DeQueued_Transactions % N_SLAVE_QUEUES;
			m_Notifiee->ReceivedBytesTransferNotification(spi_rx_buf[CurrentIndex], m_SPI_Slave.size());
			m_SPI_Slave.pop();
			++m_DeQueued_Transactions;
			memset(spi_rx_buf[CurrentIndex], 0, SPI_MAX_DATA_BYTES);
        }
    }
	const size_t remained_transactions = m_SPI_Slave.remained();
	for(size_t q = 0; q < N_SLAVE_QUEUES - remained_transactions; ++q)
	{
		size_t CurrentIndex = m_Queued_Transactions % N_SLAVE_QUEUES;
		memset(spi_tx_buf[CurrentIndex], 0, SPI_MAX_DATA_BYTES);
		size_t SendBytesSize = m_Notifiee->SendBytesTransferNotification(spi_tx_buf[CurrentIndex], SPI_MAX_DATA_BYTES);
		if(SendBytesSize > 0)
		{
			m_SPI_Slave.queue(spi_rx_buf[CurrentIndex], spi_tx_buf[CurrentIndex], SendBytesSize);
			++m_Queued_Transactions;
		}
		else
		{
			m_SPI_Slave.queue(spi_rx_buf[CurrentIndex], SPI_MAX_DATA_BYTES);
			++m_Queued_Transactions;
		}
	}
}


