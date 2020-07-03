#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <BLEHIDDevice.h>
#include <HIDTypes.h>

#include "HIDDescriptor.h"

/**
 * Representation of a Gamepad that is connectable via Bluetooth Low Energy (BLE).
 * The Gamepad device acts as a BLE server. It supports the HID over GATT protocol.
 * 
 * References:
 * - HID: https://www.usb.org/hid
 * - HID over GATT: https://www.bluetooth.org/docman/handlers/downloaddoc.ashx?doc_id=245141
 * 
 * Default mapping for sticks and buttons applied in this class:
 * https://docs.microsoft.com/en-us/windows/win32/xinput/directinput-and-xusb-devices
 * 
 */
class GamepadBLE {

    public:
        // Struct that is used for storing and transmitting the gamepad state.
        typedef StructGamepadInputGeneric2_t StructGamepadInput_t;

        // Type that is used for x- and y-axis values.
        typedef int16_t StickAxis_t;

        /**
         * Creates a new GamepadBLE object. 
         */
        GamepadBLE();

        /**
         * Initializes the necessary GATT services of the HID device and starts the BLE server.
         */ 
        void start(BLEServer* pServer);

        bool isConnected();

        void setButtonA(bool state);

        void setButtonB(bool state);
        
        void setButtonX(bool state);

        void setButtonY(bool state);

        void setButtonLB(bool state);

        void setButtonRB(bool state);

        void setButtonBack(bool state);

        void setButtonStart(bool state);

        void setLeftStick(StickAxis_t xPos, StickAxis_t yPos);

        void setLeftStickButton(bool state);

        void setRightStick(StickAxis_t xPos, StickAxis_t yPos);

        void setRightStickButton(bool state);

        /**
         * Sends the input report to the connected host device via BLE.
         */
        void updateInputReport();

        /**
         * Sends the battery level to the connected host device via BLE.
         */
        void updateBatteryLevel(uint8_t level);

    private:

        // BLE HID device consisting of several GATT services and characteristics
        BLEHIDDevice* pHIDdevice_;

        // BLE GATT input report characteristic of the gamepad.
        BLECharacteristic* pInputCharacteristicId1_;

        // BLE GATT battery level characteristic of the gamepad.
        BLECharacteristic* pBatteryLevelCharacteristic_;

        /**
         * Connection status. True, if connected to host. 
         */
        bool connected_;

        /**
         * Current values of main gamepad controls, i.e. sticks and buttons.
         */
        StructGamepadInput_t gamepadData_;

        class ConnectionEventCallback : public BLEServerCallbacks
        {
            public:
                ConnectionEventCallback(GamepadBLE* pGamepad);

                void onConnect(BLEServer* pServer);

                void onDisconnect(BLEServer* pServer);

            private:
                GamepadBLE* pGamepad_;
        };

};