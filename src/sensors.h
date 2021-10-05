#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

#include <ArduinoLog.h>
#include <AirGradient.h>

class Sensors
{
private:
    AirGradient *airGradient;

    bool hasPM2;
    bool hasCO2;
    bool hasSHT;

    int pm2;
    int co2;
    TMP_RH sht;


public:
    Sensors(bool pm2Enabled, bool co2enabled, bool shtEnabled)
    {
        airGradient = new AirGradient();

        hasPM2 = pm2Enabled;
        hasCO2 = co2enabled;
        hasSHT = shtEnabled;
        
        pm2 = -1;
        co2 = -1;
        sht.t = -1.0;
        sht.rh = -1;

        if (hasPM2) {
            Log.infoln("Initializing PMS");

            airGradient->PMS_Init();
        } else {
            Log.infoln("PM2 not enabled");
        }

        if (hasCO2) {
            Log.infoln("Initializing CO2");

            airGradient->CO2_Init();
        } else {
            Log.infoln("CO2 not enabled");
        }

        if (hasSHT) {
            Log.infoln("Initializing SHT");

            airGradient->TMP_RH_Init(0x44);
        } else {
            Log.infoln("SHT not enabled");
        }
    }

    void refresh() {
        if (hasPM2) {
            Log.infoln("Refreshing PM2");
            pm2 = airGradient->getPM2_Raw();
            Log.infoln("Read PM2 %d", pm2);
        }

        if (hasCO2) {
            Log.infoln("Refreshing CO2");
            co2 = airGradient->getCO2_Raw();
            Log.infoln("Read CO2 %d", co2);
        }

        if (hasSHT) {
            Log.infoln("Refreshing SHT");
            sht = airGradient->periodicFetchData();
            Log.infoln("Read SHT T %.2f", sht.t);
            Log.infoln("Read SHT RH %d", sht.rh);
        }
    }

    int getPM2() {
        return pm2;
    }

    int getCO2() {
        return co2;
    }

    float getT() {
        return sht.t;
    }

    int getRH() {
        return sht.rh;
    }
};

#endif
