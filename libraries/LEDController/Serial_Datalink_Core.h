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

#ifndef SerialDataLink_H
#define SerialDataLink_H
#define QUEUE_SIZE 10

#include <HardwareSerial.h>
#include <Arduino.h>
#include <DataTypes.h>
#include "Streaming.h"

class SerialDataLinkCore: public NamedItem
{
  public:
    SerialDataLinkCore(String Title);
    virtual ~SerialDataLinkCore();
	virtual DataItemConfig_t* GetConfig() = 0;
	virtual size_t GetConfigCount() = 0;
	
    void Setup();
    void CheckForNewSerialData();
    void ProcessEventQueue();
	QueueHandle_t GetQueueHandleForDataItem(String Name);
  private:
  DataItem_t* m_DataItem = NULL;
  size_t m_DataItemCount = 0;
  HardwareSerial &hSerial = Serial2;
  String m_InboundStringData = "";
  
  template <class T>
  void EncodeAndTransmitData(String Name, void* Object, size_t Count)
  {
	  String Header = "<" + Name + ">";
	  String DataToSend = "";
	  DataToSend += Header;
	  for(int i = 0; i < Count; ++i)
	  {
		  T item = *((T*)Object + i);
		  DataToSend += String(item);
		  if(i < Count) DataToSend += ",";
	  }
	  DataToSend += "\n";
	  hSerial.print(DataToSend);
  }
};

#endif
