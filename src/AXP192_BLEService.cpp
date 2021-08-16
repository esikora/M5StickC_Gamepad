/**
    M5StickC_GamepadApp:
    This application has been developed to use an M5StickC device (ESP32)
    as bluetooth gamepad input device. It reads a joystick position and
    button status and provides these data via Bluetooth Low Energy (BLE)
    using the Human Interface Device (HID) via GATT protocol.
    Copyright (C) 2020 by Ernst Sikora
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Arduino.h>
#include "AXP192_BLEService.h"
#include "BLE2901.h"

AXP192_BLEService::AXP192_BLEService()
: powerService_{nullptr}
, batVoltage_{nullptr}
, batPower_{nullptr}
, batChargeCurrent_{nullptr}
, coulombData_{nullptr}
, vBusPresent_{nullptr}
, vBusAvailable_{nullptr}
, acInPresent_{nullptr}
, acInAvailable_{nullptr}
, currentDirection_{nullptr}
{
}

void AXP192_BLEService::start(BLEServer *pServer)
{
    log_v(">>");

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

    // Characteristic: Battery charge current
    {
        batChargeCurrent_ = powerService_->createCharacteristic(kBatChargeCurrentUUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

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
        coulombData_ = powerService_->createCharacteristic(kCoulombDataUUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

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

    // Characteristic: Current direction
    {
        currentDirection_ = powerService_->createCharacteristic(kCurrentDirectionUUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

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
        vBusPresent_ = powerService_->createCharacteristic(kVBusPresentUUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

        BLE2901 *pBle2901 = new BLE2901("VBus present indication");
        BLE2904 *pBle2904 = new BLE2904();

        pBle2904->setFormat(BLE2904::FORMAT_BOOLEAN);
        pBle2904->setUnit(0x2700); // Unitless
        pBle2904->setExponent(0);

        vBusPresent_->addDescriptor(pBle2901);
        vBusPresent_->addDescriptor(pBle2904);
    }

    // Characteristic: VBus available
    {
        vBusAvailable_ = powerService_->createCharacteristic(kVBusAvailableUUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

        BLE2901 *pBle2901 = new BLE2901("VBus available indication");
        BLE2904 *pBle2904 = new BLE2904();

        pBle2904->setFormat(BLE2904::FORMAT_BOOLEAN);
        pBle2904->setUnit(0x2700); // Unitless
        pBle2904->setExponent(0);

        vBusAvailable_->addDescriptor(pBle2901);
        vBusAvailable_->addDescriptor(pBle2904);
    }

    // Characteristic: ACin present
    {
        acInPresent_ = powerService_->createCharacteristic(kACInPresentUUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

        BLE2901 *pBle2901 = new BLE2901("ACin present indication");
        BLE2904 *pBle2904 = new BLE2904();

        pBle2904->setFormat(BLE2904::FORMAT_BOOLEAN);
        pBle2904->setUnit(0x2700); // Unitless
        pBle2904->setExponent(0);

        acInPresent_->addDescriptor(pBle2901);
        acInPresent_->addDescriptor(pBle2904);
    }

    // Characteristic: ACin available
    {
        acInAvailable_ = powerService_->createCharacteristic(kACInAvailableUUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

        BLE2901 *pBle2901 = new BLE2901("ACin available indication");
        BLE2904 *pBle2904 = new BLE2904();

        pBle2904->setFormat(BLE2904::FORMAT_BOOLEAN);
        pBle2904->setUnit(0x2700); // Unitless
        pBle2904->setExponent(0);

        acInAvailable_->addDescriptor(pBle2901);
        acInAvailable_->addDescriptor(pBle2904);
    }

    log_v("Starting power service.");

    powerService_->start();

    log_v("<<");
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

void AXP192_BLEService::setVBusAvailable(uint8_t b)
{
    vBusAvailable_->setValue(&b, 1);
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

void AXP192_BLEService::setACInAvailable(uint8_t b)
{
    acInAvailable_->setValue(&b, 1);
}
