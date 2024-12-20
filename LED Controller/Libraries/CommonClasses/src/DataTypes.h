#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <type_traits>
#include "Streaming.h"

#define BT_NAME_LENGTH 50
#define BT_ADDRESS_LENGTH 18

#define THREAD_PRIORITY_RT configMAX_PRIORITIES - 1
#define THREAD_PRIORITY_HIGH configMAX_PRIORITIES - 5
#define THREAD_PRIORITY_MEDIUM configMAX_PRIORITIES - 10
#define THREAD_PRIORITY_LOW configMAX_PRIORITIES - 15
#define THREAD_PRIORITY_IDLE configMAX_PRIORITIES - 20

#define ENCODE_OBJECT_DIVIDER "|"
#define ENCODE_VALUE_DIVIDER ","


enum DeviceState
{
Installed,
Uninstalled,
Running,
Stopped
};
typedef DeviceState DeviceState_t;

struct UpdateStatus
{
    bool ValueChanged = false;
    bool ValidValue = false;
    bool UpdateAllowed = false;
    bool UpdateSuccessful = false;

    bool operator==(const UpdateStatus& other) const
    {
        return ( this->ValueChanged == other.ValueChanged &&
                 this->ValidValue == other.ValidValue &&
                 this->UpdateAllowed == other.UpdateAllowed &&
                 this->UpdateSuccessful == other.UpdateSuccessful );
    }

    UpdateStatus& operator|=(const UpdateStatus& other)
    {
        this->ValueChanged |= other.ValueChanged;
        this->ValidValue |= other.ValidValue;
        this->UpdateAllowed |= other.UpdateAllowed;
        this->UpdateSuccessful |= other.UpdateSuccessful;
        return *this;
    }
};
typedef UpdateStatus UpdateStatus_t;

template <typename T>
struct TemplateObjectWithChangeCount
{
    size_t ChangeCount;
    T Object;
};

template <typename T>
using TemplateObjectWithChangeCount_t = TemplateObjectWithChangeCount<T>;

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

template <typename T>
struct MyAllocator
{
	using value_type = T;

	T* allocate(std::size_t n)
	{
		return static_cast<T*>(malloc(sizeof(T) * n));
	}

	void deallocate(T* p, std::size_t n)
	{
		free(p);
	}
};

struct KeyValueTuple
{
  String Key;
  String Value1;
  String Value2;
};
typedef KeyValueTuple KVT;

struct BT_Device_Info
{
	public:
		char name[BT_NAME_LENGTH+1] = "\0";
		char address[BT_ADDRESS_LENGTH+1] = "\0";
		int32_t rssi = 0;
        
		BT_Device_Info(){}
		BT_Device_Info(const char* name_In, int32_t rssi_In = 0)
		{
            strncpy(name, name_In, BT_NAME_LENGTH);
            name[BT_NAME_LENGTH] = '\0';
			rssi = rssi_In;
		}
		BT_Device_Info(const char* name_In, const char* address_In, int32_t rssi_In = 0)
		{
            strncpy(name, name_In, BT_NAME_LENGTH);
            name[BT_NAME_LENGTH] = '\0';
    
            strncpy(address, address_In, BT_ADDRESS_LENGTH);
            address[BT_ADDRESS_LENGTH] = '\0';
			rssi = rssi_In;
		}
		bool operator==(const BT_Device_Info& other) const
		{
			return ( strcmp(this->name, other.name) == 0 &&
				     strcmp(this->address, other.address) == 0 );
		}
		bool operator!=(const BT_Device_Info& other) const
		{
			return !(*this == other);
		}
};
typedef BT_Device_Info BT_Device_Info_t;

struct ActiveBluetoothDevice_t
{
public:
    char name[BT_NAME_LENGTH+1];
    char address[BT_ADDRESS_LENGTH+1];
    int32_t rssi;
    unsigned long lastUpdateTime;
    uint32_t timeSinceUpdate;
    
    ActiveBluetoothDevice_t() : rssi(0), lastUpdateTime(0), timeSinceUpdate(0)
    {
        name[0] = '\0';
        address[0] = '\0';
    }
    
	ActiveBluetoothDevice_t(const String &str)
	{
		*this =  fromString(str.c_str());
	}

    ActiveBluetoothDevice_t(const String& name_In, const String& address_In)
    {
        if (BT_NAME_LENGTH < name_In.length())
        {
            Serial << "Bad SSID: " << name_In.c_str() << " | " << name_In.length() << "\n";
            assert(BT_NAME_LENGTH >= name_In.length());
        }
        strncpy(this->name, name_In.c_str(), sizeof(this->name));
        this->name[sizeof(this->name)] = '\0';

        if (BT_ADDRESS_LENGTH < address_In.length())
        {
            Serial << "Bad address: " << address_In.c_str() << " | " << address_In.length() << "\n";
            assert(BT_ADDRESS_LENGTH >= address_In.length());
        }
        strncpy(this->address, address_In.c_str(), sizeof(this->address));
        this->address[sizeof(this->address)] = '\0';
    }

