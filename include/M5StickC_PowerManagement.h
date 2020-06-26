#include <Arduino.h>

/**
 * Helper class to use the functions of the M5StickC AXP192 power management unit.
 * 
 * Content of the AXP192 power status register (0x00), see:
 * https://github.com/ContextQuickie/TTGO-T-Beam/wiki/Register-00H:-Power-Status-Register
 * 
 * Content of the AXP192 power mode / charge status register (0x01), see:
 * https://github.com/ContextQuickie/TTGO-T-Beam/wiki/Register-01H:-Power-Mode---Charge-Status-Register
 * 
 */
class M5StickC_PowerManagement {

    public:

        static const uint16_t kAxpStorageKey = 0x1a7c;

        static const float kCoulombMin; // mAh

        static const float kCoulombMax; // mAh

        static const float kCoulombThresholdNegative; // mAh

        static const float kCoulombLowVoltage; // V

        static const float kCoulombLowVoltageMaxValue; // mAh

        static const uint8_t kAxp192StorageDefault[];

        

        M5StickC_PowerManagement();

        /**
         * Initializes the power management functions.
         * 
         * Needs to be called once, e.g. during setup, for the other functions of this class to work correctly.
         */
        void start();

        /**
         * Reads all required data from the AXP192 unit of the M5StickC.
         *
         * Needs to be called periodically, e.g. every 1 s, for the other functions of this class to work correctly.
         */
        void readData();

        /**
         * Processes the data read from the AXP192 unit in order to determine the battery level.
         * 
         * Needs to be called periodically, e.g. every 1 s, for the other functions of this class to work correctly.
         */
        void computeBatteryLevel();

        /**
         * Calls readData and computeBatteryLevel.
         */
        void readAndProcessData();

        /**
         * Returns the battery voltage.
         * The provided value is the one that has been determined by the last call of readAll().
         * 
         * Remark: When an external power source is connected, the voltage is typically higher.
         * 
         * @return Battery voltage [V].
         */
        inline float getBatVoltage()
        {
            return batVoltage_;
        }

        /**
         * Returns the power that is drawn from the battery.
         * 
         * Remark: When an external power source is connected, typically the power is zero.
         * 
         * @return Battery power [mW].
         */
        inline float getBatPower()
        {
            return batPower_;
        }

        /**
         * Returns the battery charge current.
         * 
         * @return Battery charge current [mA].
         */
        inline float getChargeCurrent()
        {
            return batChargeCurrent_;
        }

        /**
         * Returns the value of coulomb counter.
         * 
         * @return Coulomb data [mAh].
         */
        inline float getCoulombData()
        {
            return coulombData_;
        }

        /**
         * Returns the start source.
         * 
         * @return 0 = VBUS, 1 = ACIN.
         */
        inline uint8_t getStartSource()
        {
            return (powerStatus_ & 0b00000001) != 0;
        }

        /**
         * Returns the current directions
         * 
         * @return 0 = discharging, 1 = charging.
         */
        inline uint8_t getCurrentDirection()
        {
            return (powerStatus_ & 0b00000100) != 0;
        }

        /**
         * Returns the "VBus available" indication.
         * 
         * @return false = VBUS not available, true = VBUS available.
         */
        inline bool isVBusAvailable()
        {
            return (powerStatus_ & 0b00010000) != 0;
        }

        /**
         * Returns the VBus Present indication.
         * 
         * @return false = VBUS not present, true = VBUS present.
         */
        inline bool isVBusPresent()
        {
            return (powerStatus_ & 0b00100000) != 0;
        }

        /**
         * Returns the "ACIn available" indication.
         * 
         * @return false = ACIN not available, true = ACIN available.
         */
        inline bool isACInAvailable()
        {
            return (powerStatus_ & 0b01000000) != 0;
        }

        /**
         * Returns the "ACIn present" indication.
         * 
         * @return false = ACIN not present, true = ACIN present.
         */
        inline bool isACInPresent()
        {
            return (powerStatus_ & 0b10000000) != 0;
        }

        /**
         * Returns the battery presence indication.
         * 
         * @return false = No battery connected to AXP192, true = Battery connected to AXP192.
         */
        inline bool isBatteryPresent()
        {
            return (powerModeChargeStatus_ & 0b00100000) != 0;
        }

        /**
         * Sets the Charge Indication power mode.
         * 
         * @return false = Not charging or charging completed, true = Charging.
         */
        inline bool isCharging()
        {
            return (powerModeChargeStatus_ & 0b01000000) != 0;
        }

        /**
         * Provides the maximum value of the coulomb counter that has been determined by this class.
         * 
         * @return Maximum value of the coulomb counter [mAh].
         */
        inline float getCoulombCounterMaxValue()
        {
            return coulombCounterMax_;
        }

        /**
         * Provides the current battery status as a percent value.
         */
        inline float getBatteryLevelPercent()
        {
            float batPerc = coulombData_ / coulombCounterMax_ * 100;

            return batPerc;
        }

        /**
         * Writes the maximum value of the coulomb counter into the storage register of the AXP192.
         */
        void storeCoulombCounterMaxValue();

        /**
         * Retrieves the maximum value of the coulomb counter from the storage register of the AXP192.
         * When retrieving the value, the method checks whether the key is correct and the value is
         * inside a plausible range.
         * 
         * @return True, if the value could be retrieved successfully.
         */
        bool retrieveCoulombCounterMaxValue();

        /**
         * Restores the AXP192 storage register to its default values.
         */ 
        void restoreDefaultAxpStorage();

        /**
         * Prints the current battery and power management status into a string.
         */
        void printStatusToString(char *strOut);


    private:

        // Maximum value of coulomb counter [mAh]
        float   coulombCounterMax_ = 0;

        float 	batVoltage_;
        float 	batPower_;
        float 	batChargeCurrent_;
        float 	coulombData_;

        uint8_t powerStatus_;
        uint8_t powerModeChargeStatus_;

        /**
         * Writes a float value and a key into the 6 byte AXP192 storage register.
         */ 
        void writeFloatToAxpStorage(float f, uint16_t key);
};