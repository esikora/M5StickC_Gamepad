#include <Arduino.h>
#include "GamepadBLE.h"

GamepadBLE::GamepadBLE()
: pHIDdevice_{nullptr}
, connected_{false}
, gamepadData_{}
{
}

void GamepadBLE::setButtonA(bool state) {
    gamepadData_.btn01 = state; // Default mapping
}

void GamepadBLE::setButtonB(bool state) {
    gamepadData_.btn02 = state; // Default mapping
}

void GamepadBLE::setButtonX(bool state) {
    gamepadData_.btn03 = state; // Default mapping
}

void GamepadBLE::setButtonY(bool state) {
    gamepadData_.btn04 = state; // Default mapping
}

void GamepadBLE::setButtonLB(bool state) {
    gamepadData_.btn05 = state; // Default mapping
}

void GamepadBLE::setButtonRB(bool state) {
    gamepadData_.btn06 = state; // Default mapping
}

void GamepadBLE::setButtonBack(bool state) {
    gamepadData_.btn07 = state; // Default mapping
}

void GamepadBLE::setButtonStart(bool state) {
    gamepadData_.btn08 = state; // Default mapping
}

void GamepadBLE::setLeftStickButton(bool state) {
    gamepadData_.btn09 = state; // Default mapping
}

void GamepadBLE::setRightStickButton(bool state) {
    gamepadData_.btn10 = state; // Default mapping
}

void GamepadBLE::setLeftStick(StickAxis_t xPos, StickAxis_t yPos) {
    gamepadData_.stickLX = xPos;
    gamepadData_.stickLY = yPos;
}

void GamepadBLE::setRightStick(StickAxis_t xPos, StickAxis_t yPos) {
    gamepadData_.stickRX = xPos;
    gamepadData_.stickRY = yPos;
}

void GamepadBLE::start(BLEServer* pServer) {

    Serial.println("[V][GamepadBLE.cpp] start(): >> start");
    Serial.flush();

    // Debug output
    Serial.print("#Bytes in struct Generic2: ");
    Serial.println(sizeof(StructGamepadInputGeneric2_t));
    Serial.print("#Bytes in struct gamepadData_: ");
    Serial.println(sizeof(gamepadData_));

    // Create HID device with required GATT services and characteristics
    pHIDdevice_ = new BLEHIDDevice(pServer);

    std::string manufacturerStr = "DIY";
    pHIDdevice_->manufacturer()->setValue(manufacturerStr);

    /* GATT Characteristic 'PNP ID':

       Source: https://www.partsnotincluded.com/understanding-the-xbox-360-wired-controllers-usb-data/

       1. Vendor ID Source: 0x02 = 'USB Implementer’s Forum assigned Vendor ID value'
       2. Vendor ID: 0x045E = 'Microsoft Corporation'
       3. Product ID: 0x02FD = 'XBOX ONE S Controller [Bluetooth]'
       4. Product Version = '1.1.4' (TBD)
    */
    //pHIDdevice_->pnp(0x02, 0x045E, 0x02FD, 0x0114);
    pHIDdevice_->pnp(0x01, 0x02e5, 0xabcd, 0x0110); // Source: https://github.com/lemmingDev/ESP32-BLE-Gamepad/blob/master/BleGamepad.cpp

    /* GATT Characteristic 'HID information':
         1. Country: 0x00 = not localized 
         2. Flags (bitfield): 0x03 = 'bit0: capable of providing wake-up signal to a HID host' | 'bit1: normally connectable'
    */    
    //pHIDdevice_->hidInfo(0x00, 0x02);
    pHIDdevice_->hidInfo(0x00, 0x01); // Source: https://github.com/lemmingDev/ESP32-BLE-Gamepad/blob/master/BleGamepad.cpp

    // Setup BLE security
    BLESecurity security;
    security.setAuthenticationMode(ESP_LE_AUTH_BOND);

    // Set the gamepad HID report descriptor
    //pHIDdevice_->reportMap( (uint8_t*) kReportDescriptorXBoxOneS, sizeof(kReportDescriptorXBoxOneS));
    pHIDdevice_->reportMap( (uint8_t*) kReportDescriptorGeneric2, sizeof(kReportDescriptorGeneric2));

    // Message size check
    if ( sizeof(gamepadData_) != kMsgSizeReportGeneric2 )
    {
        char log[150];

        sprintf(log,
            "[E][GamepadBLE.cpp] start(): Wrong size of message struct. Check padding! Expected size is %d. Actual size of gamepadData_ is %d.",
            kMsgSizeReportGeneric2,
            sizeof(gamepadData_) );

        Serial.println(log);
        Serial.flush();
    }

    // Create the input report characteristic for the gamepad state
    pInputCharacteristicId1_ = pHIDdevice_->inputReport(1);

    // Enable server-initiated notifications for the "input report" characteristic
    ((BLE2902*) pInputCharacteristicId1_->getDescriptorByUUID(BLEUUID((uint16_t) 0x2902)))->setNotifications(true);

    // Register callback object to listen for connect and disconnect events
    pServer->setCallbacks(new ConnectionEventCallback(this));

    // Initialize battery level, range 0..100
    pHIDdevice_->setBatteryLevel(50);

    // Retrieve battery level characteristic and store for later use
    pBatteryLevelCharacteristic_ = pHIDdevice_->batteryService()->getCharacteristic( BLEUUID((uint16_t) 0x2a19) );

    // Enable server-initiated notifications for the "battery level" characteristic
    ((BLE2902*) pBatteryLevelCharacteristic_->getDescriptorByUUID(BLEUUID((uint16_t) 0x2902)))->setNotifications(true);

    // Start the service
    pHIDdevice_->startServices();

    // Advertise the HID gamepad service
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    //pAdvertising->setScanResponse(false); // required to allow for device name with length > 23
    //pAdvertising->setMinPreferred(0x0); // required to allow for device name with length > 23
    pAdvertising->setAppearance(HID_GAMEPAD);
    pAdvertising->addServiceUUID(pHIDdevice_->hidService()->getUUID());
    pAdvertising->start();

    UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
    Serial.print("[V][GamepadBLE.cpp] start(): StackHighWaterMark = ");
    Serial.println(uxHighWaterMark);

    Serial.println("[V][GamepadBLE.cpp] start(): << start");
    Serial.flush();
};

