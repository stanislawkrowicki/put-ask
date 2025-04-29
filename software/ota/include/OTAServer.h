#pragma once

#include <Update.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiUdp.h>

#define HTTP_SERVER_PORT 80
#define UDP_PORT 25545

#define DEVICE_ID "PPBOY"
#define DISCOVERY_PHRASE "DISCOVER_PPBOYS"

class OTAServer
{
private:
    WiFiUDP udp;
    WebServer httpServer;

    void setRoutes();
    void handleUpdate();
    void handleUpdateUpload();

    void handleDiscoveries();

public:
    OTAServer();
    void enable();
};