    ActiveBluetoothDevice_t(const String& name_In, const String& address_In, int32_t rssi_in, unsigned long lastUpdateTime_in, uint32_t timeSinceUpdate_in)
    {
        if (BT_NAME_LENGTH < name_In.length())
        {
            Serial << "Bad SSID: " << name_In.c_str() << " | " << name_In.length() << "\n";
            assert(BT_NAME_LENGTH >= name_In.length());
        }
        strncpy(this->name, name_In.c_str(), sizeof(this->name));
        this->name[sizeof(this->name)] = '\0';

        if (BT_ADDRESS_LENGTH < address_In.length())
        {
            Serial << "Bad address: " << address_In.c_str() << " | " << address_In.length() << "\n";
            assert(BT_ADDRESS_LENGTH >= address_In.length());
        }
        strncpy(this->address, address_In.c_str(), sizeof(this->address));
        this->address[sizeof(this->address)] = '\0';

        rssi = rssi_in;
        lastUpdateTime = lastUpdateTime_in;
        timeSinceUpdate = timeSinceUpdate_in;
    }
    
    ActiveBluetoothDevice_t(const BT_Device_Info& deviceInfo)
    {
        strncpy(this->name, deviceInfo.name, sizeof(this->name));
        this->name[sizeof(this->name)] = '\0';

        strncpy(this->address, deviceInfo.address, sizeof(this->address));
        this->address[sizeof(this->address)] = '\0';

        this->rssi = deviceInfo.rssi;
        this->lastUpdateTime = millis();
        this->timeSinceUpdate = 0;
    }

    ActiveBluetoothDevice_t& operator=(const ActiveBluetoothDevice_t& other)
    {
        if (this != &other)
        {
            strncpy(this->name, other.name, sizeof(this->name));
            this->name[sizeof(this->name)] = '\0';

            strncpy(this->address, other.address, sizeof(this->address));
            this->address[sizeof(this->address)] = '\0';

            rssi = other.rssi;
            lastUpdateTime = other.lastUpdateTime;
            timeSinceUpdate = other.timeSinceUpdate;
        }
        return *this;
    }

    ActiveBluetoothDevice_t& operator=(const BT_Device_Info& other)
    {
        strncpy(this->name, other.name, sizeof(this->name));
        this->name[sizeof(this->name)] = '\0';

        strncpy(this->address, other.address, sizeof(this->address));
        this->address[sizeof(this->address)] = '\0';

        rssi = other.rssi;
        lastUpdateTime = millis();
        timeSinceUpdate = 0;

        return *this;
    }

    bool operator==(const ActiveBluetoothDevice_t& other) const
    {
        return ( strcmp(this->name, other.name) == 0 &&
                 strcmp(this->address, other.address ) == 0 );
    }
    
    bool operator==(const BT_Device_Info& other) const
    {
        return ( strcmp(this->name, other.name) == 0 &&
                 strcmp(this->address, other.address ) == 0 );
    }

    bool operator!=(const ActiveBluetoothDevice_t& other) const
    {
        return !(*this == other);
    }

	operator String() const
	{
		return toString();
	}

    String toString() const
    {
        return String(name) + ENCODE_VALUE_DIVIDER + String(address) + ENCODE_VALUE_DIVIDER + String(rssi) + ENCODE_VALUE_DIVIDER + String(lastUpdateTime) + ENCODE_VALUE_DIVIDER + String(timeSinceUpdate);
    }

    static ActiveBluetoothDevice_t fromString(const std::string &str)
    {
        const std::string delimiter = ENCODE_VALUE_DIVIDER; // Assuming ENCODE_VALUE_DIVIDER is a string or char
        size_t delimiterIndex = str.find(delimiter);

        if (delimiterIndex == std::string::npos)
        {
            return ActiveBluetoothDevice_t();  // Return default if delimiter not found
        }

        // Extract name
        std::string name = str.substr(0, delimiterIndex);

        // Find the next delimiter for the address
        size_t nextDelimiterIndex = str.find(delimiter, delimiterIndex + delimiter.length());
        if (nextDelimiterIndex == std::string::npos)
        {
            return ActiveBluetoothDevice_t();
        }

        // Extract address
        std::string address = str.substr(delimiterIndex + delimiter.length(), nextDelimiterIndex - delimiterIndex - delimiter.length());

        // Find the next delimiter for RSSI
        size_t nextDelimiterIndex2 = str.find(delimiter, nextDelimiterIndex + delimiter.length());
        if (nextDelimiterIndex2 == std::string::npos)
        {
            return ActiveBluetoothDevice_t();
        }

        // Extract RSSI
        std::string rssiStr = str.substr(nextDelimiterIndex + delimiter.length(), nextDelimiterIndex2 - nextDelimiterIndex - delimiter.length());
        int rssi = std::stoi(rssiStr);

        // Find the next delimiter for lastUpdateTime
        size_t nextDelimiterIndex3 = str.find(delimiter, nextDelimiterIndex2 + delimiter.length());
        if (nextDelimiterIndex3 == std::string::npos)
        {
            return ActiveBluetoothDevice_t();
        }

        // Extract lastUpdateTime
        std::string lastUpdateTimeStr = str.substr(nextDelimiterIndex2 + delimiter.length(), nextDelimiterIndex3 - nextDelimiterIndex2 - delimiter.length());
        unsigned long lastUpdateTime = std::stoul(lastUpdateTimeStr);

        // Extract timeSinceUpdate
        std::string timeSinceUpdateStr = str.substr(nextDelimiterIndex3 + delimiter.length());
        uint32_t timeSinceUpdate = std::stoul(timeSinceUpdateStr);

        // Create and return the ActiveBluetoothDevice_t object
        return ActiveBluetoothDevice_t(name.c_str(), address.c_str(), rssi, lastUpdateTime, timeSinceUpdate);
    }

	
    friend std::istream& operator>>(std::istream& is, ActiveBluetoothDevice_t& device) {
        std::string str;
        std::getline(is, str);
        device = ActiveBluetoothDevice_t::fromString(str);
        return is;
    }

