#ifndef SCREEN_H
#define SCREEN_H

#include <Arduino.h>

#include <SSD1306Wire.h>
#include <Wire.h>

class Screen
{
private:
    SSD1306Wire *display;

public:
    Screen(uint8_t address)
    {
        display = new SSD1306Wire(address, SDA, SCL);
        display->init();
        display->flipScreenVertically();
    }

    void showTextRectangle(String ln1, String ln2, boolean small)
    {
        display->clear();
        display->setTextAlignment(TEXT_ALIGN_LEFT);
        if (small)
        {
            display->setFont(ArialMT_Plain_16);
        }
        else
        {
            display->setFont(ArialMT_Plain_24);
        }
        display->drawString(32, 16, ln1);
        display->drawString(32, 36, ln2);
        display->display();
    }

    void showInt(String title, unsigned int value) {
        showTextRectangle(title, String(value), false);
    }

    void showFloat(String title, float value) {
        char output[6];
        snprintf(output, 5, "%.2f", value);
        showTextRectangle(title, String(output), false);
    }

    void showUnavailable(String title) {
        showTextRectangle("No", title, false);
    }
};

#endif
