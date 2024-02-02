/**
 * M5Dial-Cast-Control
 * Control your Cast/Chromecast devices with an M5Dial
 *
 * Uses the ArduCastControl library
 *
 * Apache2.0 License
 * by Lucas Placentino
 */

/**
 * This implements a simple chromecast control example
 * Current status gathered from chromecast will be printed on serial.
 * Button will act as pause and encoder as volume control.
 */
//TODO: add screen and prev next controls

#include <Arduino.h>
#include <ArduinoOTA.h>
#include "ArduCastControl.h"
#include <M5Dial.h>
#include <WiFi.h>

#include "SECRETS.h"

//#define B_SELECT D5
//#define B_LEFT D6
//#define B_RIGHT D7
#define CHROMECASTIP "192.168.1.12"

ArduCastControl cc = ArduCastControl();
bool bSelectPressed = false;
//bool bLeftPressed = false;
//bool bRightPressed = false;
long oldEncoderPosition = -999;
float currentVolume = 1;

void setup()
{

    Serial.begin(115200);
    Serial.println("booted");

    auto cfg = M5.config();
    M5Dial.begin(cfg, true, false);
    M5Dial.Display.clear();

    M5Dial.Display.print("WiFi:");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(500);
    }
    Serial.println("\r\n WiFi Connected.");
    M5Dial.Display.print("Connected.");

    ArduinoOTA.setHostname("chromecastremote");
    ArduinoOTA.begin();

    M5Dial.Display.clear();
    M5Dial.Display.setTextColor(WHITE);
    M5Dial.Display.setTextDatum(middle_center);
    M5Dial.Display.setTextFont(&fonts::Orbitron_Light_32);
    M5Dial.Display.setTextSize(2);

    //pinMode(D8, OUTPUT); // common pin for keys, used for pulldown - should have a pulldown anyway
    //pinMode(B_SELECT, INPUT_PULLUP);
    //pinMode(B_LEFT, INPUT_PULLUP);
    //pinMode(B_RIGHT, INPUT_PULLUP);
}

uint32_t lastScreenUpdate = 0;
uint32_t screenUpatePeriod = 2000;
bool screenUpdated = true;
void clearDisplayAfterDelay()
{
    if (screenUpdated)
    {
        uint32_t now = millis()
        if (lastScreenUpdate + now > screenUpdatePeriod)
        {
            M5Dial.Display.clear();
            screenUpdated = false;
        }
    }
}

uint32_t lastUpdated = 0;
uint32_t updatePeriod = 5000; // 5s

uint32_t bLastUpdated = 0;
uint32_t bUpdatePeriod = 25; // 25ms

uint32_t now;

