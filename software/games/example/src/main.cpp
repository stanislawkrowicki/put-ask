#include <Arduino.h>
#include <WiFi.h>

#include <OTAServer.h>
#include "wifi_secrets.h"

OTAServer otaServer;

void setup()
{
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void loop()
{
  otaServer.connectionLoop();
  otaServer.handleHttpClient();
  otaServer.handleDiscoveries();
}