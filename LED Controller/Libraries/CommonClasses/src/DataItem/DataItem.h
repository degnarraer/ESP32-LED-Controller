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

#include "DataItem/LocalDataItem.h"
#include "DataItem/SerialMessageInterface.h"

template <typename T, size_t COUNT>
class DataItem: public LocalDataItem<T, COUNT>
			  , public SerialMessageInterface<T, COUNT>
{
	public:
		DataItem( const String name
				, const T* initialValue
				, const RxTxType_t rxTxType
				, const UpdateStoreType_t updateStoreType
				, const uint16_t rate )
				: LocalDataItem<T, COUNT>(name, initialValue)
				, SerialMessageInterface<T, COUNT>(rxTxType, updateStoreType, rate)
		{
			ESP_LOGI("DataItem", "DataItem Instantiated: Default Constructor 1");
		}

		DataItem( const String name
				, const T& initialValue
				, const RxTxType_t rxTxType
				, const UpdateStoreType_t updateStoreType
				, const uint16_t rate )
				: LocalDataItem<T, COUNT>(name, initialValue)
				, SerialMessageInterface<T, COUNT>(rxTxType, updateStoreType, rate)
		{
			ESP_LOGI("DataItem", "DataItem Instantiated: Default Constructor 2");
		}

		DataItem( const String name
				, const T* initialValue
				, const RxTxType_t rxTxType
				, const UpdateStoreType_t updateStoreType
				, const uint16_t rate
				, SerialPortMessageManager *serialPortMessageManager
				, NamedCallback_t *namedCallback
				, SetupCallerInterface *setupCallerInterface )
				: LocalDataItem<T, COUNT>( name, initialValue, namedCallback, setupCallerInterface)
				, SerialMessageInterface<T, COUNT>(rxTxType, updateStoreType, rate, serialPortMessageManager)
		{
			ESP_LOGI("DataItem", "DataItem Instantiated: Constructor 1");
		}

		DataItem( const String name
				, const T& initialValue
				, const RxTxType_t rxTxType
				, const UpdateStoreType_t updateStoreType
				, const uint16_t rate
				, SerialPortMessageManager *serialPortMessageManager
				, NamedCallback_t *namedCallback
				, SetupCallerInterface *setupCallerInterface )
				: LocalDataItem<T, COUNT>( name, initialValue, namedCallback, setupCallerInterface)
				, SerialMessageInterface<T, COUNT>(rxTxType, updateStoreType, rate, serialPortMessageManager)				
		{
			ESP_LOGI("DataItem", "DataItem Instantiated: Constructor 2");
		}
		
		DataItem( const String name
				, const T* initialValue
				, const RxTxType_t rxTxType
				, const UpdateStoreType_t updateStoreType
				, const uint16_t rate
				, SerialPortMessageManager *serialPortMessageManager
				, NamedCallback_t *namedCallback
				, SetupCallerInterface *setupCallerInterface
				, ValidStringValues_t *validStringValues )
				: LocalDataItem<T, COUNT>( name, initialValue, namedCallback, setupCallerInterface, validStringValues)
				, SerialMessageInterface<T, COUNT>(rxTxType, updateStoreType, rate, serialPortMessageManager)
		{
			ESP_LOGI("DataItem", "DataItem Instantiated: Constructor 3");
		}

		DataItem( const String name
				, const T& initialValue
				, const RxTxType_t rxTxType
				, const UpdateStoreType_t updateStoreType
				, const uint16_t rate
				, SerialPortMessageManager *serialPortMessageManager
				, NamedCallback_t *namedCallback
				, SetupCallerInterface *setupCallerInterface
				, ValidStringValues_t *validStringValues )
				: LocalDataItem<T, COUNT>( name, initialValue, namedCallback, setupCallerInterface, validStringValues)
				, SerialMessageInterface<T, COUNT>(rxTxType, updateStoreType, rate, serialPortMessageManager)
		{
			ESP_LOGI("DataItem", "DataItem Instantiated: Constructor 4");
		}

		DataItem( const String name
				, const T& initialValue
				, const RxTxType_t rxTxType
				, const UpdateStoreType_t updateStoreType
				, const uint16_t rate
				, SerialPortMessageManager *serialPortMessageManager
				, NamedCallback_t *namedCallback
				, SetupCallerInterface *setupCallerInterface
				, bool printDelimited )
				: LocalDataItem<T, COUNT>( name, initialValue, namedCallback, setupCallerInterface, printDelimited)
				, SerialMessageInterface<T, COUNT>(rxTxType, updateStoreType, rate, serialPortMessageManager)
		{
			ESP_LOGI("DataItem", "DataItem Instantiated: Constructor 5");
		}
		
		virtual ~DataItem() override
		{
			ESP_LOGI("DataItem::~DataItem()", "\"%s\": DataItem Freeing Memory", LocalDataItem<T,COUNT>::GetName().c_str());				
		}

		//SetupCalleeInterface
		virtual void Setup() override
		{
			ESP_LOGD("DataItem<T, COUNT>::Setup()", "\"%s\": Allocating Memory", LocalDataItem<T,COUNT>::GetName().c_str());
			LocalDataItem<T, COUNT>::Setup();
			SerialMessageInterface<T, COUNT>::Setup();
		}

		//SerialMessageInterface
		virtual size_t GetCount() const override
		{
			return LocalDataItem<T, COUNT>::GetCount();
		}
		virtual T* GetValuePointer() const override
		{
			return LocalDataItem<T, COUNT>::GetValuePointer();
		}

		virtual bool EqualsValue(T *object, size_t count) const override
		{
			return LocalDataItem<T, COUNT>::EqualsValue(object, count);
		}

		virtual String GetName() const override
		{
			return LocalDataItem<T, COUNT>::GetName();
		}

		virtual String GetValueAsString() const override
		{
			return LocalDataItem<T, COUNT>::GetValueAsString();
		}

		virtual DataType_t GetDataType() override
		{
			return LocalDataItem<T, COUNT>::GetDataType();
		}
		
		virtual String ConvertValueToString(const T *pvalue, size_t count) const override
		{
			return LocalDataItem<T, COUNT>::ConvertValueToString(pvalue, count);
		}

		virtual size_t ParseStringValueIntoValues(const String& stringValue, T* values) override
		{
			return LocalDataItem<T, COUNT>::ParseStringValueIntoValues(stringValue, values);
		}

		virtual bool SetValue(const T* values, size_t count) override
		{
			if(this->ConfirmValueValidity(values, count))
			{
				return this->Set_Tx_Value(values, count);
			}
			else
			{
				return false;
			}
		}

		virtual bool SetValue(const T value) override
		{
			assert(COUNT == 1);
			if(this->ConfirmValueValidity(&value, 1))
			{
				return this->Set_Tx_Value(&value, 1);
			}
			else
			{
				return false;
			}
		}

		virtual bool SetValueFromString(const String& stringValue) override
		{
			ESP_LOGI( "DataItem::SetValueFromString"
					, "Name: \"%s\" String Value: \"%s\""
					, this->GetName().c_str()
					, stringValue.c_str() );
			T values[COUNT];
			size_t parseCount = ParseStringValueIntoValues(stringValue, values);
			if(parseCount == COUNT)
			{
				ESP_LOGI("SetValueFromString", "Name: \"%s\" Setting Tx Value: \"%s\"", this->GetName().c_str(), stringValue.c_str() );
				return this->Set_Tx_Value(values, parseCount);
			}
			else
			{
				ESP_LOGE("SetValueFromString", "Name: \"%s\" Count Error!", this->GetName().c_str() );
				return false;
			}
		}

		virtual bool UpdateStore(const T *values, size_t count) override
		{
			return LocalDataItem<T,COUNT>::UpdateStore(values, count);
		}
};