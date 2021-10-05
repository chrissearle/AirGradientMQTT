#ifndef MQTT_H
#define MQTT_H

#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoLog.h>

class MQTT
{
private:
    PubSubClient *pubSubClient;

    char *location;
    char *topic;
    char *username = NULL;
    char *password = NULL;

    void reconnect()
    {
        while (!pubSubClient->connected())
        {
            Log.noticeln("Attempting MQTT connection...");

            String clientId = String("DHT-" + String(location));

            if (pubSubClient->connect(clientId.c_str(), username, password))
            {
                Log.noticeln("connected");
            }
            else
            {
                Log.warningln("failed, rc=%d try again in 5 seconds", pubSubClient->state());
                // Wait 5 seconds before retrying
                delay(5000);
            }
        }
    }

    void publishToTopic(const char *topicName, const char *topicType, float value)
    {
        char *publishTopic = (char *)malloc(50);
        memset(publishTopic, 0, 50);

        sprintf(publishTopic, "%s/%s/%s", topic, topicName, topicType);

#if SENDMQ
        Log.noticeln("Posting");
        char charVal[10];
        dtostrf(value, 4, 4, charVal);
        pubSubClient->publish(publishTopic, charVal);
#endif

        Log.noticeln("%s %F", publishTopic, value);
    }

public:
    MQTT(const char *loc, Client &client, const char *address, const char *top, const char *user, const char *pass)
    {
        location = (char *)malloc(strlen(loc) + 1);
        strncpy(location, loc, strlen(loc));

        topic = (char *)malloc(strlen(top));
        strncpy(topic, top, strlen(top));

        if (user != NULL) {
            username = (char *)malloc(strlen(user));
            strncpy(username, user, strlen(user));
        }

        if (pass != NULL) {
            password = (char *)malloc(strlen(pass));
            strncpy(password, pass, strlen(pass));
        }

        Log.noticeln("Initializing %s", location);

        pubSubClient = new PubSubClient(client);
        pubSubClient->setServer(address, 1883);
    }

    void keepAlive()
    {
        if (!pubSubClient->connected())
        {
            reconnect();
        }

        pubSubClient->loop();
    }

    void publish(int pm1, int pm2, int pm10, int co2, float t, int rh)
    {
        publishToTopic(location, "temperature", t);
        publishToTopic(location, "humidity", rh);
        publishToTopic(location, "co2", co2);
        publishToTopic(location, "pm1", pm1);
        publishToTopic(location, "pm2", pm2);
        publishToTopic(location, "pm10", pm10);
    }
};

#endif
