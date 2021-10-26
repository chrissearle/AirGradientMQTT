#ifndef CO2_H
#define CO2_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoLog.h>

typedef struct co2ReadResult
{
    int co2 = -1;
    bool success = false;
} CO2Data;

class CO2Sensor
{
private:
    SoftwareSerial *co2Serial;
    CO2Data *co2Data;
    bool validData;

    struct co2ReadResult readCO2data(Stream *s)
    {
        const byte co2Command[] = {0xFE, 0X44, 0X00, 0X08, 0X02, 0X9F, 0X25};

        co2ReadResult result;

        byte response[] = {0, 0, 0, 0, 0, 0, 0};

        int retry = 0;

        while (!(s->available()))
        {
            Log.infoln("Sending command");

            retry++;

            s->write(co2Command, 7);

            delay(50);

            if (retry > 10)
            {
                Log.errorln("Sending command - max retries reached");

                result.success = false;
                result.co2 = -1;

                return result;
            }
        }

        int timeout = 0;

        while (s->available() < 7)
        {
            timeout++;

            if (timeout > 10)
            {
                while (s->available())
                    s->read();
                break;
            }

            delay(50);
        }

        for (int i = 0; i < 7; i++)
        {
            int byte = s->read();

            if (byte == -1)
            {
                Log.errorln("Read failed");

                result.success = false;
                result.co2 = -1;

                return result;
            }
            response[i] = byte;
        }

        int high = response[3];
        int low = response[4];
        unsigned long val = high * 256 + low;

        result.success = true;
        result.co2 = val;

        return result;
    }

public:
    CO2Sensor()
    {
        co2Serial = new SoftwareSerial(D4, D3);
        co2Serial->begin(9600);
    }

    void refresh()
    {
        Log.infoln("Refreshing CO2");

        struct co2ReadResult data = readCO2data(co2Serial);

        validData = data.success;

        if (data.success)
        {
            Log.verboseln("Refreshing CO2 - Valid");

            co2Data = &data;
        }
        else
        {
            Log.verboseln("Refreshing CO2 - Invalid");
        }
    }

    bool isValid()
    {
        return validData;
    }

    uint16_t co2()
    {
        if (validData)
        {
            return co2Data->co2;
        }

        return 0;
    }
};

#endif