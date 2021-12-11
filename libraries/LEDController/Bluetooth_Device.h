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

#ifndef Bluetooth_Device_H
#define Bluetooth_Device_H 

#include <Arduino.h>
#include <DataTypes.h>
#include <Helpers.h>
#include "Streaming.h"
#include <BluetoothA2DPSink.h>

class Bluetooth_Sink: public NamedItem
					  , public CommonUtils
{
  public:
    Bluetooth_Sink( String Title
				  , i2s_port_t i2S_PORT
				  , int BufferCount
				  , int BufferSize
				  , int SerialClockPin
				  , int WordSelectPin
				  , int SerialDataInPin
				  , int SerialDataOutPin );			
    virtual ~Bluetooth_Sink();
	void Setup()
	{
		InstallDevice();
	}
	void InstallDevice()
	{
		Serial << "Configuring Bluetooth\n";
		m_BTSink.set_stream_reader(read_data_stream);
		m_BTSink.set_on_data_received(data_received_callback);

		static i2s_config_t i2s_config = {
		  .mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_TX),
		  .sample_rate = 44100, // updated automatically by A2DP
		  .bits_per_sample = (i2s_bits_per_sample_t)I2S_BITS_PER_SAMPLE_32BIT,
		  .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
		  .communication_format = (i2s_comm_format_t) (I2S_COMM_FORMAT_STAND_I2S),
		  .intr_alloc_flags = 0, // default interrupt priority
		  .dma_buf_count = m_BufferCount,
		  .dma_buf_len = m_BufferSize,
		  .use_apll = true,
		  .tx_desc_auto_clear = true // avoiding noise in case of data unavailability
		};
		i2s_pin_config_t my_pin_config = 
		{
			.bck_io_num = m_SerialClockPin,
			.ws_io_num = m_WordSelectPin,
			.data_out_num = m_SerialDataOutPin,
			.data_in_num = m_SerialDataInPin
		};
		m_BTSink.set_pin_config(my_pin_config);
		m_BTSink.set_i2s_config(i2s_config);
		m_BTSink.set_i2s_port(m_i2S_PORT);
	}
	void StartDevice()
	{
		m_BTSink.start("Massive Cock");
	}
	void StopDevice()
	{
		m_BTSink.stop();
	}
  private:
	BluetoothA2DPSink m_BTSink;
	i2s_port_t m_i2S_PORT;
	const int m_BufferCount;
	int m_BufferSize;
	int m_SerialClockPin;
	int m_WordSelectPin;
	int m_SerialDataInPin;
	int m_SerialDataOutPin;
	
	static void data_received_callback() 
	{
	}

	static void read_data_stream(const uint8_t *data, uint32_t length)
	{
		
	}

};

#endif