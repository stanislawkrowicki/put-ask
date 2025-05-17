#pragma once

#include <Update.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiUdp.h>

#define HTTP_SERVER_PORT 80
#define UDP_PORT 25545

#define DEVICE_ID "PPBOY"
#define DISCOVERY_PHRASE "DISCOVER_PPBOYS"

/* Check WiFi status while connecting every 1s */
constexpr unsigned int WIFI_CONN_CHECK_INTERVAL = 1000;
/* Check if WiFi lost connection every 10s*/
constexpr unsigned int WIFI_STATUS_CHECK_INTERVAL = 10000;

class OTAServer
{
private:
    unsigned long lastWifiCheck = 0;
    unsigned int wifiCheckInterval = WIFI_CONN_CHECK_INTERVAL;
    bool wifiConnected;
    
    WiFiUDP udp;
    WebServer httpServer;

    void setRoutes();
    void onUpdateFinish();
    void handleUpdateUpload();

public:
    OTAServer();
    /* Tries to connect to Wi-Fi until it becomes connected,
    then enables the OTAServer.
    After connection this function has no effect.
    You need to call WiFi.begin() before using.*/
    void connectionLoop();
    void enable();
    void handleHttpClient();
    void handleDiscoveries();
};