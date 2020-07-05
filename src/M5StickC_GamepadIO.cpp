#include "M5StickC_GamepadIO.h"

#include <M5StickC.h>

const uint8_t M5StickC_GamepadIO::kBtnPin[2] = {kPinButtonBlue, kPinButtonRed};


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

        /* Register an interrupt routine for each button
           Intention: Detect very short button activations between two calls of the "process" function
           The registered interrupt routines set a flag each time a button is pressed
        */
        /*attachInterruptArg(digitalPinToInterrupt(kPinButtonBlue), isrBtnBlue, this, CHANGE);
        attachInterruptArg(digitalPinToInterrupt(kPinButtonRed),  isrBtnRed,  this, CHANGE);*/

        // Create task with maximum priority for reading the button states
        xTaskCreate(M5StickC_GamepadIO::buttonTask, "Gamepad button task", 4096, this, configMAX_PRIORITIES - 1, NULL);
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
    //int btnBluePinValInv = !digitalRead(kPinButtonBlue);
    //int btnRedPinValInv  = !digitalRead(kPinButtonRed);

    // To detect button presses between two calls of "digitalRead", check the flags activated by interrupt routines
    //btnBlueState_ = btnBluePinValInv || btnBlueFlag_;
    //btnRedState_  = btnRedPinValInv  || btnRedFlag_;

    // Reset the flags
    //btnBlueFlag_ = 0;
    //btnRedFlag_ = 0;
}

M5StickC_GamepadIO::~M5StickC_GamepadIO()
{
    /*detachInterrupt(kPinButtonBlue);
    detachInterrupt(kPinButtonRed );*/
}