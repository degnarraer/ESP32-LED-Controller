#pragma once
#include "DataTypes.h"
#include "Preferences.h"

class IPreferences
{
    public:
        virtual ~IPreferences() {}

        virtual bool begin(const char* name, bool readOnly = false, const char* partition_label = nullptr) = 0;
        virtual void end() = 0;
        virtual bool clear() = 0;
        virtual bool remove(const char* key) = 0;
        virtual size_t putChar(const char* key, int8_t value) = 0;
        virtual size_t putUChar(const char* key, uint8_t value) = 0;
        virtual size_t putShort(const char* key, int16_t value) = 0;
        virtual size_t putUShort(const char* key, uint16_t value) = 0;
        virtual size_t putInt(const char* key, int32_t value) = 0;
        virtual size_t putUInt(const char* key, uint32_t value) = 0;
        virtual size_t putLong(const char* key, int32_t value) = 0;
        virtual size_t putULong(const char* key, uint32_t value) = 0;
        virtual size_t putLong64(const char* key, int64_t value) = 0;
        virtual size_t putULong64(const char* key, uint64_t value) = 0;
        virtual size_t putFloat(const char* key, float_t value) = 0;
        virtual size_t putDouble(const char* key, double_t value) = 0;
        virtual size_t putBool(const char* key, bool value) = 0;
        virtual size_t putString(const char* key, const char* value) = 0;
        virtual size_t putString(const char* key, String value) = 0;
        virtual size_t putBytes(const char* key, const void* value, size_t len) = 0;
        virtual bool isKey(const char* key) = 0;
        virtual PreferenceType getType(const char* key) = 0;
        virtual int8_t getChar(const char* key, int8_t defaultValue = 0) = 0;
        virtual uint8_t getUChar(const char* key, uint8_t defaultValue = 0) = 0;
        virtual int16_t getShort(const char* key, int16_t defaultValue = 0) = 0;
        virtual uint16_t getUShort(const char* key, uint16_t defaultValue = 0) = 0;
        virtual int32_t getInt(const char* key, int32_t defaultValue = 0) = 0;
        virtual uint32_t getUInt(const char* key, uint32_t defaultValue = 0) = 0;
        virtual int32_t getLong(const char* key, int32_t defaultValue = 0) = 0;
        virtual uint32_t getULong(const char* key, uint32_t defaultValue = 0) = 0;
        virtual int64_t getLong64(const char* key, int64_t defaultValue = 0) = 0;
        virtual uint64_t getULong64(const char* key, uint64_t defaultValue = 0) = 0;
        virtual float_t getFloat(const char* key, float_t defaultValue = NAN) = 0;
        virtual double_t getDouble(const char* key, double_t defaultValue = NAN) = 0;
        virtual bool getBool(const char* key, bool defaultValue = false) = 0;
        virtual size_t getString(const char* key, char* value, size_t maxLen) = 0;
        virtual String getString(const char* key, String defaultValue = String()) = 0;
        virtual size_t getBytesLength(const char* key) = 0;
        virtual size_t getBytes(const char* key, void* buf, size_t maxLen) = 0;
        virtual size_t freeEntries() = 0;
};

