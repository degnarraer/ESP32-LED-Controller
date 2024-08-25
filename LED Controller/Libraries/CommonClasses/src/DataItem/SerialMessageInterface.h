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
#include <iostream>
#include <sstream>

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
class SerialMessageInterface: public NewRxValue_Caller_Interface<T>
			  				, public NewRxTxVoidObjectCalleeInterface
{
	public:
		SerialMessageInterface( SerialPortMessageManager *serialPortMessageManager = nullptr )
							  : NewRxTxVoidObjectCalleeInterface(COUNT)
							  , mp_SerialPortMessageManager(serialPortMessageManager)
							  {
								ESP_LOGD("SerialMessageInterface", "Constructor1 SerialMessageInterface");
							  }
		SerialMessageInterface( RxTxType_t rxTxType
							  , UpdateStoreType_t updateStoreType
							  , uint16_t rate
							  , SerialPortMessageManager *serialPortMessageManager = nullptr )
							  : NewRxTxVoidObjectCalleeInterface(COUNT)
							  , m_RxTxType(rxTxType)
							  , m_UpdateStoreType(updateStoreType)
							  , m_Rate(rate)
							  , mp_SerialPortMessageManager(serialPortMessageManager)
							  {
								ESP_LOGD("SerialMessageInterface", "Constructor2 SerialMessageInterface");
							  }

		virtual ~SerialMessageInterface()
		{
			ESP_LOGD("~DataItem", "Deleting SerialMessageInterface");
			DestroyTimer();
			if(mp_SerialPortMessageManager)
			{
				mp_SerialPortMessageManager->DeRegisterForNewRxValueNotification(this);
			}
			if(mp_RxValue)
			{
        		ESP_LOGD("~SerialMessageInterface", "freeing mp_RxValue Memory");
				heap_caps_free(mp_RxValue);
			}
			if(mp_TxValue)
			{
        		ESP_LOGD("~SerialMessageInterface", "freeing mp_TxValue Memory");
				heap_caps_free(mp_TxValue);	
			}
		}
		virtual T* GetValuePointer() const = 0;
		virtual bool UpdateStore(const T *value, size_t count) = 0;
		virtual bool EqualsValue(T *Object, size_t Count) const = 0;
		virtual String GetName() const = 0;
		virtual String GetValueAsString() const = 0;
		virtual DataType_t GetDataType() = 0;
		virtual String ConvertValueToString(const T *pvalue, size_t count) const = 0;
		virtual size_t ParseStringValueIntoValues(const String& stringValue, T* values) = 0;

		void Configure( RxTxType_t rxTxType
					  , UpdateStoreType_t updateStoreType
					  , uint16_t rate )
		{
			m_RxTxType = rxTxType;
			m_UpdateStoreType = updateStoreType;
			m_Rate = rate;
		}

		virtual bool NewRxValueReceived(const NewRxTxVoidObjectCallerInterface* sender, const void* values, size_t count) override
		{	
			assert(count == COUNT);
			bool StoreUpdated = false;
			const T* receivedValue = static_cast<const T*>(values);
			ESP_LOGI( "DataItem: NewRxValueReceived"
					, "\"%s\" RX: \"%s\" Value: \"%s\""
					, mp_SerialPortMessageManager->GetName().c_str()
					, this->GetName().c_str()
					, this->ConvertValueToString(receivedValue, count).c_str());
			if(memcmp(mp_RxValue, receivedValue, sizeof(T) * count) != 0)
			{
				ESP_LOGI( "DataItem: NewRxValueReceived"
						, "Value Changed for: \"%s\" to Value: \"%s\""
						, this->GetName().c_str()
						, this->ConvertValueToString(receivedValue, count).c_str());
				ZeroOutMemory(mp_RxValue);
				memcpy(mp_RxValue, receivedValue, sizeof(T) * count);
				if( UpdateStoreType_On_Rx == m_UpdateStoreType )
				{
					StoreUpdated = this->UpdateStore(mp_RxValue, count);
				}
			}
			if(RxTxType_Rx_Echo_Value == m_RxTxType)
			{
				ESP_LOGD( "DataItem: NewRxValueReceived"
						, "RX Echo for: \"%s\" with Value: \"%s\""
						, this->GetName().c_str()
						, this->ConvertValueToString(receivedValue, count).c_str());
				ZeroOutMemory(mp_TxValue);
				memcpy(mp_TxValue, mp_RxValue, sizeof(T) * count);
				Tx_Now();
			}
			return StoreUpdated;
		}

		void Setup()
		{
			if(!mp_RxValue) mp_RxValue = (T*)heap_caps_malloc(sizeof(T)*COUNT, MALLOC_CAP_SPIRAM);
			if(!mp_TxValue) mp_TxValue = (T*)heap_caps_malloc(sizeof(T)*COUNT, MALLOC_CAP_SPIRAM);
			if(this->GetValuePointer())
			{
				if (mp_RxValue && mp_TxValue)
				{
					ESP_LOGD("DataItem<T, COUNT>::Setup()", "Setting Initial Tx/Rx Values to: %s", this->GetValueAsString().c_str());
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
			assert(mp_TxValue);
			bool StoreUpdated = false;
			ESP_LOGD( "Set_Tx_Value", "\"%s\" Set Tx Value for: \"%s\": Current Value: \"%s\" New Value: \"%s\""
					, mp_SerialPortMessageManager->GetName().c_str()
					, this->GetName().c_str()
					, this->ConvertValueToString(mp_TxValue, count).c_str()
					, this->ConvertValueToString(newTxValue, count).c_str() );
			if(count <= COUNT)
			{
				if(memcmp(mp_TxValue, newTxValue, sizeof(T)*count) != 0)
				{
					ESP_LOGI( "Set_Tx_Value", "\"%s\" Set Tx Value for: \"%s\": Value changed."
							, mp_SerialPortMessageManager->GetName().c_str()
							, this->GetName().c_str() );
					ZeroOutMemory(mp_TxValue);
					memcpy(mp_TxValue, newTxValue, sizeof(T)*count);
					StoreUpdated = Try_TX_On_Change();
				}
				else
				{
					ESP_LOGI( "Set_Tx_Value", "\"%s\" Set Tx Value for: \"%s\": Value did not change."
							, mp_SerialPortMessageManager->GetName().c_str()
							, this->GetName().c_str() );
				}
				return StoreUpdated;
			}
			else
			{
				ESP_LOGE("Set_Tx_Value", "Name: \"%s\" Count Error!", this->GetName().c_str() );
				return false;
			}
		}

		bool Tx_Now()
		{
			ESP_LOGD( "Tx_Now", "\"%s\" Tx: \"%s\" Value: \"%s\""
					, mp_SerialPortMessageManager->GetName().c_str()
					, this->GetName().c_str()
					, this->GetValueAsString().c_str() );
			if(mp_SerialPortMessageManager)
			{
				
				if(mp_SerialPortMessageManager->QueueMessageFromData(this->GetName(), this->GetDataType(), mp_TxValue, COUNT))
				{
					ESP_LOGI( "Tx_Now", "\"%s\": Messaged Queued for TX", this->GetName().c_str());
					if(m_UpdateStoreType == UpdateStoreType_On_Tx)
					{
						ESP_LOGI( "Tx_Now", "\"%s\": Updating Store on Tx", this->GetName().c_str());
						return this->UpdateStore(mp_TxValue, COUNT);	
					}
				}
				else
				{
					ESP_LOGE("Tx_Now", "ERROR! Data Item: \"%s\": Unable to Tx Message.", this->GetName().c_str());
				}
			}
			else
			{
				ESP_LOGE("Tx_Now", "ERROR! Null Pointer!");
			}
			return false;
		}		
	protected:
		bool m_DataLinkEnabled = false;
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
			if(mp_SerialPortMessageManager)
			{
				if(enable)
				{
					ESP_LOGD( "SetDataLinkEnabled", "\"%s\" Set Datalink Enabled for: \"%s\""
							, mp_SerialPortMessageManager->GetName().c_str()
							, this->GetName().c_str() );
					bool enablePeriodicTx = false;
					bool enableRx = false;
					switch(m_RxTxType)
					{
						case RxTxType_Tx_Periodic:
						case RxTxType_Tx_On_Change_With_Heartbeat:
							enablePeriodicTx = true;
							enableRx = true;
							Tx_Now();
							break;
						case RxTxType_Tx_On_Change:
							Tx_Now();
						case RxTxType_Rx_Only:
						case RxTxType_Rx_Echo_Value:
							enableRx = true;
							break;
						default:
						break;
					}
					EnableRx(enableRx);
					EnablePeriodicTx(enablePeriodicTx);
				}
				else
				{
					ESP_LOGD( "SetDataLinkEnabled", "\"%s\" Set Datalink Disabled for: \"%s\""
							, mp_SerialPortMessageManager->GetName().c_str()
							, this->GetName().c_str() );
					EnableRx(enable);
					EnablePeriodicTx(enable);
				}
				m_DataLinkEnabled = enable;
			}
			else
			{
				ESP_LOGE( "SetDataLinkEnabled", "ERROR! Null Pointer." );
			}
		}

		static void Static_Periodic_TX(void *arg)
		{
			SerialMessageInterface *aSerialMessageInterface = static_cast<SerialMessageInterface*>(arg);
			if(aSerialMessageInterface)
			{
				ESP_LOGD( "EnablePeriodicTx", "\"%s\": Periodic Tx"
						, aSerialMessageInterface->GetName().c_str() );
				aSerialMessageInterface->Tx_Now();
			}
			else
			{
				ESP_LOGE( "SetDataLinkEnabled", "ERROR! Null Pointer." );
			}
		}

		bool Try_TX_On_Change()
		{
			ESP_LOGI("Try_TX_On_Change", "\"%s\": Try TX On Change", this->GetName().c_str());
			if(m_RxTxType == RxTxType_Tx_On_Change || m_RxTxType == RxTxType_Tx_On_Change_With_Heartbeat)
			{
				return Tx_Now();
			}
			return false;
		}

		void EnablePeriodicTx(bool enablePeriodicTx)
		{
			if(enablePeriodicTx)
			{
				ESP_LOGD( "EnablePeriodicTx", "\"%s\" Enable Tx for: \"%s\""
					, mp_SerialPortMessageManager->GetName().c_str()
					, this->GetName().c_str() );
				StartTimer();
			}
			else
			{
				ESP_LOGD( "EnablePeriodicTx", "\"%s\" Disable Tx for: \"%s\""
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
					ESP_LOGD( "EnableRx", "\"%s\" Enable Rx for: \"%s\""
							, mp_SerialPortMessageManager->GetName().c_str()
							, this->GetName().c_str() );
					mp_SerialPortMessageManager->RegisterForNewRxValueNotification(this);
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
					ESP_LOGD( "EnableRx", "\"%s\" Disable Rx for: \"%s\""
							, mp_SerialPortMessageManager->GetName().c_str()
							, this->GetName().c_str() );
					mp_SerialPortMessageManager->DeRegisterForNewRxValueNotification(this);
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

		void ZeroOutMemory(T* object)
		{
			memset((void*)object, 0, sizeof(T) * COUNT);
		}
};