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
#include "M5Dial.h"

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

    ArduinoOTA.setHostname("chromecastremote");
    ArduinoOTA.begin();

    auto cfg = M5.config();
    M5Dial.begin(cfg, true, false);

    //pinMode(D8, OUTPUT); // common pin for keys, used for pulldown - should have a pulldown anyway
    //pinMode(B_SELECT, INPUT_PULLUP);
    //pinMode(B_LEFT, INPUT_PULLUP);
    //pinMode(B_RIGHT, INPUT_PULLUP);
}

uint32_t lastUpdated = 0;
uint32_t updatePeriod = 5000; // 5s

uint32_t bLastUpdated = 0;
uint32_t bUpdatePeriod = 25; // 25ms

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

        bool prevSelect = bSelectPressed;
        //bool prevLeft = bLeftPressed;
        //bool prevRight = bRightPressed;

        //bSelectPressed = digitalRead(B_SELECT) == LOW;
        bSelectPressed = M5Dial.BtnA.wasPressed();
        //bRightPressed = digitalRead(B_RIGHT) == LOW;
        //bLeftPressed = digitalRead(B_LEFT) == LOW;

        long newEncoderPosition = M5Dial.Encoder.read();
        Serial.println(newEncoderPosition);

        if (!bSelectPressed && prevSelect)
        { // select released
            cc.pause(true);
        }

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

        if (newEncoderPosition != oldEncoderPosition && currentVolume != -1) // -1 means nothing reported
        {
            if (newEncoderPosition > oldEncoderPosition)
            {
                float newVolume = currentVolume + 0.05;
                if (newVolume > 1)
                {
                    newVolume = 1;
                }
                cc.setVolume(newVolume);
            }
            else
            {
                float newVolume = currentVolume - 0.05;
                if (newVolume < 0)
                {
                    newVolume = 0;
                }
                cc.setVolume(newVolume);
            }
            oldEncoderPosition = newEncoderPosition;
        }

        bLastUpdated = millis();
    }
}