class PreferencesWrapper : public IPreferences
{
    public:
        PreferencesWrapper(Preferences* preferences)
                          : mp_preferences(preferences){}
    private:
        Preferences* mp_preferences = nullptr;
    public:
        bool begin(const char* name, bool readOnly = false, const char* partition_label = nullptr) override
        {
            ESP_LOGD("PreferencesWrapper", "begin");
            assert(mp_preferences);
            return mp_preferences->begin(name, readOnly, partition_label);
        }
        void end() override
        {
            ESP_LOGD("PreferencesWrapper", "end");
            assert(mp_preferences);
            mp_preferences->end();
        }
        bool clear() override
        {
            ESP_LOGD("PreferencesWrapper", "clear");
            assert(mp_preferences);
            return mp_preferences->clear();
        }
        bool remove(const char* key) override
        {
            ESP_LOGD("PreferencesWrapper", "remove");
            assert(mp_preferences);
            return mp_preferences->remove(key);
        }
        size_t putChar(const char* key, int8_t value) override
        {
            ESP_LOGD("PreferencesWrapper", "putChar");
            assert(mp_preferences);
            return mp_preferences->putChar(key, value);
        }
        size_t putUChar(const char* key, uint8_t value) override
        {
            ESP_LOGD("PreferencesWrapper", "putUChar");
            assert(mp_preferences);
            return mp_preferences->putUChar(key, value);
        }
        size_t putShort(const char* key, int16_t value) override
        {
            ESP_LOGD("PreferencesWrapper", "putShort");
            assert(mp_preferences);
            return mp_preferences->putShort(key, value);
        }
        size_t putUShort(const char* key, uint16_t value) override
        {
            ESP_LOGD("PreferencesWrapper", "putUShort");
            assert(mp_preferences);
            return mp_preferences->putUShort(key, value);
        }
        size_t putInt(const char* key, int32_t value) override
        {
            ESP_LOGD("PreferencesWrapper", "putInt");
            assert(mp_preferences);
            return mp_preferences->putInt(key, value);
        }
        size_t putUInt(const char* key, uint32_t value) override
        {
            ESP_LOGD("PreferencesWrapper", "putUInt");
            assert(mp_preferences);
            return mp_preferences->putUInt(key, value);
        }
        size_t putLong(const char* key, int32_t value) override
        {
            ESP_LOGD("PreferencesWrapper", "putLong");
            assert(mp_preferences);
            return mp_preferences->putLong(key, value);
        }
        size_t putULong(const char* key, uint32_t value) override
        {
            ESP_LOGD("PreferencesWrapper", "putULong");
            assert(mp_preferences);
            return mp_preferences->putULong(key, value);
        }
        size_t putLong64(const char* key, int64_t value) override
        {
            ESP_LOGD("PreferencesWrapper", "putLong64");
            assert(mp_preferences);
            return mp_preferences->putLong64(key, value);
        }
        size_t putULong64(const char* key, uint64_t value) override
        {
            ESP_LOGD("PreferencesWrapper", "putULong64");
            assert(mp_preferences);
            return mp_preferences->putULong64(key, value);
        }
        size_t putFloat(const char* key, float_t value) override
        {
            ESP_LOGD("PreferencesWrapper", "putFloat");
            assert(mp_preferences);
            return mp_preferences->putFloat(key, value);
        }
        size_t putDouble(const char* key, double_t value) override
        {
            ESP_LOGD("PreferencesWrapper", "putDouble");
            assert(mp_preferences);
            return mp_preferences->putDouble(key, value);
        }
        size_t putBool(const char* key, bool value) override
        {
            ESP_LOGD("PreferencesWrapper", "putBool");
            assert(mp_preferences);
            return mp_preferences->putBool(key, value);
        }
        size_t putString(const char* key, const char* value) override
        {
            ESP_LOGD("PreferencesWrapper", "putString");
            assert(mp_preferences);
            return mp_preferences->putString(key, value);
        }
        size_t putString(const char* key, String value) override
        {
            ESP_LOGD("PreferencesWrapper", "putString");
            assert(mp_preferences);
            return mp_preferences->putString(key, value);
        }
        size_t putBytes(const char* key, const void* value, size_t len) override
        {
            ESP_LOGD("PreferencesWrapper", "putBytes");
            assert(mp_preferences);
            return mp_preferences->putBytes(key, value, len);
        }
        bool isKey(const char* key) override
        {
            ESP_LOGD("PreferencesWrapper", "isKey");
            assert(mp_preferences);
            return mp_preferences->isKey(key);
        }
        PreferenceType getType(const char* key) override
        {
            ESP_LOGD("PreferencesWrapper", "getType");
            assert(mp_preferences);
            return mp_preferences->getType(key);
        }
        int8_t getChar(const char* key, int8_t defaultValue = 0) override
        {
            ESP_LOGD("PreferencesWrapper", "getChar");
            assert(mp_preferences);
            return mp_preferences->getChar(key, defaultValue);
        }
        uint8_t getUChar(const char* key, uint8_t defaultValue = 0) override
        {
            ESP_LOGD("PreferencesWrapper", "getUChar");
            assert(mp_preferences);
            return mp_preferences->getUChar(key, defaultValue);
        }
        int16_t getShort(const char* key, int16_t defaultValue = 0) override
        {
            ESP_LOGD("PreferencesWrapper", "getShort");
            assert(mp_preferences);
            return mp_preferences->getShort(key, defaultValue);
        }
        uint16_t getUShort(const char* key, uint16_t defaultValue = 0) override
        {
            ESP_LOGD("PreferencesWrapper", "getUShort");
            assert(mp_preferences);
            return mp_preferences->getUShort(key, defaultValue);
        }
        int32_t getInt(const char* key, int32_t defaultValue = 0) override
        {
            ESP_LOGD("PreferencesWrapper", "getInt");
            assert(mp_preferences);
            return mp_preferences->getInt(key, defaultValue);
        }
        uint32_t getUInt(const char* key, uint32_t defaultValue = 0) override
        {
            ESP_LOGD("PreferencesWrapper", "getUInt");
            assert(mp_preferences);
            return mp_preferences->getUInt(key, defaultValue);
        }
        int32_t getLong(const char* key, int32_t defaultValue = 0) override
        {
            ESP_LOGD("PreferencesWrapper", "getLong");
            assert(mp_preferences);
            return mp_preferences->getLong(key, defaultValue);
        }
        uint32_t getULong(const char* key, uint32_t defaultValue = 0) override
        {
            ESP_LOGD("PreferencesWrapper", "getULong");
            assert(mp_preferences);
            return mp_preferences->getULong(key, defaultValue);
        }
        int64_t getLong64(const char* key, int64_t defaultValue = 0) override
        {
            ESP_LOGD("PreferencesWrapper", "getLong64");
            assert(mp_preferences);
            return mp_preferences->getLong64(key, defaultValue);
        }
        uint64_t getULong64(const char* key, uint64_t defaultValue = 0) override
        {
            ESP_LOGD("PreferencesWrapper", "getULong64");
            assert(mp_preferences);
            return mp_preferences->getULong64(key, defaultValue);
        }
        float_t getFloat(const char* key, float_t defaultValue = NAN) override
        {
            ESP_LOGD("PreferencesWrapper", "getFloat");
            assert(mp_preferences);
            return mp_preferences->getFloat(key, defaultValue);
        }
        double_t getDouble(const char* key, double_t defaultValue = NAN) override
        {
            ESP_LOGD("PreferencesWrapper", "getDouble");
            assert(mp_preferences);
            return mp_preferences->getDouble(key, defaultValue);
        }
        bool getBool(const char* key, bool defaultValue = false) override
        {
            ESP_LOGD("PreferencesWrapper", "getBool");
            assert(mp_preferences);
            return mp_preferences->getBool(key, defaultValue);
        }
        size_t getString(const char* key, char* value, size_t maxLen) override
        {
            ESP_LOGD("PreferencesWrapper", "getString");
            assert(mp_preferences);
            return mp_preferences->getString(key, value, maxLen);
        }
        String getString(const char* key, String defaultValue = String()) override
        {
            ESP_LOGD("PreferencesWrapper", "getString");
            assert(mp_preferences);
            return mp_preferences->getString(key, defaultValue);
        }
        size_t getBytesLength(const char* key) override
        {
            ESP_LOGD("PreferencesWrapper", "getBytesLength");
            assert(mp_preferences);
            return mp_preferences->getBytesLength(key);
        }
        size_t getBytes(const char* key, void* buf, size_t maxLen) override
        {
            ESP_LOGD("PreferencesWrapper", "getBytes");
            assert(mp_preferences);
            return mp_preferences->getBytes(key, buf, maxLen);
        }
        size_t freeEntries() override
        {
            ESP_LOGD("PreferencesWrapper", "freeEntries");
            assert(mp_preferences);
            return mp_preferences->freeEntries();
        }
};

