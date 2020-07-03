#include "M5StickC_GamepadIO.h"

#include <M5StickC.h>

M5StickC_GamepadIO* M5StickC_GamepadIO::getInstance()
{
    static M5StickC_GamepadIO instance{};

    return &instance;
}


M5StickC_GamepadIO::M5StickC_GamepadIO()
{

}

void M5StickC_GamepadIO::start()
{
    if (!initialized_)
    {
        initialized_ = true;

        // Setup I2C communiation for JoyC (grove port)
        Wire.begin(kI2CpinSda, kI2CpinScl, kI2Cfreq);

        // Configure GPIO pins of dual button unit
        pinMode(kPinButtonBlue, INPUT_PULLUP);
        pinMode(kPinButtonRed, INPUT_PULLUP);

        // Register an interrupt routine for each button
        attachInterruptArg(digitalPinToInterrupt(kPinButtonBlue), isrBtnBlue, this, FALLING);
        attachInterruptArg(digitalPinToInterrupt(kPinButtonRed),  isrBtnRed,  this, FALLING);
    }
}

void M5StickC_GamepadIO::process()
{
    // Read joystick raw data via I2C
    if ( Wire.requestFrom(kI2CjoystickUnitAddr, kI2CjoystickUnitNumBytes) ) {
        joyRawX_ = Wire.read();
        joyRawY_ = Wire.read();
        joyPressed_ = Wire.read();
    }
    else
    {
        Serial.println("M5StickC_GamepadIO::process: Error reading joystick data via I2C.");

        // Note: If reading is unsuccessful, the variables keep their previous values
    }
    
    // Compute normalized stick positions
    joyNormX_ = joyFactorX_ * (joyRawX_ - joyRawCenterX_);
    joyNormY_ = joyFactorY_ * (joyRawY_ - joyRawCenterY_);

    // Read current button states via GPIO
    int btnBluePinValInv = !digitalRead(kPinButtonBlue);
    int btnRedPinValInv  = !digitalRead(kPinButtonRed);

    // To detect button presses between two calls of "digitalRead", check the flags activated by interrupt routines
    btnBluePressed_ = btnBluePinValInv || btnBluePressedIsr_;
    btnRedPressed_  = btnRedPinValInv  || btnRedPressedIsr_;

    // Reset the flags
    btnBluePressedIsr_ = 0;
    btnRedPressedIsr_ = 0;
}

M5StickC_GamepadIO::~M5StickC_GamepadIO()
{
    detachInterrupt(kPinButtonBlue);
    detachInterrupt(kPinButtonRed );
}