    friend std::ostream& operator<<(std::ostream& os, const ActiveBluetoothDevice_t& device) {
        os << device.toString().c_str();
        return os;
    }
};

struct BT_Device_Info_With_Time_Since_Update
{
	public:
		char name[BT_NAME_LENGTH] = "\0";
		char address[BT_ADDRESS_LENGTH] = "\0";
		int32_t rssi = 0;
		uint32_t timeSinceUpdate = 0;

		BT_Device_Info_With_Time_Since_Update(){}
		BT_Device_Info_With_Time_Since_Update(const char* name_In, const char* address_In, int32_t rssi_In, uint32_t timeSinceUdpate_in)
		{
            strncpy(name, name_In, BT_NAME_LENGTH);
            name[BT_NAME_LENGTH] = '\0';
    
            strncpy(address, address_In, BT_ADDRESS_LENGTH);
            address[BT_ADDRESS_LENGTH] = '\0';
			rssi = rssi_In;
			timeSinceUpdate = timeSinceUdpate_in;
		}

        BT_Device_Info_With_Time_Since_Update(const ActiveBluetoothDevice_t device_in, uint32_t timeSinceUpdate_in)
        {
            strncpy(name, device_in.name, BT_NAME_LENGTH);
            name[BT_NAME_LENGTH] = '\0';
    
            strncpy(address, device_in.address, BT_ADDRESS_LENGTH);
            address[BT_ADDRESS_LENGTH] = '\0';
			rssi = device_in.rssi;
            timeSinceUpdate = timeSinceUpdate_in;
        }
        
		BT_Device_Info_With_Time_Since_Update& operator=(const BT_Device_Info_With_Time_Since_Update& other)
		{
			strncpy(this->name, other.name, sizeof(this->name));
			this->name[sizeof(this->name)] = '\0';

			strncpy(this->address, other.address, sizeof(this->address));
			this->address[sizeof(this->address)] = '\0';

			this->rssi = other.rssi;
			this->timeSinceUpdate = other.timeSinceUpdate;

			return *this;
		}
		bool operator==(const BT_Device_Info_With_Time_Since_Update& other) const
		{
			return ( strcmp(this->name, other.name) == 0 &&
				     strcmp(this->address, other.address) == 0 );
		}
		bool operator!=(const BT_Device_Info_With_Time_Since_Update& other) const
		{
			return !(*this == other);
		}

		operator String() const
        {
            return toString();
        }

        String toString() const
        {
            return String(name) + ENCODE_VALUE_DIVIDER + String(address) + ENCODE_VALUE_DIVIDER + String(rssi) + ENCODE_VALUE_DIVIDER + String(timeSinceUpdate);
        }

		static BT_Device_Info_With_Time_Since_Update fromString(const std::string &str)
        {
            const std::string delimiter = ENCODE_VALUE_DIVIDER; // Assuming ENCODE_VALUE_DIVIDER is a string or char
            size_t delimiterIndex = str.find(delimiter);
            
            if (delimiterIndex == std::string::npos)
            {
                return BT_Device_Info_With_Time_Since_Update();  // Return default if delimiter not found
            }

            // Extract name
            std::string name = str.substr(0, delimiterIndex);

            // Find the next delimiter for the address
            size_t nextDelimiterIndex = str.find(delimiter, delimiterIndex + delimiter.length());
            if (nextDelimiterIndex == std::string::npos)
            {
                return BT_Device_Info_With_Time_Since_Update();
            }

            // Extract address
            std::string address = str.substr(delimiterIndex + delimiter.length(), nextDelimiterIndex - delimiterIndex - delimiter.length());

            // Find the next delimiter for RSSI
            size_t nextDelimiterIndex2 = str.find(delimiter, nextDelimiterIndex + delimiter.length());
            if (nextDelimiterIndex2 == std::string::npos)
            {
                return BT_Device_Info_With_Time_Since_Update();
            }

            // Extract RSSI
            std::string rssiStr = str.substr(nextDelimiterIndex + delimiter.length(), nextDelimiterIndex2 - nextDelimiterIndex - delimiter.length());
            int32_t rssi = std::stoi(rssiStr);

            // Extract timeSinceUpdate (assuming it's the last part of the string)
            std::string timeSinceUpdateStr = str.substr(nextDelimiterIndex2 + delimiter.length());
            uint32_t timeSinceUpdate = std::stoul(timeSinceUpdateStr);

            // Create and return the BT_Device_Info_With_Time_Since_Update object without lastUpdateTime
            return BT_Device_Info_With_Time_Since_Update(name.c_str(), address.c_str(), rssi, timeSinceUpdate);
        }


		
		friend std::istream& operator>>(std::istream& is, BT_Device_Info_With_Time_Since_Update& device) {
			std::string str;
			std::getline(is, str);
			device = BT_Device_Info_With_Time_Since_Update::fromString(str);
			return is;
		}

		
		friend std::ostream& operator<<(std::ostream& os, const BT_Device_Info_With_Time_Since_Update& device) {
			os << device.toString().c_str();
			return os;
		}
};
typedef BT_Device_Info_With_Time_Since_Update BT_Device_Info_With_Time_Since_Update_t;