class PreferenceManager : public DataTypeFunctions 
{
public:
	typedef bool (*LoadedValueCallback_t)(const String&, void* object);
    enum class PreferenceUpdateType
    {
        Initialize,
        Load,
        Save,
        Timer
    };
    typedef PreferenceUpdateType PreferenceUpdateType_t;

    struct PreferenceManagerTimerArgs
    {
        PreferenceManager* PreferenceManagerInstance;
        const String Value;
        PreferenceManagerTimerArgs( PreferenceManager* preferenceManagerInstance
                                  , const String value )
                                  : PreferenceManagerInstance(preferenceManagerInstance)
                                  , Value(value) {}
    };

    PreferenceManager( IPreferences* preferencesInterface
                     , const String key
                     , const String initialValue
                     , const unsigned long timeoutTime
                     , LoadedValueCallback_t callback
                     , void* object)
                     : mp_PreferencesInterface(preferencesInterface)
                     , m_Key(key)
                     , m_InitialValue(initialValue)
                     , m_TimeoutTime(timeoutTime)
                     , m_Callback(callback)
                     , mp_Object(object)
    {
        m_PreferencesMutex = xSemaphoreCreateRecursiveMutex();
    }

    virtual ~PreferenceManager()
    {
        CleanUpTimer();
        if (m_PreferencesMutex)
        {
            vSemaphoreDelete(m_PreferencesMutex);
        }
    }

