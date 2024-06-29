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

#include "DataItem/DataItem.h"
#include "DataItem/PreferencesWrapper.h"

template <typename T, size_t COUNT>
class DataItemWithPreferences: public DataItem<T, COUNT>
							 , public PreferencesManager
{
	public:
		DataItemWithPreferences( const String name
							   , const T* initialValue
							   , const RxTxType_t rxTxType
							   , const UpdateStoreType_t updateStoreType
							   , const uint16_t rate
							   , IPreferences *preferences
							   , SerialPortMessageManager *serialPortMessageManager
							   , NamedCallback_t *namedCallback )
							   : DataItem<T, COUNT>( name
							   					   , initialValue
							   					   , rxTxType
							   					   , updateStoreType
							   					   , rate
							   					   , serialPortMessageManager
							   					   , namedCallback )
							   , PreferencesManager(preferences)

		{
		}
							   
		DataItemWithPreferences( const String name
							   , const T& initialValue
							   , const RxTxType_t rxTxType
							   , const UpdateStoreType_t updateStoreType
							   , const uint16_t rate
							   , IPreferences *preferences
							   , SerialPortMessageManager *serialPortMessageManager
							   , NamedCallback_t *namedCallback
							   , SetupCallerInterface *setupCallerInterface )
							   : DataItem<T, COUNT>( name
												   , initialValue
												   , rxTxType
												   , updateStoreType
												   , rate
												   , serialPortMessageManager
												   , namedCallback
												   , setupCallerInterface )
							   , PreferencesManager(preferences)
		{
		}

		DataItemWithPreferences( const String name
							   , const T* initialValue
							   , const RxTxType_t rxTxType
							   , const UpdateStoreType_t updateStoreType
							   , const uint16_t rate
							   , IPreferences *preferences
							   , SerialPortMessageManager *serialPortMessageManager
							   , NamedCallback_t *namedCallback
							   , SetupCallerInterface *setupCallerInterface
							   , const ValidStringValues_t *validStringValues )
							   : DataItem<T, COUNT>( name
							   					   , initialValue
							   					   , rxTxType
							   					   , updateStoreType
							   					   , rate
							   					   , serialPortMessageManager
							   					   , namedCallback
												   , validStringValues
												   , setupCallerInterface )
							   , PreferencesManager(preferences)
		{
		}
							   
		DataItemWithPreferences( const String name
							   , const T& initialValue
							   , const RxTxType_t rxTxType
							   , const UpdateStoreType_t updateStoreType
							   , const uint16_t rate
							   , IPreferences *preferences
							   , SerialPortMessageManager *serialPortMessageManager
							   , NamedCallback_t *namedCallback
							   , SetupCallerInterface *setupCallerInterface
							   , const ValidStringValues_t *validStringValues )
							   : DataItem<T, COUNT>( name
												   , initialValue
												   , rxTxType
												   , updateStoreType
												   , rate
												   , serialPortMessageManager
												   , namedCallback
												   , setupCallerInterface
												   , validStringValues )
							   , PreferencesManager(preferences)
		{
		}

		virtual ~DataItemWithPreferences()
		{
			ESP_LOGI("DataItemWithPreferences::~DataItemWithPreferences()", "\"%s\": Freeing Memory", this->m_Name.c_str());
		}

		void Setup()
		{
			DataItem<T, COUNT>::Setup();
			this->InitializeAndLoadPreference( this->m_Name
							   				 , this->GetInitialValueAsString()
							   				 , this->StaticSetValueFromString
							   				 , this );
			this->CreatePreferencesTimer(this->m_Name, this->GetValueAsString(), this->GetInitialValueAsString());
		}
	protected:

		bool DataItem_TX_Now()
		{
			bool result = DataItem<T, COUNT>::DataItem_TX_Now();
			if(result)
			{
				this->Update_Preference( PreferencesManager::PreferenceUpdateType::Save
									   , this->m_Name
									   , this->GetValueAsString()
									   , this->GetInitialValueAsString()
									   , this->StaticSetValueFromString
									   , this );
			}
			return result;
		}

		virtual bool NewRxValueReceived(void* Object, size_t Count) override
		{
			bool result = DataItem<T, COUNT>::NewRxValueReceived(Object, Count);
			if(result)
			{
				this->Update_Preference( PreferencesManager::PreferenceUpdateType::Save
									   , this->m_Name
									   , this->GetValueAsString()
									   , this->GetInitialValueAsString()
									   , this->StaticSetValueFromString
									   , this );
			}
			return result;
		}
};