struct  BluetoothDevice_t
{
	public:
		char name[BT_NAME_LENGTH] = "\0";
		char address[BT_ADDRESS_LENGTH] = "\0";
		
        BluetoothDevice_t(){}

		BluetoothDevice_t(const String &str)
		{
			*this = fromString(str.c_str());
		}

		BluetoothDevice_t(const char* name_In, const char* address_In)
		{
            strncpy(name, name_In, BT_NAME_LENGTH);
            name[BT_NAME_LENGTH] = '\0';
    
            strncpy(address, address_In, BT_ADDRESS_LENGTH);
            address[BT_ADDRESS_LENGTH] = '\0';
		}

		BluetoothDevice_t& operator=(const BluetoothDevice_t& other)
		{
			strncpy(this->name, other.name, sizeof(this->name));
			this->name[sizeof(this->name)] = '\0';

			strncpy(this->address, other.address, sizeof(this->address));
			this->address[sizeof(this->address)] = '\0';
			return *this;
		}

		bool operator==(const BluetoothDevice_t& other) const
		{
			return ( strcmp(this->name, other.name) == 0 &&
				     strcmp(this->address, other.address) == 0 );
		}

		bool operator!=(const BluetoothDevice_t& other) const
		{
			return !(*this == other);
		}

		operator String() const
        {
            return toString();
        }

        String toString() const
        {
            return String(name) + ENCODE_VALUE_DIVIDER + String(address);
        }

		static BluetoothDevice_t fromString(const std::string &str)
        {
            const std::string delimiter = ENCODE_VALUE_DIVIDER;
            size_t delimiterIndex = str.find(delimiter);

            if (delimiterIndex == std::string::npos)
            {
                return BluetoothDevice_t();
            }

            std::string name = str.substr(0, delimiterIndex);
            std::string address = str.substr(delimiterIndex + delimiter.length());

            return BluetoothDevice_t(name.c_str(), address.c_str());
        }

		friend std::istream& operator>>(std::istream& is, BluetoothDevice_t& device) {
			std::string str;
			std::getline(is, str);
			device = BluetoothDevice_t::fromString(str);
			return is;
		}

		friend std::ostream& operator<<(std::ostream& os, const BluetoothDevice_t& device) {
			os << device.toString().c_str();
			return os;
		}
};

class SoundInputSource
{
    public:
        enum Value
        {
            OFF = 0,
            Microphone = 1,
            Bluetooth = 2,
            Count = 3
        };
 
        static String ToString(const Value &source)
        {
            switch (source)
            {
                case OFF: return "OFF";
                case Microphone: return "Microphone";
                case Bluetooth: return "Bluetooth";
                default: return "Unknown";
            }
        }

        static Value FromString(const String& str)
        {
            if (str.equals("OFF")) return OFF;
            if (str.equals("Microphone")) return Microphone;
            if (str.equals("Bluetooth")) return Bluetooth;
            return OFF;
        }

        
        friend std::ostream& operator<<(std::ostream& os, const SoundInputSource::Value& value) {
            String result = SoundInputSource::ToString(value);
            ESP_LOGD("SoundInputSource", "ostream input value: \"%i\" Converted to: \"%s\"", value, result.c_str());
            os << result.c_str();
            return os;
        }
        
        
        friend std::istream& operator>>(std::istream& is, SoundInputSource::Value& value) 
        {
            std::string token;
            is >> token;
            value = SoundInputSource::FromString(token.c_str());
            ESP_LOGD("SoundInputSource", "istream input value: \"%s\" Converted to: \"%i\"", token.c_str(), value);
            return is;
        }
};
typedef SoundInputSource::Value SoundInputSource_t;

class SoundOutputSource
{
    public:
        enum Value
        {
            OFF = 0,
            Bluetooth = 1,
            Count = 2
        };

        
        static String ToString(Value source)
        {
            switch (source)
            {
                case OFF:         return "OFF";
                case Bluetooth:   return "Bluetooth";
                default:          return "Unknown";
            }
        }

        static Value FromString(const String& str)
        {
            if (str.equals("OFF")) return OFF;
            if (str.equals("Bluetooth")) return Bluetooth;
            return OFF;
        }

        
        friend std::ostream& operator<<(std::ostream& os, const SoundOutputSource::Value& value) {
            String result = SoundOutputSource::ToString(value);
            ESP_LOGD("SoundOutputSource", "ostream input value: \"%i\" Converted to: \"%s\"", value, result.c_str());
            os << result.c_str();
            return os;
        }
        
        
        friend std::istream& operator>>(std::istream& is, SoundOutputSource::Value& value) 
        {
            std::string token;
            is >> token;
            value = SoundOutputSource::FromString(token.c_str());
            ESP_LOGD("SoundOutputSource", "istream input value: \"%s\" Converted to: \"%i\"", token.c_str(), value);
            return is;
        }
};
typedef SoundOutputSource::Value SoundOutputSource_t;

