#include <Arduino.h>
#include "GamepadBLE.h"

GamepadBLE* GamepadBLE::getInstance()
{
    static GamepadBLE instance{};

    return &instance;
}

GamepadBLE::GamepadBLE()
: pHIDdevice_{nullptr}
, gamepadData_{}
{
}

GamepadBLE::~GamepadBLE()
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

    log_v(">>");

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
        log_e(
            "Wrong size of message struct. Check padding! Expected size is %d. Actual size of gamepadData_ is %d.",
            kGamepadReportSize,
            sizeof(gamepadData_)
        );
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

    log_d("Device name: %s", deviceInfo.deviceName.c_str());

    // Setup the BLE advertisement data for the HID gamepad device
    //setupAdvertisementDataEspIdf(deviceInfo.deviceName);
    setupAdvertisementDataBleLib();

    // Start advertising using the previously defined configuration data
    startAdvertising();

    log_v("<<");
};

void GamepadBLE::setupAdvertisementDataBleLib()
{
    // Store information that BLE library is used for advertisement
    advLib_ = tAdvLib::ESP_BLE;

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();

    /*** Define advertisement data and scan response data using BLE library functions ***/
    pAdvertising->setAppearance(HID_GAMEPAD);
    //pAdvertising->addServiceUUID(pHIDdevice_->hidService()->getUUID());
    //pAdvertising->setMinPreferred(0x0);
    //pAdvertising->setMaxPreferred(0x0);
}

void GamepadBLE::setupAdvertisementDataBleRaw(const std::string &deviceName)
{
    // Store information that BLE library is used for advertisement
    advLib_ = tAdvLib::ESP_BLE;

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();

    /*** Define advertisement data as raw string ***/    
    BLEAdvertisementData advData;
    advData.setFlags(ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);
    advData.setAppearance(ESP_BLE_APPEARANCE_HID_GAMEPAD);
    advData.setCompleteServices(pHIDdevice_->hidService()->getUUID());

    pAdvertising->setAdvertisementData(advData);

    //esp_ble_gap_config_adv_data_raw( (uint8_t*) advData.getPayload().data(), advData.getPayload().length() );

    /*** Define scan response data as raw string ***/    
    BLEAdvertisementData scanRespData;
    scanRespData.setName(deviceName);
    pAdvertising->setScanResponseData(scanRespData);
}

void GamepadBLE::setupAdvertisementDataEspIdf(const std::string &deviceName)
{
    // Store the information that ESP-IDF library is used for advertisement
    advLib_ = tAdvLib::ESP_IDF;

    // Create the semaphore that will be used for synchronization on BLE GAP events
    espBleGapEventSemaphore_ = xSemaphoreCreateBinary();

    // Register the custom Gap event handler of the gamepad class
    BLEDevice::setCustomGapHandler(gapEventHandler);

    /*** Define advertisement data using ESP-IDF library struct ***/
    esp_ble_adv_data_t   advDataIDF;

    advDataIDF.set_scan_rsp        = false;
	advDataIDF.include_name        = false;
	advDataIDF.include_txpower     = true;
	advDataIDF.min_interval        = 0x20;
	advDataIDF.max_interval        = 0x40;
	advDataIDF.appearance          = ESP_BLE_APPEARANCE_HID_GAMEPAD;
	advDataIDF.manufacturer_len    = 0;
	advDataIDF.p_manufacturer_data = nullptr;
	advDataIDF.service_data_len    = 0;
	advDataIDF.p_service_data      = nullptr;
	advDataIDF.service_uuid_len    = 0;
	advDataIDF.p_service_uuid      = nullptr;
	advDataIDF.flag                = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);

    // Pass on config data to ESP-IDF
    esp_ble_gap_config_adv_data(&advDataIDF);

    // Synchronize on the event 'ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT'
    xSemaphoreTake(espBleGapEventSemaphore_, portMAX_DELAY);

    /*** Define scan response data using ESP-IDF library struct ***/
    esp_ble_adv_data_t   scanRespDataIDF;

    scanRespDataIDF.set_scan_rsp        = true;
	scanRespDataIDF.include_name        = true;
	scanRespDataIDF.include_txpower     = false;
	scanRespDataIDF.min_interval        = 0;
	scanRespDataIDF.max_interval        = 0;
	scanRespDataIDF.appearance          = 0;
	scanRespDataIDF.manufacturer_len    = 0;
	scanRespDataIDF.p_manufacturer_data = nullptr;
	scanRespDataIDF.service_data_len    = 0;
	scanRespDataIDF.p_service_data      = nullptr;
	scanRespDataIDF.service_uuid_len    = 0;
	scanRespDataIDF.p_service_uuid      = nullptr;
	scanRespDataIDF.flag                = 0;

    // Pass on config data to ESP-IDF
    esp_ble_gap_config_adv_data(&scanRespDataIDF);

    // Synchronize on the event 'ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT'
    xSemaphoreTake(espBleGapEventSemaphore_, portMAX_DELAY);

    // Unregister the custom Gap event handler
    BLEDevice::setCustomGapHandler(nullptr);

    /*** Set advertisement configuration parameters using ESP-IDF library struct ***/    
	advParamsIdf_.adv_int_min       = 0x20;
	advParamsIdf_.adv_int_max       = 0x40;
	advParamsIdf_.adv_type          = ADV_TYPE_IND;
	advParamsIdf_.own_addr_type     = BLE_ADDR_TYPE_PUBLIC;
	advParamsIdf_.channel_map       = ADV_CHNL_ALL;
	advParamsIdf_.adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;
	advParamsIdf_.peer_addr_type    = BLE_ADDR_TYPE_PUBLIC;
}

