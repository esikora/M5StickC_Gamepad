#include <Arduino.h>
#include <M5StickC.h>
#include <Wire.h>

#include "GamepadBLE.h"
#include "AXP192_BLEService.h"
#include "M5StickC_PowerManagement.h"

#define JOY_ADDR 0x52

static const uint16_t image_data_icon_bluetooth[384] = {
    0x0000, 0x0000, 0x0000, 0x0193, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x0193, 0x0000, 0x0000, 0x0000, 
    0x0000, 0x0000, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x0000, 0x0000, 
    0x0000, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x1252, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x0000, 
    0x0212, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0xadfb, 0x2273, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x0231, 
    0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0xcebd, 0xcebd, 0x1232, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0xcebd, 0xffff, 0xbe3b, 0x0a12, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0xcebd, 0xbe5c, 0xf7bf, 0x9d9a, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x2ab3, 0x09f2, 0x09f2, 0xcebd, 0x8d39, 0x53b6, 0xffff, 0x7cd8, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x53b6, 0xf7df, 0x4bb6, 0x09f2, 0xcebd, 0x8d39, 0x09f2, 0x9d9a, 0xffff, 0x4355, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x9d7a, 0xffdf, 0x53d6, 0xcebd, 0x8d39, 0x6416, 0xffff, 0x9539, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x9d9a, 0xffdf, 0xef9e, 0xdf1d, 0xffff, 0x8519, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x9d7a, 0xffff, 0xffff, 0x7cd8, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x7cd8, 0xffff, 0xffff, 0x53d6, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x84f9, 0xffff, 0xf7bf, 0xef9e, 0xffdf, 0x5bd6, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x8519, 0xffff, 0x6437, 0xcebd, 0x8d39, 0x8d19, 0xffff, 0x6417, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x53b6, 0xffdf, 0x5bf6, 0x09f2, 0xcebd, 0x8d39, 0x09f2, 0xa5ba, 0xffff, 0x3b35, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x32f4, 0x09f2, 0x09f2, 0xcebd, 0x8d39, 0x2293, 0xe77e, 0xb63b, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0xcebd, 0x957a, 0xcebd, 0xd6fd, 0x1a52, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0xcebd, 0xffdf, 0xef7e, 0x2ab3, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0xcebd, 0xf7df, 0x4355, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 
    0x0212, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0xcebc, 0x6416, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x0231, 
    0x0000, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x53d6, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x0000, 
    0x0000, 0x0000, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f1, 0x0000, 0x0000, 
    0x0000, 0x0000, 0x0000, 0x01d1, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x01d1, 0x0000, 0x0000, 0x0000
};

typedef struct {
    const uint16_t *data;
    uint16_t width;
    uint16_t height;
    uint8_t dataSize;
} tImage;

static const tImage icon_bluetooth = { image_data_icon_bluetooth, 16, 24, 16 };

GamepadBLE gamepad;

AXP192_BLEService axp192ble;
M5StickC_PowerManagement axp192PowMan;

uint8_t joyRawCenterX = 122; // @TODO determine dynamically, e.g. no user action for x seconds and value within specified range
uint8_t joyRawCenterY = 122; // @TODO determine dynamically, e.g. no user action for x seconds and value within specified range

uint16_t curSlotNr = 0;
static const uint16_t numSlots = 20;
static const uint32_t slotTimeMicros = 50000;

uint64_t timeStartMicros = 0;
uint64_t timeEndMicros = 0;
uint32_t timeDeltaMicros = 0;
uint32_t timeDeltaMaxMicros = 0;
uint32_t timeDeltaMaxInCycleMicros = 0;

char strOut[200];

BLEServer *pServer = nullptr;

void setupBLE()
{
    // Initialize bluetooth device
    std::string deviceStr = "ESP32 BLE Controller";
    BLEDevice::init(deviceStr);
    BLEDevice::setPower(ESP_PWR_LVL_P9);

    // Create BLE GATT server
    pServer = BLEDevice::createServer();
}

void setup()
{
    M5.begin();

    axp192PowMan.start();

    M5.Lcd.setTextFont(4);
    M5.Lcd.setCursor(70, 0, 4);
    M5.Lcd.println(("Joy"));

    // Setup I2C communiation for JoyC (grove port)
    Wire.begin(32, 33, 400000);

    setupBLE();

    gamepad.start(pServer);
    axp192ble.start(pServer);
}

void processAxp()
{
    // Read and process AXP192 data
    axp192PowMan.readAndProcessData();
    
    // Debug output
    axp192PowMan.printStatusToString(strOut);
    Serial.println(strOut);

    // Set battery level of gamepad
    gamepad.setBatteryLevel( (uint8_t) axp192PowMan.getBatteryLevelPercent() );

    // Update AXP192 BLE service data
    axp192ble.setBatVoltage( axp192PowMan.getBatVoltage() );
    axp192ble.setBatPower( axp192PowMan.getBatPower() );
    axp192ble.setBatChargeCurrent( axp192PowMan.getChargeCurrent() );
    axp192ble.setCoulombData( axp192PowMan.getCoulombData() );
    axp192ble.setCurrentDirection( axp192PowMan.getCurrentDirection() );
    axp192ble.setACInPresent( axp192PowMan.isACInPresent() );
    axp192ble.setVBusPresent( axp192PowMan.isVBusPresent() );
}

