#include <Arduino.h>
#include "OTAServer.h"

OTAServer::OTAServer() : httpServer(HTTP_SERVER_PORT) {}

void OTAServer::setRoutes()
{
  httpServer.on("/", HTTP_GET, [this]()
                {
    httpServer.sendHeader("Location", "https://www.youtube.com/watch?v=dQw4w9WgXcQ");
    httpServer.send(302, "text/plain", "Redirecting..."); });
  httpServer.on("/update", HTTP_POST, [this]()
                { this->handleUpdate(); }, [this]()
                { this->handleUpdateUpload(); });
}

void OTAServer::enable()
{
  udp.begin(UDP_PORT);
  Serial.println("UDP discovery server started");
  setRoutes();
  httpServer.begin();
  Serial.println("HTTP server started");
}

void OTAServer::handleHttpClient()
{
  httpServer.handleClient();
}

void OTAServer::handleUpdate()
{
  ESP.restart();
}

void OTAServer::handleUpdateUpload()
{
  HTTPUpload &upload = httpServer.upload();

  if (upload.status == UPLOAD_FILE_START)
  {
    Serial.printf("Updating firmware over the air: %s\n", upload.filename.c_str());
    if (!Update.begin(UPDATE_SIZE_UNKNOWN))
      Update.printError(Serial);
  }
  else if (upload.status == UPLOAD_FILE_WRITE)
  {
    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
      Update.printError(Serial);
  }
  else if (upload.status == UPLOAD_FILE_END)
  {
    if (Update.end(true))
    {
      Serial.println("Firmware updated. Rebooting...");
      ESP.restart();
    }
  }
}

void OTAServer::handleDiscoveries()
{
  int packetSize = udp.parsePacket();
  if (packetSize)
  {
    char incomingPacket[255];
    int len = udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }

    if (strcmp(incomingPacket, DISCOVERY_PHRASE) == 0)
    {
      udp.beginPacket(udp.remoteIP(), udp.remotePort());
      udp.write((const uint8_t *)DEVICE_ID, strlen(DEVICE_ID));
      udp.endPacket();
    }
  }
}