#include <Arduino.h>

#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <ArduinoLog.h>

#include "screen.h"
#include "mqtt.h"
#include "sensors.h"

#define SENDMQ 0

Screen *display;
MQTT *mqtt;
Sensors *sensors;

WiFiClient wifiClient;

bool mqttEnabled = false;

void connectToWifi()
{
  Log.infoln("Connecting to wifi");

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
    
    Log.infoln("Wifi connected");
  }
}

void setupDisplay() {
  Log.infoln("Initializing Display");

  display = new Screen(0x3c);
  display->showTextRectangle("Init", String(ESP.getChipId(), HEX), true);
}

void setupSensors() {
  Log.infoln("Initializing Sensors");

  sensors = new Sensors(true, false, false);
}

void setupMQTT() {
    if (mqttEnabled) {
    Log.infoln("Initializing MQTT");

    mqtt = new MQTT("LocationName", wifiClient, "192.168.1.xxx", "TopicName", NULL, NULL);
  } else {
    Log.infoln("MQTT not enabled");
  }
}

void setup()
{
  Serial.begin(115200);

  Log.begin(LOG_LEVEL_VERBOSE, &Serial);

  setupDisplay();
  connectToWifi();
  setupSensors();
  setupMQTT();

  Log.infoln("Setup Complete");
}

int processInt(String name, int value) {
  if (value >= 0) {
    display->showInt(name, value);
  } else {
    display->showUnavailable(name);
  }

  return value;
}

float processFloat(String name, float value) {
  if (value >= 0) {
    display->showFloat(name, value);
  } else {
    display->showUnavailable(name);
  }

  return value;
}

void loop()
{
  sensors->refresh();

  int pm2 = processInt("PM2", sensors->getPM2());

  delay(3000);

  int co2 = processInt("CO2", sensors->getPM2());

  delay(3000);

  float t = processFloat("T", sensors->getT());

  delay(3000);

  int rh = processInt("RH", sensors->getRH());

  delay(3000);

  if (mqttEnabled) {
    Log.infoln("Publishing to MQTT");

    mqtt->keepAlive();
    mqtt->publish(pm2, co2, t, rh);
  }
}