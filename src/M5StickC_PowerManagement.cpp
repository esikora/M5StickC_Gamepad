#include "M5StickC_PowerManagement.h"
#include <M5StickC.h>

const float M5StickC_PowerManagement::kCoulombMin = 0.0f; // mAh

const float M5StickC_PowerManagement::kCoulombMax = 5000.0f; // mAh

const float M5StickC_PowerManagement::kCoulombThresholdNegative = -0.5f; // mAh

const float M5StickC_PowerManagement::kBatteryVoltageLow = 3.2f; // V

const float M5StickC_PowerManagement::kBatteryVoltageHigh = 4.14f; // V

const float M5StickC_PowerManagement::kBatteryChargeCurrentLow = 20.0f;

const float M5StickC_PowerManagement::kCoulombLowVoltageMaxValue = 0.9f; // mAh

const uint8_t M5StickC_PowerManagement::kAxp192StorageDefault[] = { 0xF0, 0x0F, 0x00, 0xFF, 0x00, 0x00 };


M5StickC_PowerManagement::M5StickC_PowerManagement()
{

}

/**
 * Initializes the power management functions.
 * 
 * Needs to be called once, e.g. during setup, for the other functions of this class to work correctly.
 */
void M5StickC_PowerManagement::start()
{
    M5.Axp.EnableCoulombcounter();

    retrieveCoulombCounterMaxValue();

    // @TODO: Estimate coulomb value from battery voltage. Two cases: power connected, power not connected
}

/**
 * Reads all required data from the AXP192 unit of the M5StickC.
 *
 * Needs to be called periodically, e.g. every 1 s, for the other functions of this class to work correctly.
 */
void M5StickC_PowerManagement::readData()
{
    batVoltage_ = M5.Axp.GetBatVoltage();
    batPower_ = M5.Axp.GetBatPower();
    
    batCurrent_ = M5.Axp.GetBatCurrent();
    batChargeCurrent_ = M5.Axp.GetBatChargeCurrent();
    batDischargeCurrent_ = batCurrent_ - batChargeCurrent_;

    coulombData_ = M5.Axp.GetCoulombData();

    powerStatus_ = M5.Axp.GetInputPowerStatus();
    powerModeChargeStatus_ = M5.Axp.GetBatteryChargingStatus();

    // Debug output
    /*char strOut[150];

    sprintf(strOut, "VBat = %.3f V, PBat = %.3f mW, ICharge = %.3f mA, Coulomb = %.3f mAh, CoulombMax = %.3f mAh",
        batVoltage_, batPower_, batChargeCurrent_, coulombData_, coulombCounterMax_);

    Serial.println(strOut);*/
}

/**
 * Processes the data read from the AXP192 unit in order to determine the battery capacity.
 * 
 * Needs to be called periodically, e.g. every 1 s, for the other functions of this class to work correctly.
 */
void M5StickC_PowerManagement::computeBatteryCapacity()
{
    char strOut[150];

    // Is the device attached to a power source?
    if (isVBusPresent())
    {
        // Is the device still charging?
        if ( (batVoltage_ < kBatteryVoltageHigh) && (batChargeCurrent_ >= kBatteryChargeCurrentLow) )
        {
            // Has the coulomb counter reached a new maximum value during charging?
            if (coulombData_ > coulombCounterMax_)
            {
                coulombCounterMax_ = coulombData_;
                
                storeCoulombCounterMaxValue();

                // Debug output
                sprintf(strOut, "During charging, the coulomb counter reached a new max value: %f.3 mAh.", coulombCounterMax_);
                Serial.println(strOut);
            }
        }

        // Has the device completed charging?
        if ( (batVoltage_ >= kBatteryVoltageHigh) && (batChargeCurrent_ < kBatteryChargeCurrentLow) )
        {

            // Coulomb counter will not get any higher. Hence store it as new max value.
            if (coulombCounterMax_ > coulombData_)
            {
                coulombCounterMax_ = coulombData_;

                // Debug output
                sprintf(strOut, "Charging complete. Saving coulomb counter max value: %f.3 mAh.", coulombCounterMax_);
                Serial.println(strOut);

                storeCoulombCounterMaxValue();
            }
        } 
    }
    else { // Device is not attached to a power source

        // Is the battery voltage within normal range, i.e. not low?
        if (batVoltage_ >= kBatteryVoltageLow)
        {
            /* If coulomb counter falls below zero:
                - reset coulomb counter to zero
                - increase the positive maximum value of the coulomb counter accordingly
                Intention: Coulomb counter shall be a positive value
            */
            if (coulombData_ < kCoulombThresholdNegative)
            {
                M5.Axp.ClearCoulombcounter();
                
                coulombCounterMax_ = coulombCounterMax_ - coulombData_;
                
                storeCoulombCounterMaxValue();

                coulombData_ = 0;

                // Debug output
                sprintf(strOut, "Negative coulomb counter. Increased max value to %f.3.", coulombCounterMax_);
                Serial.println(strOut);
            }
        }
    }

    /* Adapt the maximum value of the coulomb counter, when voltage drops to
       critical level and counter has got a high value */

    // Is the device approaching power-off due to low voltage?
    if (batVoltage_ < kBatteryVoltageLow)
    {
        // At a low voltage the coulomb counter should be close to zero
        if (coulombData_ > kCoulombLowVoltageMaxValue)
        {
            // Set coulomb counter to zero
            M5.Axp.ClearCoulombcounter();

            // Ajust the max value of the coulomb counter accordingly
            coulombCounterMax_ = coulombCounterMax_ - coulombData_;

            // Debug output
            sprintf(strOut, "Coulomb counter too large at low voltage (%f.3). Decreased max value to %f.3.", coulombData_, coulombCounterMax_);
            Serial.println(strOut);

            storeCoulombCounterMaxValue();

            // Set coulomb data to zero.
            coulombData_ = 0;
        }
    }

}

