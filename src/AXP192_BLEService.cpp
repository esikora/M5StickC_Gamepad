#include <Arduino.h>
#include "AXP192_BLEService.h"
#include "BLE2901.h"

AXP192_BLEService::AXP192_BLEService()
: powerService_{nullptr}
, chargeService_{nullptr}
, statusService_{nullptr}
, batVoltage_{nullptr}
, batPower_{nullptr}
, batChargeCurrent_{nullptr}
, coulombData_{nullptr}
, vBusPresent_{nullptr}
, acInPresent_{nullptr}
, currentDirection_{nullptr}
{
}

void AXP192_BLEService::start(BLEServer *pServer)
{
    Serial.println("[V][AXP192_BLEService.cpp] start(): >> start");
    Serial.flush();

    powerService_ = pServer->createService(kPowerServiceUUID, 100); // Number of handles needs to be increased, default is 15

    /***** Power service *****/

    // Characteristic: Battery voltage characteristic
    {
        batVoltage_ = powerService_->createCharacteristic(kBatVoltageUUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
        
        BLE2901 *pBle2901 = new BLE2901("Battery voltage");
        BLE2902 *pBle2902 = new BLE2902();
        BLE2904 *pBle2904 = new BLE2904();

        pBle2904->setFormat(BLE2904::FORMAT_FLOAT32); // IEEE Float 32
        pBle2904->setUnit(0x2728); // Volt
        pBle2904->setExponent(0);

        batVoltage_->addDescriptor(pBle2901);
        batVoltage_->addDescriptor(pBle2902);
        batVoltage_->addDescriptor(pBle2904);
    }
    
    // Characteristic: Battery power characteristic
    {
        batPower_ = powerService_->createCharacteristic(kBatPowerUUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

        BLE2901 *pBle2901 = new BLE2901("Battery power");
        BLE2902 *pBle2902 = new BLE2902();
        BLE2904 *pBle2904 = new BLE2904();

        pBle2904->setFormat(BLE2904::FORMAT_FLOAT32); // IEEE Float 32
        pBle2904->setUnit(0x2726); // Watt
        pBle2904->setExponent(-3); // mW

        batPower_->addDescriptor(pBle2901);
        batPower_->addDescriptor(pBle2902);
        batPower_->addDescriptor(pBle2904);
    }

    Serial.println("[V][AXP192_BLEService.cpp] start(): power service done");
    Serial.flush();

    /***** Charge service *****/
    chargeService_ = pServer->createService(kChargeServiceUUID);

    // Characteristic: Battery charge current
    {
        batChargeCurrent_ = chargeService_->createCharacteristic(kBatChargeCurrentUUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

        BLE2901 *pBle2901 = new BLE2901("Charge current");
        BLE2902 *pBle2902 = new BLE2902();
        BLE2904 *pBle2904 = new BLE2904();

        pBle2904->setFormat(BLE2904::FORMAT_FLOAT32); // IEEE Float 32
        pBle2904->setUnit(0x2704); // Ampere
        pBle2904->setExponent(-3); // mA

        batChargeCurrent_->addDescriptor(pBle2901);
        batChargeCurrent_->addDescriptor(pBle2902);
        batChargeCurrent_->addDescriptor(pBle2904);
    }
    
    // Characteristic: Coulomb data
    {
        coulombData_ = chargeService_->createCharacteristic(kCoulombDataUUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

        BLE2901 *pBle2901 = new BLE2901("Coulomb data");
        BLE2902 *pBle2902 = new BLE2902();
        BLE2904 *pBle2904 = new BLE2904();

        pBle2904->setFormat(BLE2904::FORMAT_FLOAT32); // IEEE Float 32
        pBle2904->setUnit(0x2727); // Coulomb
        pBle2904->setExponent(0);

        coulombData_->addDescriptor(pBle2901);
        coulombData_->addDescriptor(pBle2902);
        coulombData_->addDescriptor(pBle2904);
    }

    Serial.println("[V][AXP192_BLEService.cpp] start(): charge service done");
    Serial.flush();

    /***** Status service *****/

    statusService_ = pServer->createService(kStatusServiceUUID);

    // Characteristic: Current direction
    {
        currentDirection_ = statusService_->createCharacteristic(kCurrentDirectionUUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

        BLE2901 *pBle2901 = new BLE2901("Battery current direction: 0 = Discharging, 1 = Charging");
        BLE2904 *pBle2904 = new BLE2904();

        pBle2904->setFormat(BLE2904::FORMAT_UINT8);
        pBle2904->setUnit(0x2700); // Unitless
        pBle2904->setExponent(0);

        currentDirection_->addDescriptor(pBle2901);
        currentDirection_->addDescriptor(pBle2904);
    }

    // Characteristic: VBus present
    {
        vBusPresent_ = statusService_->createCharacteristic(kVBusPresentUUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

        BLE2901 *pBle2901 = new BLE2901("VBus present indication");
        BLE2904 *pBle2904 = new BLE2904();

        pBle2904->setFormat(BLE2904::FORMAT_BOOLEAN);
        pBle2904->setUnit(0x2700); // Unitless
        pBle2904->setExponent(0);

        vBusPresent_->addDescriptor(pBle2901);
        vBusPresent_->addDescriptor(pBle2904);
    }

    // Characteristic: VBus present
    {
        acInPresent_ = statusService_->createCharacteristic(kACInPresentUUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

        BLE2901 *pBle2901 = new BLE2901("ACin present indication");
        BLE2904 *pBle2904 = new BLE2904();

        pBle2904->setFormat(BLE2904::FORMAT_BOOLEAN);
        pBle2904->setUnit(0x2700); // Unitless
        pBle2904->setExponent(0);

        acInPresent_->addDescriptor(pBle2901);
        acInPresent_->addDescriptor(pBle2904);
    }

    Serial.println("[V][AXP192_BLEService.cpp] start(): status service done");
    Serial.flush();

    powerService_->start();
    Serial.println("[V][AXP192_BLEService.cpp] start(): power service started");
    Serial.flush();

    chargeService_->start();
    Serial.println("[V][AXP192_BLEService.cpp] start(): charge service started");
    Serial.flush();

    statusService_->start();
    Serial.println("[V][AXP192_BLEService.cpp] start(): status service started");
    Serial.flush();

    Serial.println("[V][AXP192_BLEService.cpp] start(): << start");
    Serial.flush();
}

/**
 * Sets the battery voltage value.
 * 
 * @param v : Volatage [V].
 */
void AXP192_BLEService::setBatVoltage(float v)
{
    batVoltage_->setValue(v);
    batVoltage_->notify();
}

/**
 * Sets the battery power value.
 * 
 * @param p : Power [mW].
 */
void AXP192_BLEService::setBatPower(float p)
{
    batPower_->setValue(p);
    batPower_->notify();
}

/**
 * Sets the battery charge current.
 * 
 * @param c : Charge current [mA].
 */
void AXP192_BLEService::setBatChargeCurrent(float c)
{
    batChargeCurrent_->setValue(c);
    batChargeCurrent_->notify();
}

/**
 * Sets the battery coulomb data.
 * 
 * @param c : Coulomb data [mAh].
 */
void AXP192_BLEService::setCoulombData(float c_mAh)
{
    float coulombValue = c_mAh * 3.6; // Output is in Coulomb = mAh * 3.6
    coulombData_->setValue(coulombValue);

    coulombData_->notify();
}

/**
 * Sets the current direction.
 * 
 * @param b : Start source: 0 = discharging, 1 = charging.
 */
void AXP192_BLEService::setCurrentDirection(uint8_t b)
{
    currentDirection_->setValue(&b, 1);
}

/**
 * Sets the VBus Present indication.
 * 
 * @param b : : 0 = VBUS not present, 1 = VBUS present.
 */
void AXP192_BLEService::setVBusPresent(uint8_t b)
{
    vBusPresent_->setValue(&b, 1);
}

/**
 * Sets the ACIn Present indication.
 * 
 * @param b : : 0 = ACIN not present, 1 = ACIN present.
 */
void AXP192_BLEService::setACInPresent(uint8_t b)
{
    acInPresent_->setValue(&b, 1);
}