class Mute_State
{
    public:
        enum Value
        {
            Mute_State_Un_Muted = 0,
            Mute_State_Muted = 1
        };

        
        static String ToString(Value state)
        {
            switch (state)
            {
                case Mute_State_Un_Muted: return "Mute_State_Un_Muted";
                case Mute_State_Muted: return "Mute_State_Muted";
                default: return "Unknown";
            }
        }

        
        static Value FromString(const String& str)
        {
            if (str == "Mute_State_Un_Muted") return Mute_State_Un_Muted;
            if (str == "Mute_State_Muted") return Mute_State_Muted;
            return Mute_State_Un_Muted;
        }

        
        friend std::istream& operator>>(std::istream& is, Mute_State::Value& value) 
        {
            std::string token;
            is >> token;
            value = Mute_State::FromString(token.c_str());
            return is;
        }
        
        
        friend std::ostream& operator<<(std::ostream& os, const Mute_State::Value& value) {
            os << Mute_State::ToString(value).c_str();
            return os;
        }
};
typedef Mute_State::Value Mute_State_t;

class Bluetooth_Discovery_Mode
{
    public:
        enum Value
        {
            Discovery_Mode_Started = 0,
            Discovery_Mode_Stopped = 1,
            Discovery_Mode_Unknown = 2
        };

        static String ToString(Value state)
        {
            switch (state)
            {
                case Discovery_Mode_Started: return "Started";
                case Discovery_Mode_Stopped: return "Stopped";
                case Discovery_Mode_Unknown: return "Unknown";
                default: return "Discovery_Mode_Unknown";
            }
        }

        static Value FromString(const String& str)
        {
            if (str == "Started") return Discovery_Mode_Started;
            if (str == "Stopped") return Discovery_Mode_Stopped;
            return Discovery_Mode_Unknown;
        }

        friend std::istream& operator>>(std::istream& is, Bluetooth_Discovery_Mode::Value& value) 
        {
            std::string token;
            is >> token;
            value = Bluetooth_Discovery_Mode::FromString(token.c_str());
            return is;
        }
        
        friend std::ostream& operator<<(std::ostream& os, const Bluetooth_Discovery_Mode::Value& value) {
            os << Bluetooth_Discovery_Mode::ToString(value).c_str();
            return os;
        }
};
typedef Bluetooth_Discovery_Mode::Value Bluetooth_Discovery_Mode_t;

class SoundState
{
    public:
        enum Value
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
            Sound_Level11_Detected = 12
        };

        
        static String ToString(Value state)
        {
            switch (state)
            {
                case LastingSilenceDetected:    return "LastingSilenceDetected";
                case SilenceDetected:           return "SilenceDetected";
                case Sound_Level1_Detected:     return "Sound_Level1_Detected";
                case Sound_Level2_Detected:     return "Sound_Level2_Detected";
                case Sound_Level3_Detected:     return "Sound_Level3_Detected";
                case Sound_Level4_Detected:     return "Sound_Level4_Detected";
                case Sound_Level5_Detected:     return "Sound_Level5_Detected";
                case Sound_Level6_Detected:     return "Sound_Level6_Detected";
                case Sound_Level7_Detected:     return "Sound_Level7_Detected";
                case Sound_Level8_Detected:     return "Sound_Level8_Detected";
                case Sound_Level9_Detected:     return "Sound_Level9_Detected";
                case Sound_Level10_Detected:    return "Sound_Level10_Detected";
                case Sound_Level11_Detected:    return "Sound_Level11_Detected";
                default:                        return "Unknown";
            }
        }

        
        static Value FromString(const String& str)
        {
            if (str == "LastingSilenceDetected")    return LastingSilenceDetected;
            if (str == "SilenceDetected")           return SilenceDetected;
            if (str == "Sound_Level1_Detected")     return Sound_Level1_Detected;
            if (str == "Sound_Level2_Detected")     return Sound_Level2_Detected;
            if (str == "Sound_Level3_Detected")     return Sound_Level3_Detected;
            if (str == "Sound_Level4_Detected")     return Sound_Level4_Detected;
            if (str == "Sound_Level5_Detected")     return Sound_Level5_Detected;
            if (str == "Sound_Level6_Detected")     return Sound_Level6_Detected;
            if (str == "Sound_Level7_Detected")     return Sound_Level7_Detected;
            if (str == "Sound_Level8_Detected")     return Sound_Level8_Detected;
            if (str == "Sound_Level9_Detected")     return Sound_Level9_Detected;
            if (str == "Sound_Level10_Detected")    return Sound_Level10_Detected;
            if (str == "Sound_Level11_Detected")    return Sound_Level11_Detected;
            return  LastingSilenceDetected;
        }

        
        friend std::istream& operator>>(std::istream& is, SoundState::Value& value) 
        {
            std::string token;
            is >> token;
            value = SoundState::FromString(token.c_str());
            return is;
        }
        
        
        friend std::ostream& operator<<(std::ostream& os, const SoundState::Value& value) {
            os << SoundState::ToString(value).c_str();
            return os;
        }
};
typedef SoundState::Value SoundState_t;