void GamepadBLE::updateInputReport() {

    //Serial.println("[V][GamepadBLE.cpp] updateInputReport(): >> updateInputReport");
    //Serial.flush();

    if (connected_)
    {
        pInputCharacteristicId1_->setValue( (uint8_t*) &gamepadData_, sizeof(gamepadData_));
        pInputCharacteristicId1_->notify();        
    }

    // Debug output
    if (gamepadData_.btn01) {
        uint8_t* pData = (uint8_t*) &gamepadData_;

        for (int i = 0; i < sizeof(gamepadData_); i++) {
            Serial.print(pData[i]);
            Serial.print(' ');
        }

        Serial.println();
        Serial.flush();
    }

    //Serial.println("[V][GamepadBLE.cpp] updateInputReport(): << updateInputReport");
    //Serial.flush();
}

void GamepadBLE::updateBatteryLevel(uint8_t level) {
    //pHIDdevice_->setBatteryLevel(level);

    if (connected_)
    {
        pBatteryLevelCharacteristic_->setValue(&level, 1);
        pBatteryLevelCharacteristic_->notify();
    }
}

bool GamepadBLE::isConnected()
{
    return connected_;
}


GamepadBLE::ConnectionEventCallback::ConnectionEventCallback(GamepadBLE* pGamepad)
{
    pGamepad_ = pGamepad;
}

void GamepadBLE::ConnectionEventCallback::onConnect(BLEServer* pServer)
{
    // Debug output
    Serial.println("onConnect");
    Serial.flush();

    // Enable server-initiated notifications
    // ((BLE2902*) pGamepad_->pInputCharacteristicId1_->getDescriptorByUUID(BLEUUID((uint16_t) 0x2902)))->setNotifications(true);

    pGamepad_->connected_ = true;
}

void GamepadBLE::ConnectionEventCallback::onDisconnect(BLEServer* pServer)
{
    Serial.println("onDisconnect");

    pGamepad_->connected_ = false;

    // Disable server-initiated notifications
    // ((BLE2902*) pGamepad_->pInputCharacteristicId1_->getDescriptorByUUID(BLEUUID((uint16_t) 0x2902)))->setNotifications(false);

    // Restart advertising after host closed the connection
    pServer->startAdvertising();
}