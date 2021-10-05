#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

#include <ArduinoLog.h>
#include <AirGradient.h>

class Sensors
{
private:
    AirGradient *airGradient;

    bool hasCO2;
    bool hasSHT;

    int co2;
    float t;
    int rh;
    
public:
    Sensors(bool co2enabled, bool shtEnabled)
    {
        airGradient = new AirGradient(true, 9600);

        hasCO2 = co2enabled;
        hasSHT = shtEnabled;

        co2 = 0;
        t = 0;
        rh = 0;

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


    unsigned int getCO2()
    {
        return co2;
    }

    float getT()
    {
        return t;
    }

    unsigned int getRH()
    {
        return rh;
    }
};

#endif