class Transciever
{
    public:
        enum Value
        {
            Transciever_None = 0,
            Transciever_TX = 1,
            Transciever_RX = 2,
            Transciever_TXRX = 3
        };

        
        static String ToString(Value transciever)
        {
            switch (transciever)
            {
                case Transciever_None: return "Transciever_None";
                case Transciever_TX: return "Transciever_TX";
                case Transciever_RX: return "Transciever_RX";
                case Transciever_TXRX: return "Transciever_TXRX";
                default: return "Unknown";
            }
        }

        
        static Value FromString(const String& str)
        {
            if (str == "Transciever_None") return Transciever_None;
            if (str == "Transciever_TX") return Transciever_TX;
            if (str == "Transciever_RX") return Transciever_RX;
            if (str == "Transciever_TXRX") return Transciever_TXRX;
            
            return Transciever_None; // Default or error value
        }

        
        friend std::istream& operator>>(std::istream& is, Transciever::Value& value) 
        {
            std::string token;
            is >> token;
            value = Transciever::FromString(token.c_str());
            return is;
        }
        
        
        friend std::ostream& operator<<(std::ostream& os, const Transciever::Value& value) {
            os << Transciever::ToString(value).c_str();
            return os;
        }

};
typedef Transciever::Value Transciever_t;

struct __attribute__((packed)) Frame_t {
  int16_t channel1;
  int16_t channel2;
  bool operator==(const Frame_t& other) const {
        return channel1 == other.channel1 && channel2 == other.channel2;
    }
};

enum BitLength_t
{
  BitLength_32,
  BitLength_16,
  BitLength_8,
};

enum DataType_t
{
  DataType_Bool_t,
  DataType_Int8_t,
  DataType_Int16_t,
  DataType_Int32_t,
  DataType_Uint8_t,
  DataType_Uint16_t,
  DataType_Uint32_t,
  DataType_Char_t,
  DataType_String_t,
  DataType_BT_Device_Info_t,
  DataType_BT_Device_Info_With_Time_Since_Update_t,
  DataType_BluetoothDevice_t,
  DataType_ActiveBluetoothDevice_t,
  DataType_Float_t,
  DataType_Double_t,
  DataType_ProcessedSoundData_t,
  DataType_MaxBandSoundData_t,
  DataType_Frame_t,
  DataType_ProcessedSoundFrame_t,
  DataType_SoundState_t,
  DataType_ConnectionStatus_t,
  DataType_SoundInputSource_t,
  DataType_SoundOutputSource_t,
  DataType_Bluetooth_Discovery_Mode_t,
  DataType_Undef,
};

