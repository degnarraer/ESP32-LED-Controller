#ifndef DataTypes_H
#define DataTypes_H

#include "Arduino.h"

class NamedItem
{
  public:
	NamedItem(String Title): m_Title(Title){}
	virtual ~NamedItem(){}
    String GetTitle()
    { 
      return m_Title;
    }
  private:
    String m_Title;
};

struct KeyValuePair
{
  String Key;
  String Value;
};
typedef KeyValuePair KVP;

struct Wifi_Info
{
	public:
		Wifi_Info(){}
		Wifi_Info(String SSID_In, int32_t RSSI_In = 0)
		{
			assert(32 >= SSID_In.length());
			snprintf(SSID, 33, "%s", SSID_In.c_str());
			RSSI = RSSI_In;
		}
		char SSID[33] = "\0";
		int32_t RSSI = 0;
		Wifi_Info& operator=(const String &a)
		{
			assert(32 >= a.length());
			snprintf(SSID, 33, "%s", a.c_str());
			return *this;
		}
};
typedef Wifi_Info Wifi_Info_t;

struct ActiveCompatibleDevices_t
{
	std::string SSID;
	int32_t RSSI;
	unsigned long LastUpdateTime;
};
	
enum SoundState_t
{
  LastingSilenceDetected = 0,
  SilenceDetected = 1,
  Sound_Level1_Detected = 2,
  Sound_Level2_Detected = 3,
  Sound_Level3_Detected = 4,
  Sound_Level4_Detected = 5,
  Sound_Level5_Detected = 6,
  Sound_Level6_Detected = 7,
  Sound_Level7_Detected = 8,
  Sound_Level8_Detected = 9,
  Sound_Level9_Detected = 10,
  Sound_Level10_Detected = 11,
  Sound_Level11_Detected = 12,
};

enum Transciever_t
{
	Transciever_None,
	Transciever_TX,
	Transciever_RX,
	Transciever_TXRX
};

struct __attribute__((packed)) Frame_t {
  int16_t channel1;
  int16_t channel2;
};

enum BitLength_t
{
  BitLength_32,
  BitLength_16,
  BitLength_8,
};

enum DataType_t
{
  DataType_bool_t,
  DataType_Int8_t,
  DataType_Int16_t,
  DataType_Int32_t,
  DataType_Uint8_t,
  DataType_Uint16_t,
  DataType_Uint32_t,
  DataType_String_t,
  DataType_Wifi_Info_t,
  DataType_Float_t,
  DataType_Double_t,
  DataType_ProcessedSoundData_t,
  DataType_MaxBandSoundData_t,
  DataType_Frame_t,
  DataType_ProcessedSoundFrame_t,
  DataType_SoundState_t,
  DataType_ConnectionStatus_t,
  DataType_Undef,
};

static const char* DataTypeStrings[] =
{
  "bool_t",
  "Int8_t",
  "Int16_t",
  "Int32_t",
  "Uint8_t",
  "Uint16_t",
  "Uint32_t",
  "String_t",
  "Wifi_Info_t",
  "Float_t",
  "Double_t",
  "ProcessedSoundData_t",
  "MaxBandSoundData_t",
  "Frame_t",
  "ProcessedSoundFrame_t",
  "SoundState_t",
  "ConnectionStatus_t",
  "Undefined_t"
};

struct DataItemConfig_t
{
  String Name;
  DataType_t DataType;
  size_t Count;
  Transciever_t TransceiverConfig;
  size_t QueueCount;
};

enum ConnectionStatus_t
{
	Disconnected = 0,
	Waiting = 1,
	Searching = 2,
	Pairing = 3,
	Paired = 4
};

struct DataItem_t
{
	String Name;
	QueueHandle_t QueueHandle_RX = NULL;
	QueueHandle_t QueueHandle_TX = NULL;
	Transciever_t TransceiverConfig;
	DataType_t DataType;
	size_t Count = 0;
	size_t TotalByteCount = 0;
	bool DataPushHasErrored = false;
};

struct ProcessedSoundData_t
{
	float NormalizedPower;
	int32_t Minimum;
	int32_t Maximum;
};

struct ProcessedSoundFrame_t
{
	ProcessedSoundData_t Channel1;
	ProcessedSoundData_t Channel2;
};

struct MaxBandSoundData_t
{
	float MaxBandNormalizedPower;
	int16_t MaxBandIndex;
	int16_t TotalBands;
};


#endif