/**
 * Calls readData and computeBatteryCapacity.
 */
void M5StickC_PowerManagement::readAndProcessData()
{
    readData();
    computeBatteryCapacity();
}

/**
 * Writes the maximum value of the coulomb counter into the storage register of the AXP192.
 */
void M5StickC_PowerManagement::storeCoulombCounterMaxValue()
{
    // Store the new max value
    writeFloatToAxpStorage(coulombCounterMax_, kAxpStorageKey);
}

/**
 * Retrieves the maximum value of the coulomb counter from the storage register of the AXP192.
 * When retrieving the value, the method checks whether the key is correct and the value is
 * inside a plausible range.
 * 
 * @return True, if the value could be retrieved successfully.
 */
bool M5StickC_PowerManagement::retrieveCoulombCounterMaxValue()
{
    char strOut[150];
    uint8_t axpStorage[6];

    M5.Axp.Read6BytesStorage(axpStorage); // Default is: F0/0F/00/FF/00/00H

    float f;

    // Interpret first 4 bytes from Axp storage as float value
    for (int byteNum = 0; byteNum < sizeof(f); ++byteNum)
    {
        ((uint8_t*) &f)[byteNum] = axpStorage[byteNum];
    }

    // Debug output
    Serial.print("Raw data from AXP192 Storage: ");

    for (int i = 0; i < sizeof(axpStorage); ++i)
    {
        sprintf(strOut, "%02X ", axpStorage[i]);
        Serial.print(strOut);
    }

    sprintf(strOut, "-> Float value: %.4f", f);
    Serial.println(strOut);

    bool valid = false;

    // Check whether last two bytes from AXP storage match the expected storage key
    if ( (axpStorage[4] == ((uint8_t*) &kAxpStorageKey)[0]) && (axpStorage[5] == ((uint8_t*) &kAxpStorageKey)[1]) )
    {
        // Check if stored value is in plausible range
        if (f > 0 && f <= 5000)
        {
            // Retrieved value seems to be a valid coulomb counter max value
            coulombCounterMax_ = f;

            valid = true;

            // Debug output
            sprintf(strOut, "Coulomb counter max value successfully restored to %.4f", coulombCounterMax_);
            Serial.println(strOut);
        }
        else {
            sprintf(strOut, "Warning: value read from AXP storage is out of plausible range!");
            Serial.println(strOut);
        }
    }
    else {
        sprintf(strOut, "Warning: storage key read from AXP does not match expected key!");
        Serial.println(strOut);
    }

    // @TODO: Compare max counter with value derived from battery voltage and adjust it if necessary

    return valid;
}

/**
 * Restores the AXP192 storage register to its default values.
 */ 
void M5StickC_PowerManagement::restoreDefaultAxpStorage()
{
    uint8_t axpStorage[6];

    // Copy default values into temporary array
    for (int byteNum = 0; byteNum < sizeof(kAxp192StorageDefault); ++byteNum)
    {
        axpStorage[byteNum] = kAxp192StorageDefault[byteNum];
    }
    
    M5.Axp.Write6BytesStorage(axpStorage);
}

/**
 * Prints the current battery and power management status into a string.
 */
void M5StickC_PowerManagement::printStatusToString(char *strOut)
{
    sprintf(strOut, "vBat = %.3f V, pBat = %.3f mW, iBat = %.3f mA, iChrg = %.3f mA, iDischrg = %.3f mAh, clmb = %.3f mAh, clmbMax = %.3f mAh, vBusPres = %d",
    batVoltage_, batPower_, batCurrent_, batChargeCurrent_, batDischargeCurrent_, coulombData_, coulombCounterMax_, isVBusPresent());
}

/**
 * Writes a float value and a key into the 6 byte AXP192 storage register.
 */ 
void M5StickC_PowerManagement::writeFloatToAxpStorage(float f, uint16_t key)
{
    uint8_t axpStorage[6];

    // Overwrite the first 4 bytes with data from the float
    for (int byteNum = 0; byteNum <= 3; ++byteNum)
    {
        axpStorage[byteNum] = ((uint8_t*) &f)[byteNum];
    }

    // Overwrite the last two bytes with the key
    for (int byteNum = 4; byteNum <= 5; ++byteNum)
    {
        axpStorage[byteNum] = ((uint8_t*) &key)[byteNum - 4];
    }

    // Write new values
    M5.Axp.Write6BytesStorage(axpStorage);
}