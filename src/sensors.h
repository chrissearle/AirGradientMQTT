#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

#include <ArduinoLog.h>
#include <AirGradient.h>

class Sensors
{
private:
    AirGradient *airGradient;

    bool hasSHT;

    float t;
    int rh;
    
public:
    Sensors(bool shtEnabled)
    {
        airGradient = new AirGradient(true, 9600);

        hasSHT = shtEnabled;

        t = 0;
        rh = 0;

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
