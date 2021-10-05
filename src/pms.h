#ifndef PMS_H
#define PMS_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoLog.h>

typedef struct pms5003data
{
    uint16_t framelen;
    uint16_t pm10_standard, pm25_standard, pm100_standard;
    uint16_t pm10_env, pm25_env, pm100_env;
    uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
    uint16_t unused;
    uint16_t checksum;
    bool valid; // Must come last - else memcpy will overwrite it
} PMSData;

class PmsSensor
{
private:
    SoftwareSerial *pmsSerial;
    PMSData *pmsData;
    bool validData;

    struct pms5003data readPMSdata(Stream *s)
    {
        const int readyByte = 0x42;
        const int maxTriesForReady = 50;
        const int bytesNeeded = 32;

        struct pms5003data data;

        data.valid = false;

        if (!s->available())
        {
            Log.errorln("Stream has no bytes available");
            return data;
        }

        int count = 0;
        while (s->peek() != readyByte && count < maxTriesForReady)
        {
            count = count + 1;
            s->read();
        }

        if (count == maxTriesForReady) {
            Log.infoln("Stream not ready");
            return data;
        }

        if (s->available() < bytesNeeded)
        {
            Log.errorln("Stream not correct length");
            return data;
        }

        uint8_t buffer[32];
        uint16_t sum = 0;
        s->readBytes(buffer, 32);

        for (uint8_t i = 0; i < 30; i++)
        {
            sum += buffer[i];
        }

        uint16_t buffer_u16[15];
        for (uint8_t i = 0; i < 15; i++)
        {
            buffer_u16[i] = buffer[2 + i * 2 + 1];
            buffer_u16[i] += (buffer[2 + i * 2] << 8);
        }

        // Create the return - writes all the sensor data - stops just short of the valid flag
        memcpy((void *)&data, (void *)buffer_u16, 30);

        if (sum != data.checksum)
        {
            Log.errorln("Checksum failure %d %d", sum, data.checksum);
            return data;
        }

        data.valid = true;
        return data;
    }

public:
    PmsSensor()
    {
        validData = false;

        pmsSerial = new SoftwareSerial(D5, D6);
        pmsSerial->begin(9600);
    }

    void refresh()
    {
        Log.infoln("Refreshing PMS");

        struct pms5003data data = readPMSdata(pmsSerial);

        validData = data.valid;

        if (data.valid)
        {
            Log.verboseln("Refreshing PMS - Valid");

            pmsData = &data;
        }
        else
        {
            Log.verboseln("Refreshing PMS - Invalid");
        }
    }

    bool isValid() {
        return validData;
    }
    
    uint16_t pm10Standard()
    {
        if (validData)
        {
            return pmsData->pm10_standard;
        }

        return 0;
    }

    uint16_t pm25Standard()
    {
        if (validData)
        {
            return pmsData->pm25_standard;
        }

        return 0;
    }

    uint16_t pm100Standard()
    {
        if (validData)
        {
            return pmsData->pm100_standard;
        }

        return 0;
    }

    uint16_t pm10Env()
    {
        if (validData)
        {
            return pmsData->pm10_env;
        }

        return 0;
    }

    uint16_t pm25Env()
    {
        if (validData)
        {
            return pmsData->pm25_env;
        }

        return 0;
    }

    uint16_t pm100Env()
    {
        if (validData)
        {
            return pmsData->pm100_env;
        }

        return 0;
    }

    uint16_t particles03()
    {
        if (validData)
        {
            return pmsData->particles_03um;
        }

        return 0;
    }
    uint16_t particles05()
    {
        if (validData)
        {
            return pmsData->particles_05um;
        }

        return 0;
    }
    uint16_t particles10()
    {
        if (validData)
        {
            return pmsData->particles_10um;
        }

        return 0;
    }
    uint16_t particles25()
    {
        if (validData)
        {
            return pmsData->particles_25um;
        }

        return 0;
    }
    uint16_t particles50()
    {
        if (validData)
        {
            return pmsData->particles_50um;
        }

        return 0;
    }
    uint16_t particles100()
    {
        if (validData)
        {
            return pmsData->particles_100um;
        }

        return 0;
    }
};

#endif