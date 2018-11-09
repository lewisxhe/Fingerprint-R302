#include <Arduino.h>
#include "fingerprint.h"

#define UART_TX 22
#define UART_RX 23
#define TOUCH_TRIGGER_PIN 21 //low level

// HardwareSerial rSerial(1);
Fingerprint eic(&Serial1);
bool flag = false;

void touchPadAttach(void)
{
    Serial.println("TouchPad Attach");
    flag = true;
}
SysParams_t params;

void setup()
{
    Serial.begin(115200);
    Serial1.begin(56700, SERIAL_8N1, UART_RX, UART_TX);
    attachInterrupt(TOUCH_TRIGGER_PIN, touchPadAttach, FALLING);
    eic.readSysParams(&params);
    Serial.printf("address: %x --packSize : %x  -- %x bps \n", params.address, params.packSize, params.bps);
}

bool loop1(GR_BufID_t id)
{
    while (!flag)
    {
        delay(500);
    }
    flag = false;
    if (eic.entryFinger())
    {
        Serial.println("entery PASS");
        delay(200);
        if (eic.generateFeature(id))
        {
            Serial.println("generater PASS");
            return true;
        }
        else
        {
            Serial.println("generater FAIL");
        }
    }
    else
    {
        Serial.println("entery FAIL");
    }
    return false;
}
void loop()
{
    loop1(GR_CHAR_BUFFER_1);
    loop1(GR_CHAR_BUFFER_2);
    if (eic.mergeFeature())
    {
        Serial.println("merge PASS");
        if (eic.storeFeature(GR_CHAR_BUFFER_1, 0))
        {
            Serial.println("storeFeature PASS");
        }
        else
        {
            Serial.println("storeFeature FAIL");
        }
    }
    else
    {
        Serial.println("merge FAIL");
    }
    while (1)
    {
        if (loop1(GR_CHAR_BUFFER_1))
        {
            if (eic.searchFingerprint(GR_CHAR_BUFFER_1, 0, 10))
            { 
                Serial.println("Fingerprint valid PASS");
            }
            else
            {
                Serial.println("Fingerprint valid FAIL");
            }
        }
    }
}
