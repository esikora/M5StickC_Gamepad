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

        // Type that is used for x- and y-axis values.
        typedef int16_t StickAxis_t;

        /**
         * Returns singleton instance of the gamepad class.
         */
        static GamepadBLE* getInstance();

        /**
         * Initializes the necessary GATT services of the HID device and starts the BLE server.
         * 
         * @param pServer Pointer to the BLE server object.
         * 
         * @param deviceInfo Required device information for the Bluetooth HID device such as device name and PNP-Info.
         */ 
        void start(BLEServer* pServer, const tDeviceInfo &deviceInfo);

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
         * Does nothing if no host is connected.
         */
        void updateInputReport();

        /**
         * Sends the battery level to the connected host device via BLE.
         */
        void updateBatteryLevel(uint8_t level);

        // Delete copy constructor to prevent creation of additinal instances
        GamepadBLE(const GamepadBLE&) = delete;

        // Delete copy assignment operator to prevent creation of additinal instances
        GamepadBLE& operator = (const GamepadBLE&) = delete;

        // Delete move constructor to prevent creation of additinal instances
        GamepadBLE(GamepadBLE&&) = delete;

        // Delete move assignment operator to prevent creation of additinal instances
        GamepadBLE& operator = (GamepadBLE&&) = delete;

    private:

        /**
         * Creates a new GamepadBLE object. 
         */
        GamepadBLE();

        /**
         * Frees resources of GamepadBLE object on destruction.
         */
        ~GamepadBLE();

        /**
         * BLE HID device consisting of several GATT services and characteristics.
         */
        BLEHIDDevice* pHIDdevice_;

        /**
         * BLE GATT report characteristic of the gamepad.
         */
        BLECharacteristic* pInputCharacteristicId1_;

        /**
         * BLE GATT battery level characteristic of the gamepad.
         */
        BLECharacteristic* pBatteryLevelCharacteristic_;

        /**
         * Connection status. True, if gamepad is connected to a host.
         */
        bool connected_ = false;

        /**
         * HID report that is provided to the connected host (Characteristic UUID 0x2A4D).
         * It contains the current values of the gamepad controls, i.e. sticks and buttons.
         * 
         * The struct datatype complies with the format that is defined by the report map
         * (Characteristic UUID 0x2A4A).
         */
        tGamepadReportStruct gamepadData_;
        
        /**
         * Enum that defines the available methods for configuration and start of BLE advertisement.
         */
        enum tAdvLib { NONE = 0, ESP_BLE = 1, ESP_IDF = 2 };

        /**
         * Defines which method is used to configure and start BLE advertisement.
         */
        tAdvLib advLib_ = tAdvLib::NONE;

        /**
         * Defines advertisement data and scan response data using ESP BLE library structs and functions.
         */
        void setupAdvertisementDataBleLib();

        /**
         * Defines advertisement data and scan response data by providing raw data to the ESP BLE library functions.
         */
        void setupAdvertisementDataBleRaw(const std::string &deviceName);

        /**
         * BLE advertisement configuration parameters as required by the ESP-IDF BLE library.
         * 
         * See: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/esp_gap_ble.html#_CPPv420esp_ble_adv_params_t
         */
        esp_ble_adv_params_t advParamsIdf_;

        /**
         * Defines advertisement data and scan response data using ESP-IDF library structs and functions.
         */
        void setupAdvertisementDataEspIdf(const std::string &deviceName);

        /**
         * Starts advertising using the same library that has been used to configure the advertisement data.
         */
        void startAdvertising();

        /**
         * Semaphore used to synchronize on BLE GAP events of ESP-IDF during configuration of BLE advertisement.
         */
        SemaphoreHandle_t espBleGapEventSemaphore_ = nullptr;

        /**
         * GAP event handler used to synchronize on BLE GAP events of ESP-IDF during configuration of BLE advertisement.
         */
        static void gapEventHandler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);

        /**
         * Callback class that is registered at the BLE server to detect 'connect' and 'disconnect' events.
         */
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