    bool InitializeAndLoadPreference()
    {
        bool result = false;
        m_Preferences_Last_Update = millis();
        ESP_LOGD("InitializeAndLoadPreference", "Initializing Preference: \"%s\" with Initial Value: \"%s\"", m_Key.c_str(), m_InitialValue.c_str());
        xSemaphoreTakeRecursive(m_PreferencesMutex, portMAX_DELAY);   
        if (mp_PreferencesInterface)
        {
            bool isKey = mp_PreferencesInterface->isKey(m_Key.c_str());
            if (isKey)
            {
                ESP_LOGI("InitializeAndLoadPreference", "Preference Found: \"%s\" Loading saved value", m_Key.c_str());
                result = Update_Preference(PreferenceUpdateType::Load, m_InitialValue);
            }
            else
            {
                ESP_LOGI("InitializeAndLoadPreference", "Preference Not Found: \"%s\" Initializing with: \"%s\" ", m_Key.c_str(), m_InitialValue.c_str());
                result = Update_Preference(PreferenceUpdateType::Initialize, m_InitialValue);
            }
        }
        else
        {
            ESP_LOGE("InitializeAndLoadPreference", "ERROR! Null Preferences Pointer.");
        }
        xSemaphoreGiveRecursive(m_PreferencesMutex);
        return result;
    }

