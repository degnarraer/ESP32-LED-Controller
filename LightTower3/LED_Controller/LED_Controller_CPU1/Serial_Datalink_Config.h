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

#ifndef SerialDataLinkConfig_H
#define SerialDataLinkConfig_H

#include <Serial_Datalink_Core.h>
#include <DataTypes.h>
#include "Manager.h"


class Manager;
class SerialDataLink: public NamedItem
                    , public SerialDataLinkCore
{
  public:
    SerialDataLink(String Title): NamedItem(Title)
                                , SerialDataLinkCore(Title) {}
    virtual ~SerialDataLink(){}
    DataItemConfig_t* GetConfig() { return ItemConfig; }
    size_t GetConfigCount() { return m_ConfigCount; }
  private:
    
    static const size_t m_ConfigCount = 1;
    DataItemConfig_t ItemConfig[m_ConfigCount]
    {
      { "FFT_Bin_Data", DataType_Int16_t, 1 }
    };

};


#endif