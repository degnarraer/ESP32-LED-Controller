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

#include "Streaming.h"
#include "Tunes.h"
#include "SettingsWebServer.h"
#include "SPIFFS.h"
#include "HardwareSerial.h"

/*
TaskHandle_t WebServer_Task;
uint32_t WebServer_TaskLoopCount = 0;

// Create AsyncWebServer object on port 80
AsyncWebServer MyWebServer(80);

// Create WebSocket
AsyncWebSocket MyWebSocket("/ws");

// Create Settings Web Server that uses the Socket 
SettingsWebServerManager m_SettingsWebServerManager( "My Settings Web Server Manager", MyWebSocket );

// Static Callback for Web Socket
void OnEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  m_SettingsWebServerManager.OnEvent(server, client, type, arg, data, len);
}

// Web Socket init to register web socket callback and connect it to the web server
void InitWebSocket()
{
  MyWebSocket.onEvent(OnEvent);
  MyWebServer.addHandler(&MyWebSocket);
}

// Init the web server to use the local SPIFFS memory and serve up index.html file.
void InitWebServer()
{
  // Web Server Root URL
  MyWebServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/index.html", "text/html");
  });
  MyWebServer.serveStatic("/", SPIFFS, "/").setCacheControl("max-age = 300");
}

// Start the web server
void StartWebServer()
{
  // Start server
  MyWebServer.begin();
}

// Initialize SPIFFS
void InitFileSystem()
{
  if (SPIFFS.begin())
  {
    Serial.println("SPIFFS mounted successfully");
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    while(file)
    {
        Serial.print("FILE: ");
        Serial.println(file.name());
        file = root.openNextFile();
    }
  }
  else
  {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
}
*/
// Init Tasks to run using FreeRTOS
void InitTasks()
{
  //xTaskCreatePinnedToCore( WebServer_TaskLoop,  "WebServer_Task",   10000,  NULL,  configMAX_PRIORITIES - 1,    &WebServer_Task,    0 );
}

void InitLocalVariables()
{
  //m_SettingsWebServerManager.SetupSettingsWebServerManager();
}

void setup()
{
  Serial.begin(500000);
  Serial1.begin(500000, SERIAL_8N1, CPU1_RX, CPU1_TX);
  Serial2.begin(500000, SERIAL_8N1, CPU2_RX, CPU2_TX);
  InitLocalVariables();
  //InitFileSystem();
  //InitWebServer();
  //InitWebSocket();
  //InitTasks();
  //StartWebServer();
  PrintMemory();
}

void loop()
{
}
//#pragma GCC diagnostic pop

void PrintMemory()
{
  ESP_LOGE("Settings_Web_Server", "Total heap: %d", ESP.getHeapSize());
  ESP_LOGE("Settings_Web_Server", "Free heap: %d", ESP.getFreeHeap());
  ESP_LOGE("Settings_Web_Server", "Total PSRAM: %d", ESP.getPsramSize());
  ESP_LOGE("Settings_Web_Server", "Free PSRAM: %d", ESP.getFreePsram());
}

/*
void WebServer_TaskLoop(void * parameter)
{
  const TickType_t xFrequency = 20;
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while(true)
  {
    vTaskDelayUntil( &xLastWakeTime, xFrequency );
    ++WebServer_TaskLoopCount;
    m_SettingsWebServerManager.ProcessEventQueue();
  }  
}
*/
