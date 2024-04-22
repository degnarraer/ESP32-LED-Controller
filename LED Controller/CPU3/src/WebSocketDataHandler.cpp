#include "WebSocketDataHandler.h"


void WebSocketDataProcessor::UpdateAllDataToClient(uint8_t clientId)
{
  ESP_LOGI("WebSocketDataProcessor::UpdateAllDataToClient", "Sending All Data to Client: %i", clientId );
  std::vector<KVP> KeyValuePairs = std::vector<KVP>();
  for(int i = 0; i < m_MySenders.size(); ++i)
  {
    m_MySenders[i]->AppendCurrentValueToKVP(&KeyValuePairs, true);
  }
  NotifyClient(clientId, Encode_Widget_Values_To_JSON(&KeyValuePairs));
}

void WebSocketDataProcessor::WebSocketDataProcessor_Task()
{
  const TickType_t xFrequency = 20;
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while(true)
  {
    vTaskDelayUntil( &xLastWakeTime, xFrequency );
    std::vector<KVP> KeyValuePairs = std::vector<KVP>();
    for(int i = 0; i < m_MySenders.size(); ++i)
    {
      m_MySenders[i]->AppendCurrentValueToKVP(&KeyValuePairs);
      if(KeyValuePairs.size() > MAX_VALUES_TO_SEND_AT_ONCE)
      {
        NotifyClients(Encode_Widget_Values_To_JSON(&KeyValuePairs));
        KeyValuePairs.clear();
      }
    }
    if(KeyValuePairs.size() >0)
    {
      NotifyClients(Encode_Widget_Values_To_JSON(&KeyValuePairs));
    }
  }  
}

void WebSocketDataProcessor::RegisterAsWebSocketDataReceiver(const String& Name, WebSocketDataHandlerReceiver *aReceiver)
{
  auto it = std::find(m_MyReceivers.begin(), m_MyReceivers.end(), aReceiver);
  if (it == m_MyReceivers.end())
  {
    ESP_LOGI("RegisterAsWebSocketDataReceiver", "Registering %s as Web Socket Data Receiver.", Name.c_str());
    m_MyReceivers.push_back(aReceiver);
  }
}

void WebSocketDataProcessor::DeRegisterAsWebSocketDataReceiver(const String& Name, WebSocketDataHandlerReceiver *aReceiver)
{
  auto it = std::find(m_MyReceivers.begin(), m_MyReceivers.end(), aReceiver);
  if (it != m_MyReceivers.end())
  {
    ESP_LOGI("RegisterAsWebSocketDataSender", "DeRegistering %s as Web Socket Data Receiver.", Name.c_str());
    m_MyReceivers.erase(it);
  }
}

void WebSocketDataProcessor::RegisterAsWebSocketDataSender(const String& Name, WebSocketDataHandlerSender *aSender)
{
  auto it = std::find(m_MySenders.begin(), m_MySenders.end(), aSender);
  if (it == m_MySenders.end())
  {
    ESP_LOGI("RegisterAsWebSocketDataSender", "Registering %s as Web Socket Data Sender.", Name.c_str());
    m_MySenders.push_back(aSender);
  }
}

void WebSocketDataProcessor::DeRegisterAsWebSocketDataSender(const String& Name, WebSocketDataHandlerSender *aSender)
{
  // Find the iterator pointing to the element
  auto it = std::find(m_MySenders.begin(), m_MySenders.end(), aSender);

  // Check if the element was found before erasing
  if (it != m_MySenders.end())
  {
    ESP_LOGI("RegisterAsWebSocketDataSender", "DeRegistering %s as Web Socket Data Sender.", Name.c_str());
    m_MySenders.erase(it);
  }
}


bool WebSocketDataProcessor::ProcessWidgetValueAndSendToDatalink(const String& WidgetId, const String& Value)
{
  bool WidgetFound = false;
  for(int i = 0; i < m_MyReceivers.size(); ++i)
  {
    if(true == m_MyReceivers[i]->ProcessWidgetValueAndSendToDatalink(WidgetId, Value))
    {
      WidgetFound = true;
    }
  }
  return WidgetFound;
}

String WebSocketDataProcessor::Encode_Widget_Values_To_JSON(std::vector<KVP> *KeyValuePairs)
{
  JSONVar jSONVars;
  String result;
  for(int i = 0; i < KeyValuePairs->size(); ++i)
  {
    JSONVar SettingValues;
    SettingValues["Id"] = KeyValuePairs->at(i).Key;
    SettingValues["Value"] = KeyValuePairs->at(i).Value;
    jSONVars["WidgetValue" + String(i)] = SettingValues; 
  }
  if(KeyValuePairs->size() > 0)
  {
    result = JSON.stringify(jSONVars);
  }
  else
  {
    result = "";
  }
  return result;
}

void WebSocketDataProcessor::NotifyClient(const uint8_t clientID, const String& TextString)
{
  if(0 < TextString.length())
  {
    m_WebSocket.text(clientID, TextString);
  }
}

void WebSocketDataProcessor::NotifyClients(const String& TextString)
{
  if(0 < TextString.length())
  {
    m_WebSocket.textAll(TextString);
  }
}