void GamepadBLE::startAdvertising()
{
    // @Todo: Define what should happen if the gamepad is connected --> e.g. disconnect

    switch (advLib_)
    {
        case tAdvLib::ESP_BLE:
        {
            BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();

            /*** Start advertising using the previously defined data ***/
            pAdvertising->start();

            break;
        }

        case tAdvLib::ESP_IDF:
            /*** Start advertising using ESP-IDF library function ***/
            esp_ble_gap_start_advertising(&advParamsIdf_);

            // Synchronize on the event 'ESP_GAP_BLE_ADV_START_COMPLETE_EVT' (possibly unnecessary)
            // Presently, 'gapEventHandler' is only used for synchronisation in 'setupAdvertisementDataEspIdf'
            /*
            if (espBleGapEventSemaphore_ != nullptr)
            {
                xSemaphoreTake(espBleGapEventSemaphore_, portMAX_DELAY);
            }
            */

            break;

        default:
            log_e("Unexpected value of advLib_: %d", advLib_);
            break;

    }
}

void GamepadBLE::updateInputReport() {

    log_v(">>");

    if (connected_)
    {
        pInputCharacteristicId1_->setValue( (uint8_t*) &gamepadData_, sizeof(gamepadData_));
        pInputCharacteristicId1_->notify();        
    }

    // Debug output
    if (gamepadData_.btn09) {
        uint8_t* pData = (uint8_t*) &gamepadData_;

        // Convert report to hex string
        std::string hexStr = "";
        char hexByteStr[4];

        for (int i = 0; i < sizeof(gamepadData_); i++)
        {
            sprintf(hexByteStr, "%02x ", pData[i]);
            hexStr.append(hexByteStr);
        }

        log_d("Report data hex: %s[%d bytes]", hexStr.c_str(), sizeof(gamepadData_));
    }

    log_v("<<");
}

void GamepadBLE::updateBatteryLevel(uint8_t level) {
    /* Not using the following function because, in addition, notify is needed.
       Without notification, the connected host will not get updates of the battery level.
       Instead the characteristic is accessed directly. */
    // pHIDdevice_->setBatteryLevel(level);

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
    log_v(">>");

    // Enable server-initiated notifications
    // ((BLE2902*) pGamepad_->pInputCharacteristicId1_->getDescriptorByUUID(BLEUUID((uint16_t) 0x2902)))->setNotifications(true);

    pGamepad_->connected_ = true;

    log_v("<<");
}

void GamepadBLE::ConnectionEventCallback::onDisconnect(BLEServer* pServer)
{
    log_v(">>");

    pGamepad_->connected_ = false;

    // Disable server-initiated notifications
    // ((BLE2902*) pGamepad_->pInputCharacteristicId1_->getDescriptorByUUID(BLEUUID((uint16_t) 0x2902)))->setNotifications(false);

    // Restart advertising after host closed the connection
    pGamepad_->startAdvertising();

    log_v("<<");
}

void GamepadBLE::gapEventHandler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    /* Note: This function is called by the bluetooth task, not by the gamepad application task */

    log_d("gapEventHandler [event no: %d]", (int) event);

    if (getInstance()->espBleGapEventSemaphore_ != nullptr)
    {
        switch (event)
        {
            case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
                xSemaphoreGive(getInstance()->espBleGapEventSemaphore_);
                break;

            case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
                xSemaphoreGive(getInstance()->espBleGapEventSemaphore_);
                break;

            case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT:
                xSemaphoreGive(getInstance()->espBleGapEventSemaphore_);
                break;

            case ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT:
                xSemaphoreGive(getInstance()->espBleGapEventSemaphore_);
                break;

            case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
                //xSemaphoreGive(getInstance()->espBleGapEventSemaphore_);
                break;

            default:
                break; // do nothing
        }
    }
}