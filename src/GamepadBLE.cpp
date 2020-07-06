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

void GamepadBLE::start(BLEServer* pServer, const tDeviceInfo &deviceInfo) {

    Serial.println("[V][GamepadBLE.cpp] start(): >> start");
    Serial.flush();

    // Debug output
    Serial.print("#Bytes in struct Generic2: ");
    Serial.println(sizeof(tGamepadReportStructGeneric2));
    Serial.print("#Bytes in struct gamepadData_: ");
    Serial.println(sizeof(gamepadData_));

    // Create HID device with required GATT services and characteristics
    pHIDdevice_ = new BLEHIDDevice(pServer);

    // Set the value of the "Manufacturer Name String" characteristic (UUID 0x2A29) of the "Device Information" service (UUID 0x180A)
    pHIDdevice_->manufacturer()->setValue(deviceInfo.manufacturerNameString);

    
    // Set the values of the "PnP ID" characteristic (UUID 0x2A50) of the "Device Information" service (UUID0x180A)
    pHIDdevice_->pnp(
        deviceInfo.vendorIdSource,
        deviceInfo.vendorId,
        deviceInfo.productId,
        deviceInfo.productVersion
    );

    // Set the values of the "HID Information" characteristic (UUID 0x2A4A) of the "Human Interface Device" service (UUID 0x1812)
    pHIDdevice_->hidInfo(
        deviceInfo.country,
        deviceInfo.flags
    );

    // Setup BLE security
    BLESecurity security;
    security.setAuthenticationMode(ESP_LE_AUTH_BOND);

    // Set the value of the "Report Map" characteristic (UUID 0x2A4B) of the "Human Interface Device" service (UUID 0x1812)
    pHIDdevice_->reportMap( (uint8_t*) pGamepadReportMap, kGamepadReportMapSize );

    // Message size check
    if ( sizeof(gamepadData_) != kGamepadReportSize )
    {
        char log[150];

        sprintf(log,
            "[E][GamepadBLE.cpp] start(): Wrong size of message struct. Check padding! Expected size is %d. Actual size of gamepadData_ is %d.",
            kGamepadReportSize,
            sizeof(gamepadData_) );

        Serial.println(log);
        Serial.flush();
    }

    // Create the characteristic for reporting the gamepad state (UUID 0x2A4D)
    pInputCharacteristicId1_ = pHIDdevice_->inputReport(1); // report ID 0x01

    // Enable server-initiated notifications for the report characteristic
    ((BLE2902*) pInputCharacteristicId1_->getDescriptorByUUID(BLEUUID((uint16_t) 0x2902)))->setNotifications(true);

    // Register callback object to listen for connect and disconnect events
    pServer->setCallbacks(new ConnectionEventCallback(this));

    // Initialize battery level, range 0..100
    //pHIDdevice_->setBatteryLevel(50);

    // Retrieve battery level characteristic and store for later use
    pBatteryLevelCharacteristic_ = pHIDdevice_->batteryService()->getCharacteristic( BLEUUID((uint16_t) 0x2a19) );

    // Enable server-initiated notifications for the "battery level" characteristic
    ((BLE2902*) pBatteryLevelCharacteristic_->getDescriptorByUUID(BLEUUID((uint16_t) 0x2902)))->setNotifications(true);

    // Start the service
    pHIDdevice_->startServices();

    /* ----- Setup the BLE advertising data for the HID gamepad device ----- */
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    
    pAdvertising->setAppearance(HID_GAMEPAD);
    pAdvertising->addServiceUUID(pHIDdevice_->hidService()->getUUID());

    pAdvertising->setScanResponse(true); // required to allow for device name with length > 23
    //pAdvertising->setMinPreferred(0x0); // required to allow for device name with length > 23
    
    // Start advertising
    pAdvertising->start();

    // Debug output
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
    if (gamepadData_.btn09) {
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