static const char* DataTypeStrings[] =
{
  "Bool_t",
  "Int8_t",
  "Int16_t",
  "Int32_t",
  "Uint8_t",
  "Uint16_t",
  "Uint32_t",
  "Char_t",
  "String_t",
  "BT_Device_Info_t",
  "BT_Device_Info_With_Time_Since_Update_t",
  "BluetoothDevice_t",
  "ActiveBluetoothDevice_t",
  "Float_t",
  "Double_t",
  "ProcessedSoundData_t",
  "MaxBandSoundData_t",
  "Frame_t",
  "ProcessedSoundFrame_t",
  "SoundState_t",
  "ConnectionStatus_t",
  "SoundInputSource_t",
  "SoundOutputSource_t",
  "Bluetooth_Discovery_Mode_t",
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

class Wifi_Mode
{
public:
    enum Value
    {
        Station = 0,
        AccessPoint = 1,
        Unknown = 2
    };

    
    static String ToString(Value status)
    {
        switch (status)
        {
            case Station: return "Station";
            case AccessPoint: return "AccessPoint";
            default: return "Unknown";
        }
    }

    
    static Value FromString(const String& str)
    {
        if (str == "Station") return Station;
        if (str == "AccessPoint") return AccessPoint;
        return Unknown; // Default or error value
    }

    
	friend std::istream& operator>>(std::istream& is, Wifi_Mode::Value& value) 
	{
		std::string token;
		is >> token;
		value = Wifi_Mode::FromString(token.c_str());
		return is;
	}
    
    
    friend std::ostream& operator<<(std::ostream& os, const Wifi_Mode::Value& value) {
        ESP_LOGD("operator<<", "Wifi Mode to String: \"%s\"", Wifi_Mode::ToString(value).c_str());
        os << Wifi_Mode::ToString(value).c_str();
        return os;
    }
    
};
typedef Wifi_Mode::Value Wifi_Mode_t;

class ConnectionStatus
{
public:
    enum Value
    {
        Disconnected = 0,
        Connecting = 1,
        Connected = 2,
        Disconnecting = 3,
        Unknown = 4
    };

    
    static String ToString(Value status)
    {
        switch (status)
        {
            case Disconnected: return "Disconnected";
            case Connecting: return "Connecting";
            case Connected: return "Connected";
            case Disconnecting: return "Disconnecting";
            case Unknown: return "Unknown";
            default: return "Unknown";
        }
    }

    
    static Value FromString(const String& str)
    {
        if (str == "Disconnected") return Disconnected;
        if (str == "Connecting") return Connecting;
        if (str == "Connected") return Connected;
        if (str == "Disconnecting") return Disconnecting;
        return Unknown; // Default or error value
    }

    
	friend std::istream& operator>>(std::istream& is, ConnectionStatus::Value& value) 
	{
		std::string token;
		is >> token;
		value = ConnectionStatus::FromString(token.c_str());
		return is;
	}
    
    
    friend std::ostream& operator<<(std::ostream& os, const ConnectionStatus::Value& value) {
        ESP_LOGD("operator<<", "Connection Status to String: \"%s\"", ConnectionStatus::ToString(value).c_str());
        os << ConnectionStatus::ToString(value).c_str();
        return os;
    }
    
};
typedef ConnectionStatus::Value ConnectionStatus_t;

struct NamedObject_t
{
	void* Object;
	String Name = "";
    size_t ChangeCount = 0;
	
	// Destructor to release memory when the object is destroyed.
	~NamedObject_t()
	{
		if (Object != nullptr)
		{
			// Assuming Object points to dynamically allocated memory.
			free(Object);
			Object = nullptr;
		}
	}
};

struct NamedCallback_t
{
    public:
        String Name;
        void (*Callback)(const String& name, void* callback, void* arg);
        void* Arg;
        NamedCallback_t(){}
        NamedCallback_t(String name, void (*callback)(const String& name, void* callback, void* arg), void* arg = nullptr)
            : Name(name), Callback(callback), Arg(arg)
        {
        }

        NamedCallback_t(const NamedCallback_t& other)
            : Name(other.Name), Callback(other.Callback), Arg(other.Arg)
        {
        }

        bool operator==(const NamedCallback_t& other) const
        {
            return this->Name.equals(other.Name) && this->Callback == other.Callback && this->Arg == other.Arg;
        }

        NamedCallback_t& operator=(const NamedCallback_t& other)
        {
            if (this != &other)
            {
                Name = other.Name;
                Callback = other.Callback;
                Arg = other.Arg;
            }
            return *this;
        }
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
    bool operator==(const ProcessedSoundData_t& other) const
    {
        return this->NormalizedPower == other.NormalizedPower && this->Minimum == other.Minimum && this->Maximum == other.Maximum;
    }
};

struct ProcessedSoundFrame_t
{
	ProcessedSoundData_t Channel1;
	ProcessedSoundData_t Channel2;
    bool operator==(const ProcessedSoundFrame_t& other) const
    {
        return this->Channel1 == other.Channel1 && this->Channel2 == other.Channel2;
    }
};

struct MaxBandSoundData_t
{
	float MaxBandNormalizedPower;
	int16_t MaxBandIndex;
	int16_t TotalBands;
    MaxBandSoundData_t()
    {
	    float MaxBandNormalizedPower = 0.0;
	    int16_t MaxBandIndex = 0;
	    int16_t TotalBands = 0;
    }
    MaxBandSoundData_t(float MaxBandNormalizedPower_In, int16_t MaxBandIndex_In, int16_t TotalBands_In)
    {
        MaxBandNormalizedPower = MaxBandNormalizedPower_In;
        MaxBandIndex = MaxBandIndex_In;
        TotalBands = TotalBands_In;
    }
    bool operator==(const MaxBandSoundData_t& other) const
    {
        return this->MaxBandNormalizedPower == other.MaxBandNormalizedPower && this->MaxBandIndex == other.MaxBandIndex && this->TotalBands == other.TotalBands;
    }

    bool operator!=(const MaxBandSoundData_t& other) const
    {
        return !(*this == other);
    }

    operator String() const
    {
        return toString();
    }

    String toString() const
    {
        return String(MaxBandNormalizedPower) + ENCODE_VALUE_DIVIDER + String(MaxBandIndex) + ENCODE_VALUE_DIVIDER + String(TotalBands);
    }

    static MaxBandSoundData_t fromString(const std::string &str)
    {
        const std::string delimiter = ENCODE_VALUE_DIVIDER;
        size_t delimiterIndex = str.find(delimiter);

        if (delimiterIndex == std::string::npos)
        {
            return MaxBandSoundData_t();
        }

        float maxBandNormalizedPower = std::stof(str.substr(0, delimiterIndex));
        int16_t maxBandIndex = std::stoi(str.substr(delimiterIndex + delimiter.length()));
	    int16_t totalBands = std::stoi(str.substr(delimiterIndex + delimiter.length()));
        return MaxBandSoundData_t(maxBandNormalizedPower, maxBandIndex, totalBands);
    }

    friend std::istream& operator>>(std::istream& is, MaxBandSoundData_t& device) {
        std::string str;
        std::getline(is, str);
        device = MaxBandSoundData_t::fromString(str);
        return is;
    }

    friend std::ostream& operator<<(std::ostream& os, const MaxBandSoundData_t& device) {
        os << device.toString().c_str();
        return os;
    }
};


class DataTypeFunctions
{
	public:			
		DataType_t GetDataTypeFromString(String DataType)
		{
			for(int i = 0; i < sizeof(DataTypeStrings) / sizeof(DataTypeStrings[0]); ++i)
			{
				if(DataType.equals(DataTypeStrings[i]))return (DataType_t)i;
			}
			ESP_LOGE("DataTypes: GetDataTypeFromString: %s", "ERROR! \"%s\": Undefined Data Type.", DataType.c_str());
			return DataType_Undef;
		}

		template <typename T>
		DataType_t GetDataTypeFromTemplateType()
		{
			if(	std::is_same<T, bool>::value) 											return DataType_Bool_t;
			else if(std::is_same<T, int8_t>::value) 									return DataType_Int8_t;
			else if(std::is_same<T, int16_t>::value) 									return DataType_Int16_t;
			else if(std::is_same<T, int32_t>::value) 									return DataType_Int32_t;
			else if(std::is_same<T, uint8_t>::value) 									return DataType_Uint8_t;
			else if(std::is_same<T, uint16_t>::value) 									return DataType_Uint16_t;
			else if(std::is_same<T, uint32_t>::value) 									return DataType_Uint32_t;
			else if(std::is_same<T, char>::value) 										return DataType_Char_t;
			else if(std::is_same<T, String>::value) 									return DataType_String_t;
			else if(std::is_same<T, BT_Device_Info_t>::value) 							return DataType_BT_Device_Info_t;
			else if(std::is_same<T, BT_Device_Info_With_Time_Since_Update_t>::value) 	return DataType_BT_Device_Info_With_Time_Since_Update_t;
			else if(std::is_same<T, BluetoothDevice_t>::value)							return DataType_BluetoothDevice_t;
			else if(std::is_same<T, ActiveBluetoothDevice_t>::value)					return DataType_ActiveBluetoothDevice_t;
			else if(std::is_same<T, float>::value) 										return DataType_Float_t;
			else if(std::is_same<T, double>::value) 									return DataType_Double_t;
			else if(std::is_same<T, ProcessedSoundData_t>::value) 						return DataType_ProcessedSoundData_t;
			else if(std::is_same<T, MaxBandSoundData_t>::value) 						return DataType_MaxBandSoundData_t;
			else if(std::is_same<T, Frame_t>::value) 									return DataType_Frame_t;
			else if(std::is_same<T, ProcessedSoundFrame_t>::value) 						return DataType_ProcessedSoundFrame_t;
			else if(std::is_same<T, SoundState_t>::value) 								return DataType_SoundState_t;
			else if(std::is_same<T, ConnectionStatus_t>::value) 						return DataType_ConnectionStatus_t;
			else if(std::is_same<T, SoundInputSource_t>::value)							return DataType_SoundInputSource_t;
			else if(std::is_same<T, SoundOutputSource_t>::value)						return DataType_SoundOutputSource_t;
			else if(std::is_same<T, Bluetooth_Discovery_Mode_t>::value)					return DataType_Bluetooth_Discovery_Mode_t;
			else
			{
				ESP_LOGE("DataTypes: GetDataTypeFromTemplateType", "ERROR! Undefined Data Type.");
				return DataType_Undef;
			}
		}
		size_t GetSizeOfDataType(DataType_t DataType)
		{
			uint32_t result = 0;
			switch(DataType)
			{
				case DataType_Bool_t:
					result = sizeof(bool);
				break;
				
				case DataType_Int8_t:
					result = sizeof(int8_t);
				break;
				
				case DataType_Int16_t:
					result = sizeof(int16_t);
				break;
				
				case DataType_Int32_t:
					result = sizeof(int32_t);
				break;
				
				case DataType_Uint8_t:
					result = sizeof(uint8_t);
				break;
				
				case DataType_Uint16_t:
					result = sizeof(uint16_t);
				break;
				
				case DataType_Uint32_t:
					result = sizeof(uint32_t);
				break;
				
				case DataType_Char_t:
					result = sizeof(char);
				break;
				
				case DataType_String_t:
					result = sizeof(String);
				break;
				
				case DataType_BT_Device_Info_t:
					result = sizeof(BT_Device_Info_t);
				break;
				
				case DataType_BT_Device_Info_With_Time_Since_Update_t:
					result = sizeof(BT_Device_Info_With_Time_Since_Update_t);
				break;
				
				case DataType_BluetoothDevice_t:
					result = sizeof(BluetoothDevice_t);
				break;
				
				case DataType_ActiveBluetoothDevice_t:
					result = sizeof(ActiveBluetoothDevice_t);
				break;
				
				case DataType_Float_t:
					result = sizeof(float);
				break;
				
				case DataType_Double_t:
					result = sizeof(double);
				break;
				
				case DataType_ProcessedSoundData_t:
					result = sizeof(ProcessedSoundData_t);
				break;
				
				case DataType_MaxBandSoundData_t:
					result = sizeof(MaxBandSoundData_t);
				break;
				
				case DataType_Frame_t:
					result = sizeof(Frame_t);
				break;
				
				case DataType_ProcessedSoundFrame_t:
					result = sizeof(ProcessedSoundFrame_t);
				break;
				
				case DataType_SoundState_t:
					result = sizeof(SoundState_t);
				break;
				
				case DataType_ConnectionStatus_t:
					result = sizeof(ConnectionStatus_t);
				break;
				
				case DataType_SoundInputSource_t:
					result = sizeof(SoundInputSource_t);
				break;
				
				case DataType_SoundOutputSource_t:
					result = sizeof(SoundOutputSource_t);
				break;

                case DataType_Bluetooth_Discovery_Mode_t:
                    result = sizeof(Bluetooth_Discovery_Mode_t);
				break;
				
				default:
					ESP_LOGE("DataTypes: GetSizeOfDataType: %s", "ERROR! \"%s\": Undefined Data Type.", DataTypeStrings[DataType]);
					result = 0;
				break;
			}
			return result;
		}
};