void printRtcTimestamp()
{
    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;

    M5.Rtc.GetTime(&RTC_TimeStruct);
    M5.Rtc.GetData(&RTC_DateStruct);
    
    sprintf(strOut, "%04d-%02d-%02d, %02d:%02d:%02d: ",
        RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date,
        RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);

    Serial.print(strOut);
}

void loop()
{
    timeStartMicros = micros();

    //Serial.println("[V][M5StickC_GamepadApp.cpp] loop(): >> loop");
    //Serial.flush();

    /* ----- Read gamepad controls and send to host ------ */

    uint8_t joyRawX = joyRawCenterX;
    uint8_t joyRawY = joyRawCenterY;
    uint8_t joyPressed = 0;

    Wire.requestFrom(JOY_ADDR, 3);

    if (Wire.available()) {
        joyRawX = Wire.read();
        joyRawY = Wire.read();
        joyPressed = Wire.read();
    }

    //StickAxis_t joyScaledX = joyRawX << 8;
    //StickAxis_t joyScaledY = joyRawY << 8;

    GamepadBLE::StickAxis_t joyScaledX = (joyRawX - joyRawCenterX) << 8;
    GamepadBLE::StickAxis_t joyScaledY = (joyRawY - joyRawCenterY) << 8;

    //GamepadBLE::StickAxis_t joyScaledX = (joyRawX - joyRawCenterX);
    //GamepadBLE::StickAxis_t joyScaledY = (joyRawY - joyRawCenterY);

    gamepad.setLeftStick(joyScaledX, joyScaledY);
    gamepad.setButtonA(joyPressed);
    gamepad.updateBLEdata();

    //sprintf(logStr, "x: %d (raw: %d), y: %d (raw: %d), b: %d\n", joyScaledX, joyRawX, joyScaledY, joyRawY, joyPressed);
    //Serial.print(logStr);

    /* ----- Update display ----- */

    // Do every second slot
    if (curSlotNr % 2 == 0)
    {
        M5.Lcd.setCursor(100, 50, 4);
        M5.Lcd.printf("X:%d      ", joyRawX - joyRawCenterX);
        M5.Lcd.setCursor(100, 80, 4);
        M5.Lcd.printf("Y:%d      ", joyRawY - joyRawCenterY);
        M5.Lcd.setCursor(100, 110, 4);
        M5.Lcd.printf("B:%d      ", joyPressed);
    }

    // Do in slot 1 and 11
    if (curSlotNr % 10 == 1)
    {
        if (gamepad.isConnected()) {
            
            uint16_t i = 0;

            for (uint16_t imgY = 0; imgY < icon_bluetooth.height; ++imgY)
            {
                for (uint16_t imgX = 0; imgX < icon_bluetooth.width; ++imgX)
                {
                    M5.Lcd.drawPixel(imgX, imgY, icon_bluetooth.data[i]);
                    ++i;
                }
            }
        }
        else {
            // Draw rectangle with background colour
            M5.Lcd.fillRect(0, 0, 16, 24, 0);
        }
    }

    // Do in slot 2
    if (curSlotNr % 20 == 3)
    {
        printRtcTimestamp();
        processAxp();
    }

    
    // Do in last slot of each cycle
    if (curSlotNr == numSlots - 1)
    {
        sprintf(strOut, "Duration of loop execution in microseconds: %d (last), %d (max in cylce), %d (max overall)",
                timeDeltaMicros,
                timeDeltaMaxInCycleMicros,
                timeDeltaMaxMicros);

        Serial.println(strOut);
    }
    
    /* ----- Update slot number and compute duration stats ----- */
    
    if (curSlotNr == 0) {
        timeDeltaMaxInCycleMicros = 0;
    }

    curSlotNr = (curSlotNr + 1) % numSlots;

    //Serial.println("[V][M5StickC_GamepadApp.cpp] loop(): << loop");
    //Serial.flush();

    /* ----- Wait until next cycle ----- */
    
    timeEndMicros = micros();

    // Compute duration of loop
    timeDeltaMicros = timeEndMicros - timeStartMicros;

    // Update maximum slot duration within cycle
    if (timeDeltaMicros > timeDeltaMaxInCycleMicros)
    {
        timeDeltaMaxInCycleMicros = timeDeltaMicros;

        // Update overall maximum slot duration
        if (timeDeltaMicros > timeDeltaMaxMicros)
        {
            timeDeltaMaxMicros = timeDeltaMicros;
        }
    }

    if (timeDeltaMicros < slotTimeMicros) {
        delayMicroseconds(slotTimeMicros - timeDeltaMicros);
    }
    else {
        sprintf(strOut, "[W][M5StickC_GamepadApp.cpp] loop(): Duration of loop greater than cycle time: %d microseconds.", timeDeltaMicros);
        Serial.println(strOut);
    }
    
}