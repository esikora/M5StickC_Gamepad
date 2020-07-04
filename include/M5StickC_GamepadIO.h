#include <Arduino.h>
#include <M5StickC.h>

class M5StickC_GamepadIO
{
    public:
        // GPIO pin of blue button of the dual button unit (depends on wiring)
        static const uint8_t kPinButtonRed = 26;

        // GPIO pin of red button of the dual button unit (depends on wiring)
        static const uint8_t kPinButtonBlue = 36;

        // IC2 configuration for joystick unit attached to M5StickC grove port
        static const int kI2CpinSda = 32;
        static const int kI2CpinScl = 33;
        static const uint32_t kI2Cfreq = 400000;

        // I2C address of the joystick unit
        static const uint8_t kI2CjoystickUnitAddr = 0x52;

        static const uint8_t kI2CjoystickUnitNumBytes = 0x03;

        static M5StickC_GamepadIO* getInstance();

        void start();

        void process();

        /**
         * Returns the current state of the blue button.
         */
        inline uint8_t isBtnBluePressed()
        {
            return btnBlueState_;
        }

        /**
         * Returns the flag value of the blue button.
         * The flag is 1, if the blue button has been pressed since the last call of this function.
         * The flag reset to 0 on each call of this function.
         */
        inline uint8_t wasBtnBluePressed()
        {
            // Save flag value
            uint8_t result = btnBlueFlag_;

            // Reset flag
            btnBlueFlag_ = 0;

            return result;
        }

        /**
         * Returns 1 if the blue button is pressed or has been pressed since the last call.
         */
        inline uint8_t getBtnBlueActivation()
        {
            return isBtnBluePressed() || wasBtnBluePressed();
        }

        inline uint8_t isBtnRedPressed()
        {
            return btnRedState_;
        }

        inline uint8_t wasBtnRedPressed()
        {
            // Save flag value
            uint8_t result = btnRedState_;

            // Reset flag
            btnRedState_ = 0;

            return result;
        }

        inline uint8_t getBtnRedActivation()
        {
            return isBtnRedPressed() || wasBtnRedPressed();
        }

        inline int8_t getJoyNormX()
        {
            return joyNormX_;
        }

        inline int8_t getJoyNormY()
        {
            return joyNormY_;
        }

        inline uint8_t isJoyPressed()
        {
            return joyPressed_;
        }

        M5StickC_GamepadIO(const M5StickC_GamepadIO&) = delete;

        M5StickC_GamepadIO& operator = (const M5StickC_GamepadIO&) = delete;

        M5StickC_GamepadIO(M5StickC_GamepadIO&&) = delete;

        M5StickC_GamepadIO& operator = (M5StickC_GamepadIO&&) = delete;

    private:

        // Private constructor to prevent creation of multiple instances
        M5StickC_GamepadIO();

        // Destructor
        ~M5StickC_GamepadIO();

        // Initialization flag
        bool initialized_ = false;

        // Flag that blue button has been pressed, set by interrupt service routine
        volatile uint8_t btnBlueFlag_ = 0;

        // Flag that red button has been pressed, set by interrupt service routine
        volatile uint8_t btnRedFlag_ = 0;

        // State of blue button
        uint8_t btnBlueState_ = 0;

        // State of red button
        uint8_t btnRedState_ = 0;


        // Joystick center value for x-axis. The joystick unit provides values in a range of approx. 0..245
        uint8_t joyRawCenterX_ = 122; // @TODO determine dynamically, e.g. no user action for x seconds and value within specified range

        // Joystick center value for y-axis. The joystick unit provides values in a range of approx. 0..245
        uint8_t joyRawCenterY_ = 122; // @TODO determine dynamically, e.g. no user action for x seconds and value within specified range

        // Current x-position value of the joystick
        uint8_t joyRawX_ = 0;

        // Current y-position value of the joystick
        uint8_t joyRawY_ = 0;

        // Current button press state of the joystick
        uint8_t joyPressed_ = 0;

        // Factor applied to joystick x-position. It is meant to be used for inversion.
        uint8_t joyFactorX_ = 1;

        // Factor applied to joystick y-position. It is meant to be used for inversion.
        uint8_t joyFactorY_ = -1;

        // Normalized joystick x-position, i.e. centered around zero and multiplied by factor
        int8_t joyNormX_ = 0;
        
        // Normalized joystick y-position, i.e. centered around zero and multiplied by factor
        int8_t joyNormY_ = 0;

        /**
         * Function is activated by an interrupt that is attached to GPIO pin of blue button
         */
        inline static void IRAM_ATTR isrBtnBlue(void *p)
        {
            // Read current button state
            uint8_t state = !digitalRead(kPinButtonBlue);

            // Store button state
            ((M5StickC_GamepadIO*) p)->btnBlueState_ = state;

            // Set flag to indicate that the butten has been pressed
            if (state)
                ((M5StickC_GamepadIO*) p)->btnBlueFlag_ = 1;
        }

        /**
         * Function is activated by an interrupt that is attached to GPIO pin of red button
         */
        inline static void IRAM_ATTR isrBtnRed(void *p)
        {
            // Read current button state
            uint8_t state = !digitalRead(kPinButtonRed);

            // Store button state
            ((M5StickC_GamepadIO*) p)->btnRedState_ = state;

            // Set flag to indicate that the butten has been pressed
            if (state)
                ((M5StickC_GamepadIO*) p)->btnRedFlag_ = 1;
        }
};