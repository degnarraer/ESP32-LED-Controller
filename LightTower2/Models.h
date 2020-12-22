/*
    Light Tower by Rob Shockency
    Copyright (C) 2020 Rob Shockency degnarraer@yahoo.com

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
 
#ifndef Models_H
#define Models_H
#include "TaskInterface.h"
#include <LinkedList.h>
#include "Streaming.h"
#include "Tunes.h"
#include "Statistical_Engine.h"
#include "LEDControllerInterface.h"



template <class T> class ModelEventNotificationCaller;
template <class T>
class ModelEventNotificationCallee
{
  public:
    virtual void NewValueNotification(T Value) = 0;
};

template <class T>
class ModelEventNotificationCaller
{
  public:
    ModelEventNotificationCaller<T>(){}
    void RegisterForNotification(ModelEventNotificationCallee<T> &callee)
    {
      if(true == debugModelNotifications) Serial << "ModelEventNotificationCaller: Added\n";        
      m_MyCallees.add(&callee);
    }
    void DeRegisterForNotification(ModelEventNotificationCallee<T> &callee)
    {
      for(int i = 0; i < m_MyCallees.size(); ++i)
      {
        if(m_MyCallees.get(i) == &callee)
        {
          m_MyCallees.remove(i);
          break;
        }
      }
    }
    bool HasUser()
    {
      return (m_MyCallees.size() > 0)? true:false;
    }
    void SendNewValueNotificationToCallees(T value)
    {
      if(true == debugModelNotifications) Serial << "ModelEventNotificationCaller: Sending New Value Notification with Value: " << value << "\n"; 
      for(int i = 0; i < m_MyCallees.size(); ++i)
      {
        if(true == debugModelNotifications) Serial << "ModelEventNotificationCaller: Sending Notification " << i << "\n"; 
        m_MyCallees.get(i)->NewValueNotification(value);
      }
    }
    virtual void UpdateValue() = 0;
  private:
    LinkedList<ModelEventNotificationCallee<T>*> m_MyCallees = LinkedList<ModelEventNotificationCallee<T>*>();
};

class StatisticalEngineModelInterface;

class Model: public Task
{
  public: 
    Model(String Title): Task(Title){}
    ~Model(){}

    //ModelEventNotificationCaller
    virtual void UpdateValue() = 0;
    
  private:
    virtual void SetupModel() = 0;
    virtual bool CanRunModelTask() = 0;
    virtual void RunModelTask() = 0;
    void Setup()
    {
      SetupModel();
    }
    bool CanRunMyTask()
    {
      return CanRunModelTask();
    }
    void RunMyTask()
    {
      RunModelTask();
      UpdateValue();
    }
};

class DataModel: public Model
{
  public: 
    DataModel(String Title, StatisticalEngineModelInterface &StatisticalEngineModelInterface): Model(Title)
                                                                                             , m_StatisticalEngineModelInterface(StatisticalEngineModelInterface){}
    ~DataModel(){}

    //ModelEventNotificationCaller
    virtual void UpdateValue() = 0;
    
  protected:
    StatisticalEngineModelInterface &m_StatisticalEngineModelInterface;  
  private:
    virtual void SetupModel() = 0;
    virtual bool CanRunModelTask() = 0;
    virtual void RunModelTask() = 0;
    void Setup()
    {
      SetupModel();
    }
    bool CanRunMyTask()
    {
      return CanRunModelTask();
    }
    void RunMyTask()
    {
      RunModelTask();
      UpdateValue();
    }
};

template <class T>
class ModelWithNewValueNotification: public Model
                                   , public ModelEventNotificationCaller<T>
{
  public:
    ModelWithNewValueNotification<T>(String Title): Model(Title){}
    ~ModelWithNewValueNotification<T>(){}
    
  protected:
    void SetCurrentValue(T value)
    {
      m_CurrentValue = value;
      if(m_PreviousValue != m_CurrentValue)
      {
        m_PreviousValue = m_CurrentValue;
        this->SendNewValueNotificationToCallees(m_CurrentValue);
      }
    }
    virtual void SetupModel() = 0;
    virtual bool CanRunModelTask() = 0;
    virtual void RunModelTask() = 0;
    T m_PreviousValue;
    T m_CurrentValue;
};

template <class T>
class DataModelWithNewValueNotification: public DataModel
                                       , public ModelEventNotificationCaller<T>
{
  public:
    DataModelWithNewValueNotification<T>(String Title, StatisticalEngineModelInterface &StatisticalEngineModelInterface): DataModel(Title, StatisticalEngineModelInterface){}
    ~DataModelWithNewValueNotification<T>(){}
    
  protected:
    void SetCurrentValue(T value)
    {
      m_CurrentValue = value;
      if(m_PreviousValue != m_CurrentValue)
      {
        m_PreviousValue = m_CurrentValue;
        this->SendNewValueNotificationToCallees(m_CurrentValue);
      }
    }
    virtual void SetupModel() = 0;
    virtual bool CanRunModelTask() = 0;
    virtual void RunModelTask() = 0;
    T m_PreviousValue;
    T m_CurrentValue;
};


class StatisticalEngineModelInterface : public Task
                                      , ADCInterruptHandler 
                                      , MicrophoneMeasureCalleeInterface
{
  public:
    StatisticalEngineModelInterface() : Task("StatisticalEngineModelInterface"){}
    ~StatisticalEngineModelInterface(){}

    //StatisticalEngine Getters
    unsigned int GetNumberOfBands() { return m_StatisticalEngine.GetNumberOfBands(); }
    float GetSoundPower() { return m_StatisticalEngine.GetSoundPower(); }
    float GetBandAverage(unsigned int band, unsigned int depth) { return m_StatisticalEngine.GetBandAverage(band, depth); }
    float GetBandAverageForABandOutOfNBands(unsigned int band, unsigned int depth, unsigned int totalBands) { return m_StatisticalEngine.GetBandAverageForABandOutOfNBands(band, depth, totalBands); }
  
    //ADCInterruptHandler
    void HandleADCInterrupt() { m_StatisticalEngine.HandleADCInterrupt(); }
    
    //MicrophoneMeasureCalleeInterface
    void MicrophoneStateChange(SoundState){}

  private:
    StatisticalEngine m_StatisticalEngine;

    //Task
    void Setup();
    bool CanRunMyTask();
    void RunMyTask();
};

class SoundPowerModel: public DataModelWithNewValueNotification<float>
{
  public:
    SoundPowerModel(String Title, StatisticalEngineModelInterface &StatisticalEngineModelInterface): DataModelWithNewValueNotification<float>(Title, StatisticalEngineModelInterface){}
    ~SoundPowerModel(){}
    
     //Model
    void UpdateValue() { SetCurrentValue(m_StatisticalEngineModelInterface.GetSoundPower()); }
  private:
     //Model
    void SetupModel(){}
    bool CanRunModelTask(){ return true; }
    void RunModelTask(){}
};

class BandPowerModel: public DataModelWithNewValueNotification<float>
{
  public:
    BandPowerModel(String Title, unsigned int Band, StatisticalEngineModelInterface &StatisticalEngineModelInterface): DataModelWithNewValueNotification<float>(Title, StatisticalEngineModelInterface)
                                                                                                                     , m_Band(Band){}
    ~BandPowerModel(){}
    
     //Model
    void UpdateValue()
    {
      float value = (m_StatisticalEngineModelInterface.GetBandAverage(m_Band, 1) / (float)ADDBITS);
      if(true == debugModels) Serial << "BandPowerModel value: " << value << " for band: " << m_Band << "\n";
      SetCurrentValue( value );
    }
  private:
     //Model
    unsigned int m_Band = 0;
    void SetupModel(){}
    bool CanRunModelTask(){ return true; }
    void RunModelTask(){}
};

class ReducedBandsBandPowerModel: public DataModelWithNewValueNotification<float>
{
  public:
    ReducedBandsBandPowerModel( String Title
                              , unsigned int band
                              , unsigned int depth
                              , unsigned int totalBands
                              , StatisticalEngineModelInterface &StatisticalEngineModelInterface)
                              : DataModelWithNewValueNotification<float>(Title, StatisticalEngineModelInterface)
                              , m_Band(band)
                              , m_Depth(depth)
                              , m_TotalBands(totalBands){}
    ~ReducedBandsBandPowerModel(){}
    
     //Model
    void UpdateValue()
    {
      float value = (m_StatisticalEngineModelInterface.GetBandAverageForABandOutOfNBands(m_Band, m_Depth, m_TotalBands) / (float)ADDBITS);
      if(true == debugModels) Serial << "ReducedBandsBandPowerModel value: " << value << " for band: " << m_Band << " of " << m_TotalBands << " bands\n";
      SetCurrentValue( value );
    }
  private:
     //Model
    unsigned int m_Band = 0;
    unsigned int m_Depth = 0;
    unsigned int m_TotalBands = 0;
    void SetupModel(){}
    bool CanRunModelTask(){ return true; }
    void RunModelTask(){}
};

class RandomColorFadingModel: public ModelWithNewValueNotification<CRGB>
{
  public:
    RandomColorFadingModel( String Title
                          , unsigned long Duration)
                          : ModelWithNewValueNotification<CRGB>(Title)
                          , m_Duration(Duration){}
    ~RandomColorFadingModel(){}
  private:
     //Model
    void UpdateValue();
    void SetupModel();
    bool CanRunModelTask();
    void RunModelTask();

    //This
    CRGB m_CurrentColor;
    CRGB m_StartColor;
    CRGB m_EndColor;
    unsigned long m_Duration;
    unsigned long m_CurrentDuration;
    unsigned long m_CurrentTime;
    unsigned long m_StartTime;
    void StartFadingNextColor();
    CRGB GetRandomNonGrayColor();
};

class RainbowColorModel: public ModelWithNewValueNotification<CRGB>
{
  public:
    RainbowColorModel( String Title
                     , unsigned int Numerator
                     , unsigned int Denominator)
                     : ModelWithNewValueNotification<CRGB>(Title)
                     , m_Numerator(Numerator)
                     , m_Denominator(Denominator){}
    ~RainbowColorModel(){}
  private:
     unsigned int m_Numerator;
     unsigned int m_Denominator;
     //Model
    void UpdateValue(){ SetCurrentValue(GetColor(m_Numerator, m_Denominator)); }
    void SetupModel(){ }
    bool CanRunModelTask(){ return true; }
    void RunModelTask() { }

    //This
    CRGB GetColor(unsigned int numerator, unsigned int denominator);

};
#endif