    bool Update_Preference( const PreferenceUpdateType updateType
						  , const String stringToSave )
    {
        ESP_LOGD("Update_Preference", "Update Prefernce for: \"%s\" with new value: \"%s\"", m_Key.c_str(), stringToSave.c_str());
        bool result = false;
        if (!mp_PreferencesInterface)
        {
            ESP_LOGE("Update_Preference", "ERROR! \"%s\": NULL PreferenceInterface Pointer.", m_Key.c_str());
            return result;
        }
        switch (updateType)
        {
            case PreferenceUpdateType::Timer:
                ESP_LOGD("Update_Preference", "\"%s\": Delayed Save", m_Key.c_str());
                m_PreferenceTimerActive = false;
                result = TrySave(stringToSave);
                break;
            case PreferenceUpdateType::Initialize:
                ESP_LOGD("Update_Preference", "\"%s\": Initializing Preference", m_Key.c_str());
                result = PerformSave(stringToSave);
                break;
            case PreferenceUpdateType::Load:
                ESP_LOGD("Update_Preference: Update_Preference", "\"%s\": Loading Preference", m_Key.c_str());
                result = HandleLoad();
                break;
            case PreferenceUpdateType::Save:
                ESP_LOGD("Update_Preference", "\"%s\": Updating Preference", m_Key.c_str());
                result = TrySave(stringToSave);
                break;
            default:
                ESP_LOGE("Update_Preference", "ERROR! \"%s\": Unsupported Update Type.", m_Key.c_str());
                break;
        }
        return result;
    }
private:
    bool CleanUpTimer()
    {  
        bool result = true;
        if (m_PreferenceTimer)
        {
            ESP_LOGD("CleanUpTimer", "Destroying timer for Key: \"%s\"", m_Key.c_str());
            esp_err_t stop_err = esp_timer_stop(m_PreferenceTimer);
            if (m_PreferenceTimerActive == true && stop_err != ESP_OK)
            {
                ESP_LOGE("CleanUpTimer", "ERROR! Unable to stop timer: %s.", esp_err_to_name(stop_err));
                result = false;
            }
            esp_err_t delete_err = esp_timer_delete(m_PreferenceTimer);
            if (delete_err != ESP_OK)
            {
                ESP_LOGE("CleanUpTimer", "ERROR! Unable to delete timer: %s.", esp_err_to_name(delete_err));
                result = false;
            }
            else
            {
                ESP_LOGD("CleanUpTimer", "Timer Destroyed");
                m_PreferenceTimer = nullptr;
            }
        }
        if (mp_PreferenceTimerCreateArgs)
        {
            ESP_LOGD("CleanUpTimer", "Deleting existing timer create args for Key: \"%s\"", m_Key.c_str());
            delete mp_PreferenceTimerCreateArgs;
            mp_PreferenceTimerCreateArgs = nullptr;
        }
        if (mp_TimerArgs)
        {
            ESP_LOGD("CleanUpTimer", "Deleting existing timer args for Key: \"%s\"", m_Key.c_str());
            delete mp_TimerArgs;
            mp_TimerArgs = nullptr;
        }
        m_PreferenceTimerActive = false;
        return result;
    }
    bool DelaySaveValue(const String &saveValue, unsigned long elapsedTime)
    {
        bool result = false;
        ESP_LOGD("DelaySaveValue", "Entering DelaySaveValue for Key: \"%s\"", m_Key.c_str());
        CleanUpTimer();
        mp_TimerArgs = new PreferenceManagerTimerArgs(this, saveValue);
        if(mp_TimerArgs)
        {
            mp_PreferenceTimerCreateArgs = new esp_timer_create_args_t;
            if(mp_PreferenceTimerCreateArgs)
            {
                *(mp_PreferenceTimerCreateArgs) = {&Static_Update_Preference_Timer_Call, mp_TimerArgs, esp_timer_dispatch_t::ESP_TIMER_TASK, "Preferences_Timer"};
                ESP_LOGD("DelaySaveValue", "Creating Delayed Save Timer for Key: \"%s\"", m_Key.c_str());    
                esp_err_t timerCreateErr = esp_timer_create(mp_PreferenceTimerCreateArgs, &m_PreferenceTimer);
                if (timerCreateErr != ESP_OK)
                {
                    ESP_LOGE("DelaySaveValue", "ERROR! Failed to create timer for Key: \"%s\", error: %d.", m_Key.c_str(), timerCreateErr);
                    delete mp_TimerArgs;
                    mp_TimerArgs = nullptr;
                    delete mp_PreferenceTimerCreateArgs;
                    mp_PreferenceTimerCreateArgs = nullptr;
                    return result;
                }
                unsigned long delayTime = (m_TimeoutTime - elapsedTime) * 1000;
                esp_err_t timerStartErr = esp_timer_start_once(m_PreferenceTimer, delayTime);
                if (timerStartErr != ESP_OK)
                {
                    ESP_LOGE("DelaySaveValue", "ERROR! Failed to start timer for Key: \"%s\", error: %d.", m_Key.c_str(), timerStartErr);
                    esp_timer_delete(m_PreferenceTimer);
                    m_PreferenceTimer = nullptr;
                    delete mp_TimerArgs;
                    mp_TimerArgs = nullptr;
                    delete mp_PreferenceTimerCreateArgs;
                    mp_PreferenceTimerCreateArgs = nullptr;
                    return result;
                }
                ESP_LOGD("DelaySaveValue", "Delayed Save Timer Started for Key: \"%s\" for %lu uS", m_Key.c_str(), delayTime);
                m_PreferenceTimerActive = true;
                result = true;
            }
            else
            {
                ESP_LOGE("DelaySaveValue", "ERROR! Null TimerCreateArg Pointer.");
            }
        }
        else
        {
            ESP_LOGE("DelaySaveValue", "ERROR! Null TimerArg Pointer.");
        }
        return result;
    }

