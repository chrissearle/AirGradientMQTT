#include <Arduino.h>

#include <WiFiManager.h>
#include <ESP8266WiFi.h>
#include <ArduinoLog.h>

#include "screen.h"
#include "mqtt.h"
#include "sensors.h"

#include "pms.h"

#define SENDMQ 0

Screen *display;
MQTT *mqtt;
Sensors *sensors;
PmsSensor *pms;

WiFiClient wifiClient;

bool mqttEnabled = false;
bool co2Enabled = false;
bool shtEnabled = false;

const int displayDelay = 1000;

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
  }
  else
  {
    IPAddress ip = WiFi.localIP();

    display->showTextRectangle(String(ip[0]), String(ip[1]), false);

    delay(2000);

    display->showTextRectangle(String(ip[2]), String(ip[3]), false);

    delay(2000);

    Log.infoln("Wifi connected");
  }
}

void setupDisplay()
{
  Log.infoln("Initializing Display");

  display = new Screen(0x3c);
  display->showTextRectangle("Init", String(ESP.getChipId(), HEX), true);
}

void setupSensors()
{
  Log.infoln("Initializing Sensors");

  sensors = new Sensors(co2Enabled, shtEnabled);
  pms = new PmsSensor();
}

void setupMQTT()
{
  if (mqttEnabled)
  {
    Log.infoln("Initializing MQTT");

    mqtt = new MQTT("LocationName", wifiClient, "192.168.1.xxx", "TopicName", NULL, NULL);
  }
  else
  {
    Log.infoln("MQTT not enabled");
  }
}

void setup()
{
  Serial.begin(9600);

  Log.begin(LOG_LEVEL_INFO, &Serial);

  setupDisplay();
  connectToWifi();
  setupSensors();
  setupMQTT();

  Log.infoln("Setup Complete");
}

unsigned int processInt(String name, int value, bool enabled)
{
  if (enabled)
  {
    display->showInt(name, value);
  }
  else
  {
    display->showUnavailable(name);
  }

  return value;
}

float processFloat(String name, float value, bool enabled)
{
  if (enabled)
  {
    display->showFloat(name, value);
  }
  else
  {
    display->showUnavailable(name);
  }

  return value;
}

void loop()
{
  // PMS3005 Data

  pms->refresh();

  if (pms->isValid())
  {
    processInt("10S", pms->pm10Standard(), true);
    delay(displayDelay);
    processInt("25S", pms->pm25Standard(), true);
    delay(displayDelay);
    processInt("100S", pms->pm100Standard(), true);
    delay(displayDelay);
    processInt("10E", pms->pm10Env(), true);
    delay(displayDelay);
    processInt("25E", pms->pm25Env(), true);
    delay(displayDelay);
    processInt("100E", pms->pm100Env(), true);
    delay(displayDelay);
    processInt("P03", pms->particles03(), true);
    delay(displayDelay);
    processInt("P05", pms->particles05(), true);
    delay(displayDelay);
    processInt("P10", pms->particles10(), true);
    delay(displayDelay);
    processInt("P25", pms->particles25(), true);
    delay(displayDelay);
    processInt("P50", pms->particles50(), true);
    delay(displayDelay);
    processInt("P100", pms->particles100(), true);
    delay(displayDelay);
  }

  // Air Gradient Data

  /*
  sensors->refresh();

  int co2 = processInt("CO2", sensors->getCO2(), co2Enabled);

    delay(displayDelay);

  float t = processFloat("T", sensors->getT(), shtEnabled);

    delay(displayDelay);

  int rh = processInt("RH", sensors->getRH(), shtEnabled);

    delay(displayDelay);

  if (mqttEnabled) {
    Log.infoln("Publishing to MQTT");

    mqtt->keepAlive();
    mqtt->publish(pm1, pm2, pm10, co2, t, rh);
  }
  */
}