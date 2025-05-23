#include <Arduino.h>
#include <WiFi.h>

#include <OTAServer.h>
#include "wifi_secrets.h"

OTAServer otaServer;

void setup()
{
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");

  otaServer.enable();
}

void loop()
{
  otaServer.handleHttpClient();
  otaServer.handleDiscoveries();
}