    bool HandleLoad()
    {
        xSemaphoreTakeRecursive(m_PreferencesMutex, portMAX_DELAY);
        bool result = false;
        if(mp_PreferencesInterface)
        {
            ESP_LOGD("HandleLoad", "Loading Key: \"%s\"", m_Key.c_str());
            String loadedValue = mp_PreferencesInterface->getString(m_Key.c_str(), m_InitialValue);
            ESP_LOGD("HandleLoad", "Loaded Key: \"%s\" Value: \"%s\"", m_Key.c_str(), loadedValue.c_str());
            if (m_Callback && mp_Object)
            {
                if(m_Callback(loadedValue, mp_Object))
                {
                    ESP_LOGI("HandleLoad", "Successfully Loaded Key: \"%s\" Value: \"%s\"", m_Key.c_str(), loadedValue.c_str());
                    result = true;
                }
                else
                {
                    ESP_LOGW("HandleLoad", "\"%s\" Failed to Load Value. Loading Default Value: \"%s\"", m_Key.c_str(), m_InitialValue.c_str());
                    if(m_Callback(m_InitialValue, mp_Object))
                    {
                        ESP_LOGI("HandleLoad", "Successfully Loaded Key: \"%s\" Default Value: \"%s\"", m_Key.c_str(), loadedValue.c_str());
                        result = true;
                    }
                    else
                    {
                        ESP_LOGE("HandleLoad", "ERROR! \"%s\" Failed to Load Default Value: \"%s\".", m_Key.c_str(), m_InitialValue.c_str());
                    }
                }
            }
            else
            {
                ESP_LOGD("HandleLoad", "\"%s\" No Callback Pointers", m_Key.c_str());
                result = true;
            }
        }
        else
        {
            ESP_LOGE("HandleLoad", "ERROR! \"%s\" Null PreferencesInterface Pointer.", m_Key.c_str());
        }
        xSemaphoreGiveRecursive(m_PreferencesMutex);
        return result;
    }
    
    bool PerformSave(const String &stringToSave)
    {
        unsigned long currentMillis = millis();
        bool result = false;
        size_t saveLength = mp_PreferencesInterface->putString(m_Key.c_str(), stringToSave);
        if(stringToSave.length() == saveLength)
        {
            String savedString = mp_PreferencesInterface->getString(m_Key.c_str(), m_InitialValue);
            ESP_LOGD("PerformSave", "Key: \"%s\" Comparing Strings: String to Save: \"%s\" Actual String Saved: \"%s\".", m_Key.c_str(), stringToSave.c_str(), savedString.c_str());  
            if(savedString.equals(stringToSave))
            {
                ESP_LOGI("PerformSave", "Key: \"%s\" String Saved: \"%s\"", m_Key.c_str(), savedString.c_str());
                m_Preferences_Last_Update = currentMillis;
                result = true;
            }
            else
            {
                ESP_LOGE("PerformSave", "ERROR! Key: \"%s\" Did Not Save Properly! String to save: \"%s\" Saved String: \"%s\".", m_Key.c_str(), stringToSave.c_str(), savedString.c_str());
                mp_PreferencesInterface->remove(m_Key.c_str());
            }
        }
        else
        {
            ESP_LOGE("PerformSave", "ERROR! Save Error: \"%s\" Tried to save: \"%s\". Expected to save %i characters, but saved %i characters.", m_Key.c_str(), stringToSave.c_str(), stringToSave.length(), saveLength);
            mp_PreferencesInterface->remove(m_Key.c_str());
        }
        return result;
    }

