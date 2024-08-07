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
#include "SerialMessageManager.h"

enum RxTxType_t
{
	RxTxType_Tx_Periodic,
	RxTxType_Tx_On_Change,
	RxTxType_Tx_On_Change_With_Heartbeat,
	RxTxType_Rx_Only,
	RxTxType_Rx_Echo_Value,
	RxTxType_Count
};

enum UpdateStoreType_t
{
	UpdateStoreType_On_Tx,
	UpdateStoreType_On_Rx,
	UpdateStoreType_Count
};

template <typename T, size_t COUNT>
class SerialDataLinkInterface: public NewRxTxValueCallerInterface<T>
			  				 , public NewRxTxVoidObjectCalleeInterface
{
	public:
		SerialDataLinkInterface( SerialPortMessageManager *serialPortMessageManager = nullptr )
							   : NewRxTxVoidObjectCalleeInterface(COUNT)
							   , mp_SerialPortMessageManager(serialPortMessageManager){}
		SerialDataLinkInterface( RxTxType_t rxTxType
							   , UpdateStoreType_t updateStoreType
							   , uint16_t rate
							   , SerialPortMessageManager *serialPortMessageManager = nullptr )
							   : NewRxTxVoidObjectCalleeInterface(COUNT)
							   , m_RxTxType(rxTxType)
							   , m_UpdateStoreType(updateStoreType)
							   , m_Rate(rate)
							   , mp_SerialPortMessageManager(serialPortMessageManager){}

		virtual ~SerialDataLinkInterface()
		{
			SetDataLinkEnabled(false);
			if(mp_RxValue)
			{
        		ESP_LOGD("~DataItem", "freeing mp_RxValue Memory");
				heap_caps_free(mp_RxValue);
			}
			if(mp_TxValue)
			{
        		ESP_LOGD("~DataItem", "freeing mp_TxValue Memory");
				heap_caps_free(mp_TxValue);	
			}
		}
		virtual T* GetValuePointer() const = 0;
		virtual bool SetValue(const T *value, size_t count) = 0;
		virtual bool EqualsValue(T *Object, size_t Count) const = 0;
		virtual String GetName() const = 0;
		virtual String GetValueAsString() const = 0;
		virtual DataType_t GetDataType() = 0;

		void Configure( RxTxType_t rxTxType
					  , UpdateStoreType_t updateStoreType
					  , uint16_t rate )
		{
			m_RxTxType = rxTxType;
			m_UpdateStoreType = updateStoreType;
			m_Rate = rate;
		}

		virtual bool NewRxValueReceived(void* Object, size_t Count) override
		{	
			bool ValueUpdated = false;
			T* receivedValue = static_cast<T*>(Object);
			ESP_LOGD( "DataItem: NewRxValueReceived"
					, "\"%s\" RX: \"%s\" Value: \"%s\""
					, mp_SerialPortMessageManager->GetName().c_str()
					, this->GetName().c_str()
					, this->GetValueAsString().c_str());
			if(memcmp(mp_RxValue, receivedValue, sizeof(T) * COUNT) != 0)
			{
				memcpy(mp_RxValue, receivedValue, sizeof(T) * COUNT);
				ESP_LOGD( "DataItem: NewRxValueReceived"
						, "Value Changed for: \"%s\" to Value: \"%s\""
						, this->GetName().c_str()
						, this->GetValueAsString().c_str());
				if( UpdateStoreType_On_Rx == m_UpdateStoreType )
				{
					SetValue(mp_RxValue, COUNT);	
					ValueUpdated = true;
				}
			}
			if(RxTxType_Rx_Echo_Value == m_RxTxType)
			{
				memcpy(mp_TxValue, mp_RxValue, sizeof(T) * COUNT);
				ESP_LOGD( "DataItem: NewRxValueReceived"
						, "RX Echo for: \"%s\" with Value: \"%s\""
						, this->GetName().c_str()
						, this->GetValueAsString().c_str());
				Tx_Now();
			}
			return ValueUpdated;
		}

		void Setup()
		{
			if(!mp_RxValue) mp_RxValue = (T*)heap_caps_malloc(sizeof(T)*COUNT, MALLOC_CAP_SPIRAM);
			if(!mp_TxValue) mp_TxValue = (T*)heap_caps_malloc(sizeof(T)*COUNT, MALLOC_CAP_SPIRAM);
			if(this->GetValuePointer())
			{
				if (mp_RxValue && mp_TxValue)
				{
					ESP_LOGD("DataItem<T, COUNT>::Setup()", "Setting Initial Tx/Rx Values");
					memcpy(mp_RxValue, this->GetValuePointer(), sizeof(T)*COUNT);
					memcpy(mp_TxValue, this->GetValuePointer(), sizeof(T)*COUNT);
					SetDataLinkEnabled(true);
				}
				else
				{
					ESP_LOGE("DataItem<T, COUNT>::Setup()", "ERROR! Failed to allocate memory on SPI RAM.");
				}
			}
			else
			{
				ESP_LOGE("DataItem<T, COUNT>::Setup()", "ERROR! Null Pointer.");
			}
		}

		bool Set_Tx_Value(const T* newTxValue, size_t count)
		{
			assert(count == COUNT);
			assert(mp_TxValue);
			bool valueUpdated = false;
			if(memcmp(mp_TxValue, newTxValue, sizeof(T)*count))
			{
				ESP_LOGD( "Set_Tx_Value", "\"%s\" Set Tx Value for for: \"%s\": Did not change"
						, mp_SerialPortMessageManager->GetName().c_str()
						, this->GetName().c_str() );
				return valueUpdated;
			}
			else
			{
				ESP_LOGD( "Set_Tx_Value", "\"%s\" Set Tx Value for for: \"%s\": Changed"
						, mp_SerialPortMessageManager->GetName().c_str()
						, this->GetName().c_str() );
				memcpy(mp_TxValue, newTxValue, sizeof(T)*count);
				Try_TX_On_Change();
				valueUpdated = true;
			}
			return valueUpdated;
		}

		bool Tx_Now()
		{
			bool ValueUpdated = false;
			if(mp_SerialPortMessageManager->QueueMessageFromData(this->GetName(), this->GetDataType(), mp_TxValue, COUNT))
			{
				if(!this->EqualsValue(mp_TxValue, COUNT))
				{
					if(m_UpdateStoreType == UpdateStoreType_On_Tx)
					{
						ValueUpdated = this->SetValue(mp_TxValue, COUNT);	
					}
				}
				ESP_LOGD( "Tx_Now", "\"%s\" TX: \"%s\" Value: \"%s\""
						, mp_SerialPortMessageManager->GetName().c_str()
						, this->GetName().c_str()
						, this->GetValueAsString().c_str() );
			}
			else
			{
				ESP_LOGE("Tx_Now", "ERROR! Data Item: \"%s\": Unable to Tx Message.", this->GetName().c_str());
			}
			return ValueUpdated;
		}		
	protected:
		bool m_DataLinkEnabled = true;
		RxTxType_t m_RxTxType;
		UpdateStoreType_t m_UpdateStoreType;
		uint16_t m_Rate;
		SerialPortMessageManager *mp_SerialPortMessageManager = nullptr;
		T *mp_RxValue = nullptr;
		T *mp_TxValue = nullptr;
	private:
		esp_timer_handle_t m_TxTimer = nullptr;
		esp_timer_create_args_t timerArgs;
		
		void SetDataLinkEnabled(bool enable)
		{
			m_DataLinkEnabled = enable;
			if(m_DataLinkEnabled)
			{
				ESP_LOGD( "SetDataLinkEnabled", "\"%s\" Set Datalink Enabled for: \"%s\""
						, mp_SerialPortMessageManager->GetName().c_str()
						, this->GetName().c_str() );
				bool enableTx = false;
				bool enableRx = false;
				switch(m_RxTxType)
				{
					case RxTxType_Tx_Periodic:
					case RxTxType_Tx_On_Change_With_Heartbeat:
						enableTx = true;
						enableRx = true;
						break;
					case RxTxType_Tx_On_Change:
					case RxTxType_Rx_Only:
					case RxTxType_Rx_Echo_Value:
						enableRx = true;
						break;
					default:
					break;
				}
				EnableRx(enableRx);
				EnableTx(enableTx);
			}
			else
			{
				ESP_LOGD( "SetDataLinkEnabled", "\"%s\" Set Datalink Disabled for: \"%s\""
						, mp_SerialPortMessageManager->GetName().c_str()
						, this->GetName().c_str() );
				EnableRx(false);
				EnableTx(false);
			}
		}

		static void Static_Periodic_TX(void *arg)
		{
			SerialDataLinkInterface *aSerialDataLinkInterface = static_cast<SerialDataLinkInterface*>(arg);
			if(aSerialDataLinkInterface)
			{
				ESP_LOGD( "EnableTx", "\"%s\": Periodic Tx"
						, aSerialDataLinkInterface->GetName().c_str() );
				aSerialDataLinkInterface->Tx_Now();
			}
			else
			{
				ESP_LOGE( "SetDataLinkEnabled", "ERROR! Null Pointer." );
			}
		}

		void Try_TX_On_Change()
		{
			ESP_LOGD("Try_TX_On_Change", "\"%s\": Try TX On Change", this->GetName().c_str());
			if(m_RxTxType == RxTxType_Tx_On_Change || m_RxTxType == RxTxType_Tx_On_Change_With_Heartbeat)
			{
				Tx_Now();
			}
		}

		void EnableTx(bool enableTX)
		{
			if(enableTX)
			{
				ESP_LOGD( "EnableTx", "\"%s\" Enable Tx for: \"%s\""
					, mp_SerialPortMessageManager->GetName().c_str()
					, this->GetName().c_str() );
				StartTimer();
			}
			else
			{
				ESP_LOGD( "EnableTx", "\"%s\" Disable Tx for: \"%s\""
					, mp_SerialPortMessageManager->GetName().c_str()
					, this->GetName().c_str() );
				StopTimer();
			}
		}

		void EnableRx(bool enableRX)
		{
			if(enableRX)
			{
				if(mp_SerialPortMessageManager)
				{
					ESP_LOGD( "EnableTx", "\"%s\" Enable Rx for: \"%s\""
							, mp_SerialPortMessageManager->GetName().c_str()
							, this->GetName().c_str() );
					mp_SerialPortMessageManager->RegisterForNewValueNotification(this);
				}
				else
				{
					ESP_LOGE("SetDataLinkEnabled", "ERROR!  Null Pointer.");
				}
			}
			else
			{
				if(mp_SerialPortMessageManager)
				{
					ESP_LOGD( "EnableTx", "\"%s\" Disable Rx for: \"%s\""
							, mp_SerialPortMessageManager->GetName().c_str()
							, this->GetName().c_str() );
					mp_SerialPortMessageManager->DeRegisterForNewValueNotification(this);
				}
				else
				{
					ESP_LOGE("SetDataLinkEnabled", "ERROR! Null Pointer.");
				}
			}
		}

		bool CreateTxTimer()
		{
			ESP_LOGD("CreateTxTimer", "Creating Timer");
			timerArgs.callback = &Static_Periodic_TX;
			timerArgs.arg = this;
			timerArgs.name = "Tx_Timer";
			if (!m_TxTimer)
			{
				if (ESP_OK == esp_timer_create(&timerArgs, &m_TxTimer))
				{
					ESP_LOGD("CreateTxTimer", "Timer Created");
					return true;
				}
				else
				{
					ESP_LOGE("CreateTxTimer", "ERROR! Unable to create timer.");
					return false;
				}
			}
			else
			{
				ESP_LOGD("CreateTxTimer", "Timer already exists.");
			}
			return false;
		}

		bool DestroyTimer()
		{
			ESP_LOGD("DestroyTimer", "Destroying Timer");
			if (m_TxTimer)
			{
				if (StopTimer())
				{
					if (ESP_OK == esp_timer_delete(m_TxTimer))
					{
						ESP_LOGD("DestroyTimer", "Timer deleted");
						m_TxTimer = nullptr;
						return true;
					}
					else
					{
						ESP_LOGE("DestroyTimer", "ERROR! Unable to delete Timer.");
					}
				}
				else
				{
					ESP_LOGW("DestroyTimer", "WARNING! Unable to stop Timer.");
				}
			}
			else
			{
				ESP_LOGD("DestroyTimer", "Timer does not exist");
			}
			return false;
		}

		bool StartTimer()
		{
			if (m_TxTimer || CreateTxTimer())
			{
				ESP_LOGD("StartTimer", "Starting Timer");
				if (ESP_OK == esp_timer_start_periodic(m_TxTimer, m_Rate * 1000))
				{
					ESP_LOGD("StartTimer", "Timer Started");
					return true;
				}
				else
				{
					ESP_LOGE("StartTimer", "ERROR! Unable to start timer.");
				}
			}
			else
			{
				ESP_LOGE("StartTimer", "ERROR! Unable to create timer.");
			}
			return false;
		}

		bool StopTimer()
		{
			bool result = false;
			if (m_TxTimer)
			{
				if (ESP_OK == esp_timer_stop(m_TxTimer))
				{
					ESP_LOGD("StopTimer", "Timer Stopped!");
					result = true;
				}
				else
				{
					ESP_LOGE("StopTimer", "ERROR! Unable to stop timer.");
				}
			}
			else
			{
				ESP_LOGD("StopTimer", "Timer does not exist");
				result = true;
			}
			return result;
		}
};