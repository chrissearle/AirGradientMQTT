#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

#include <ArduinoLog.h>
#include <AirGradient.h>

class Sensors
{
private:
    AirGradient *airGradient;

    bool hasPM;
    bool hasCO2;
    bool hasSHT;

    int pm1;
    int pm2;
    int pm10;
    int co2;
    float t;
    int rh;

public:
    Sensors(bool pm2Enabled, bool co2enabled, bool shtEnabled)
    {
        airGradient = new AirGradient();

        hasPM = pm2Enabled;
        hasCO2 = co2enabled;
        hasSHT = shtEnabled;

        pm1 = -1;
        pm2 = -1;
        pm10 = -1;
        co2 = -1;
        t = -1.0;
        rh = -1;

        if (hasPM)
        {
            Log.infoln("Initializing PM");

            airGradient->PMS_Init();
        }
        else
        {
            Log.infoln("PM2 not enabled");
        }

        if (hasCO2)
        {
            Log.infoln("Initializing CO2");

            airGradient->CO2_Init();
        }
        else
        {
            Log.infoln("CO2 not enabled");
        }

        if (hasSHT)
        {
            Log.infoln("Initializing SHT");

            airGradient->TMP_RH_Init(0x44);
        }
        else
        {
            Log.infoln("SHT not enabled");
        }
    }

    void refresh()
    {
        if (hasPM)
        {
            Log.infoln("Refreshing PM");
            PMS_DATA pmsData = airGradient->getPM_Raw();
            pm1 = pmsData.PM_AE_UG_1_0;
            pm2 = pmsData.PM_AE_UG_2_5;
            pm10 = pmsData.PM_AE_UG_10_0;
            Log.infoln("Read PM1 %d", pm1);
            Log.infoln("Read PM2 %d", pm2);
            Log.infoln("Read PM10 %d", pm10);
        }

        if (hasCO2)
        {
            Log.infoln("Refreshing CO2");
            co2 = airGradient->getCO2_Raw();
            Log.infoln("Read CO2 %d", co2);
        }

        if (hasSHT)
        {
            Log.infoln("Refreshing SHT");
            TMP_RH sht = airGradient->periodicFetchData();
            t = sht.t;
            rh = sht.rh;
            Log.infoln("Read SHT T %.2f", t);
            Log.infoln("Read SHT RH %d", rh);
        }
    }

    int getPM1()
    {
        return pm1;
    }

    int getPM2()
    {
        return pm2;
    }

    int getPM10()
    {
        return pm10;
    }

    int getCO2()
    {
        return co2;
    }

    float getT()
    {
        return t;
    }

    int getRH()
    {
        return rh;
    }
};

#endif