    bool TrySave(const String &stringToSave)
    {
        ESP_LOGD("TrySave", "Try Saving Key: \"%s\" Value: \"%s\"", m_Key.c_str(), stringToSave.c_str());
        bool result = false;
        if(mp_PreferencesInterface)
        {
            unsigned long currentMillis = millis();
            unsigned long elapsedTime;
            if (currentMillis >= m_Preferences_Last_Update)
            {
                elapsedTime = currentMillis - m_Preferences_Last_Update;
            } 
            else
            {
                elapsedTime = (ULONG_MAX - m_Preferences_Last_Update + 1) + currentMillis;
            }
            if (elapsedTime <= m_TimeoutTime)
            {
                ESP_LOGD("TrySave", "\"%s\": Too early to save preference", m_Key.c_str());
                if (!m_PreferenceTimerActive)
                {
                    ESP_LOGD("TrySave", "\"%s\": Started NVM Update Timer", m_Key.c_str());
                    result = DelaySaveValue(stringToSave, elapsedTime);
                }
                else
                {
                    ESP_LOGD("TrySave", "\"%s\": NVM Update Timer already running, injecting new value to save.", m_Key.c_str());
                    result = DelaySaveValue(stringToSave, elapsedTime);
                }
            }
            else
            {
                xSemaphoreTakeRecursive(m_PreferencesMutex, portMAX_DELAY);
                String savedString = mp_PreferencesInterface->getString(m_Key.c_str(), m_InitialValue);
                ESP_LOGD("TrySave", "Key: \"%s\" Comparing Strings: New: \"%s\" Existing: \"%s\".", m_Key.c_str(), stringToSave.c_str(), savedString.c_str());  
                if(stringToSave.equals(savedString))
                {
                    ESP_LOGD("TrySave", "Key: \"%s\" Skipping save as String to Save: \"%s\" equals Saved String: \"%s\".", m_Key.c_str(), stringToSave.c_str(), savedString.c_str());       
                }
                else
                {
                    result = PerformSave(stringToSave);
                }
                xSemaphoreGiveRecursive(m_PreferencesMutex);
            }
        }
        else
        {
            ESP_LOGE("TrySave", "ERROR! \"%s\" Null Pointer.", m_Key.c_str());
        }
        return result;
    }

private:
    IPreferences* mp_PreferencesInterface = nullptr;
    const String m_Key;
    const String m_InitialValue;
    const unsigned long m_TimeoutTime;
    LoadedValueCallback_t m_Callback;
    void* mp_Object;
    PreferenceManagerTimerArgs* mp_TimerArgs = nullptr;
    esp_timer_create_args_t* mp_PreferenceTimerCreateArgs = nullptr;
    esp_timer_handle_t m_PreferenceTimer = nullptr;
    uint64_t m_Preferences_Last_Update = 0;
    bool m_PreferenceTimerActive = false;
    SemaphoreHandle_t m_PreferencesMutex;

    static void Static_Update_Preference_Timer_Call(void* arg)
    {
        if(arg)
        {
            ESP_LOGD("Static_Update_Preference_Timer_Call", "Static_Update_Preference Function Called");
            PreferenceManagerTimerArgs* timerArgsPtr = static_cast<PreferenceManagerTimerArgs*>(arg);
            if(timerArgsPtr)
            {
                PreferenceManager* preferenceManager = timerArgsPtr->PreferenceManagerInstance;
                if (preferenceManager)
                {
                    ESP_LOGD("Static_Update_Preference_Timer_Call", "Updating Preference");
                    preferenceManager->Update_Preference( PreferenceUpdateType::Timer
                                                        , timerArgsPtr->Value );
                }
                else
                {
                    ESP_LOGE("Static_Update_Preference_Timer_Call", "ERROR! Null PreferenceManagerInstance Pointer.");
                }
            }
            else
            {
                ESP_LOGE("Static_Update_Preference_Timer_Call", "ERROR! Null timerArgsPtr Pointer.");
            }
        }
        else
        {
            ESP_LOGE("Static_Update_Preference_Timer_Call", "ERROR! Null arg Pointer.");
        }
    }
};