void loop()
{
    ArduinoOTA.handle();
    // wait for 5s to boot - this is useful in case of a bootloop to keep OTA running
    if (millis() < 10000)
        return;

    if (millis() - lastUpdated > updatePeriod)
    {
        if (cc.getConnection() != WAIT_FOR_RESPONSE)
        {
            cc.dumpStatus();
        }
        int st;

        if (cc.getConnection() == DISCONNECTED)
        {
            Serial.print("Connecting...");
            st = cc.connect(CHROMECASTIP);
            Serial.println(st);
        }
        else
        {
            // at this point, cc.volume and cc.isMuted should be valid
            connection_t c = cc.loop();
            if (c == WAIT_FOR_RESPONSE || c == CONNECT_TO_APPLICATION)
            {
                updatePeriod = 50;
            }
            else if (c == APPLICATION_RUNNING)
            {
                updatePeriod = 500;
                // at this point, all public fields describing the casting
                //(e.g. cc.artist, cc.title) should be valid
            }
            else
            {
                updatePeriod = 5000;
            }
        }
        lastUpdated = millis();
    }
    if (millis() - bLastUpdated > bUpdatePeriod && cc.getConnection() == APPLICATION_RUNNING)
    {
        M5Dial.update();

        currentVolume = cc.volume;
        Serial.println(currentVolume);
        Serial.println((int) (currentVolume * 20));

        //bool prevSelect = bSelectPressed;
        //bool prevLeft = bLeftPressed;
        //bool prevRight = bRightPressed;

        //bSelectPressed = digitalRead(B_SELECT) == LOW;
        //bSelectPressed = M5Dial.BtnA.wasPressed();
        //bRightPressed = digitalRead(B_RIGHT) == LOW;
        //bLeftPressed = digitalRead(B_LEFT) == LOW;

        long newEncoderPosition = M5Dial.Encoder.read();
        Serial.println(newEncoderPosition);

        /*
        if (M5Dial.BtnA.wasReleased()
        {
            if (M5Dial.BtnA.pressedFor(2000)
            {
                M5Dial.Speaker.tone(6000, 20);
                cc.prev();
                M5Dial.Display.drawString("Previous",
                                  M5Dial.Display.width() / 2,
                                  M5Dial.Display.height() / 2);
            }
            else if (M5Dial.BtnA.pressedFor(1000, 2000)
            {
                M5Dial.Speaker.tone(6000, 20);
                cc.next();
                M5Dial.Display.drawString("Next",
                                  M5Dial.Display.width() / 2,
                                  M5Dial.Display.height() / 2);
            }
            else
            {
                M5Dial.Speaker.tone(2000, 20);
                cc.pause(true);
                M5Dial.Display.drawString("Paused",
                                  M5Dial.Display.width() / 2,
                                  M5Dial.Display.height() / 2);
            }
        }
        */
        
        if (M5Dial.BtnA.wasPressed())
        {
            M5Dial.Speaker.tone(2000, 20);
            cc.pause(true);
            M5Dial.Display.clear();
            M5Dial.Display.drawString("Paised",
                              M5Dial.Display.width() / 2,
                              M5Dial.Display.height() / 2);
            screenUpdated = true;
        }
        if (M5Dial.BtnA.pressedFor(1000, 2000))
        {
            M5Dial.Speaker.tone(6000, 20);
            cc.next();
            M5Dial.Display.clear();
            M5Dial.Display.drawString("Next",
                              M5Dial.Display.width() / 2,
                              M5Dial.Display.height() / 2);
            screenUpdated = true;
        }
        if (M5Dial.BtnA.pressedFor(2000))
        {
            M5Dial.Speaker.tone(6000, 20);
            cc.prev();
            M5Dial.Display.clear();
            M5Dial.Display.drawString("Previous",
                              M5Dial.Display.width() / 2,
                              M5Dial.Display.height() / 2);
            screenUpdated = true;
        }

        /*
        if (!bSelectPressed && prevSelect)
        { // select released
            M5Dial.Speaker.tone(2000, 20);
            cc.pause(true);
            M5Dial.Display.drawString("Paused",
                                  M5Dial.Display.width() / 2,
                                  M5Dial.Display.height() / 2);
        }
        */

        /*
        if (!bLeftPressed && prevLeft)
        { // left released
            cc.prev();
        }

        if (!bRightPressed && prevRight)
        { // right released
            cc.next();
        }
        */

        /*
        if (M5Dial.BtnA.pressedFor(1000)) // 1s
        {
            M5Dial.Speaker.tone(6000, 20);
            cc.next();
            M5Dial.Display.drawString("Next",
                                  M5Dial.Display.width() / 2,
                                  M5Dial.Display.height() / 2);
        }
        */

        if (newEncoderPosition != oldEncoderPosition && currentVolume != -1) // -1 means nothing reported
        {
            float newVolume;
            if (newEncoderPosition > oldEncoderPosition)
            {
                M5Dial.Speaker.tone(7000, 20);
                /*
                newVolume = currentVolume + 0.05;
                if (newVolume > 1)
                {
                    newVolume = 1;
                }
                cc.setVolume(newVolume);
                */
                cc.setVolume(true, 0.05); // relative volume
            }
            else
            {
                M5Dial.Speaker.tone(5000, 20);
                /*
                newVolume = currentVolume - 0.05;
                if (newVolume < 0)
                {
                    newVolume = 0;
                }
                cc.setVolume(newVolume);
                */
                cc.setVolume(true, -0.05); // relative volume
            }
            oldEncoderPosition = newEncoderPosition;
            /*
            M5Dial.Display.clear();
            M5Dial.Display.drawString(String(newVolume),
                                  M5Dial.Display.width() / 2,
                                  M5Dial.Display.height() / 2);
            screenUpdated = true;
            */
        }

        bLastUpdated = millis();
        now = millis()

        // clear screen (if updated) after a delay
        if (screenUpdated && now - lastScreenUpdate > screenUpdatePeriod)
        {
                M5Dial.Display.clear();
                screenUpdated = false;
        }
    }
}
