#include <Arduino.h>

#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <ArduinoLog.h>
#include <AirGradient.h>

#include "screen.h"
#include "mqtt.h"

#define SENDMQ 0

Screen *display;
MQTT *mqtt;

WiFiClient wifiClient;

AirGradient ag = AirGradient();

bool hasPM2 = true;
bool hasCO2 = false;
bool hasSHT = false;

bool mqttEnabled = false;

void connectToWifi()
{
  WiFiManager wifiManager;

  String HOTSPOT = "AIRGRADIENT-" + String(ESP.getChipId(), HEX);

  wifiManager.setTimeout(120);

  if (!wifiManager.autoConnect((const char *)HOTSPOT.c_str()))
  {
    Log.errorln("failed to connect and hit timeout");
    delay(3000);

    ESP.restart();
    delay(5000);
  } else {
    IPAddress ip = WiFi.localIP();

    display->showTextRectangle(String(ip[0]), String(ip[1]), false);

    delay(2000);

    display->showTextRectangle(String(ip[2]), String(ip[3]), false);

    delay(2000);
  }
}

void setup()
{
  Serial.begin(115200);

  Log.begin(LOG_LEVEL_VERBOSE, &Serial);

  Log.infoln("Initializing Display");

  display = new Screen(0x3c);
  display->showTextRectangle("Init", String(ESP.getChipId(), HEX), true);

  if (hasPM2) {
    Log.infoln("Initializing PMS");

    ag.PMS_Init();
  } else {
    Log.infoln("PM2 not enabled");
  }

  if (hasCO2) {
    Log.infoln("Initializing CO2");

    ag.CO2_Init();
  } else {
    Log.infoln("CO2 not enabled");
  }


  if (hasSHT) {
    Log.infoln("Initializing SHT");

    ag.TMP_RH_Init(0x44);
  } else {
    Log.infoln("SHT not enabled");
  }

  Log.infoln("Connecting to wifi");

  connectToWifi();

  Log.infoln("Wifi connected");

  delay(2000);

  if (mqttEnabled) {
    Log.infoln("Initializing MQTT");

    mqtt = new MQTT("LocationName", wifiClient, "192.168.1.xx", "TopicName", NULL, NULL);
  } else {
    Log.infoln("MQTT not enabled");
  }

  Log.infoln("Setup Complete");
}

void loop()
{
  int PM2 = -1;

  if (hasPM2) {
    Log.infoln("Reading PM2");

    PM2 = ag.getPM2_Raw();
  
    Log.infoln("Read PM2 %d", PM2);

    display->showTextRectangle("PM2", String(PM2), false);
  } else {
    display->showTextRectangle("No", "PM2", false);
  }

  delay(3000);

  int CO2 = -1;

  if (hasCO2) {
    Log.infoln("Reading CO2");

    CO2 = ag.getCO2_Raw();
  
    Log.infoln("Read CO2 %d", CO2);

    display->showTextRectangle("CO2", String(CO2), false);
  } else {
    display->showTextRectangle("No", "CO2", false);
  }

  delay(3000);

  TMP_RH result;
  result.t = -1.0;
  result.rh = -1;

  if (hasSHT) {
    Log.infoln("Reading SHT");

    TMP_RH result = ag.periodicFetchData();

    Log.infoln("Read SHT %.2f %d", result.t, result.rh);

    display->showTextRectangle(String(result.t), String(result.rh), false);
  } else {
    display->showTextRectangle("No", "SHT", false);
  }

  delay(3000);

  if (mqttEnabled) {
    Log.infoln("Publishing to MQTT");

    mqtt->keepAlive();
    mqtt->publish(PM2, CO2, result.t, result.rh);
  }
}