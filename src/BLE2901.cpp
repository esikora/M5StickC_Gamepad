#include "BLE2901.h"

BLE2901::BLE2901(const std::string &str) : BLEDescriptor(BLEUUID((uint16_t) 0x2901))
{
	setValue(str);
    setAccessPermissions(ESP_GATT_PERM_READ);
} // BLE2901

void BLE2901::setUserDescription(const std::string &str)
{
    setValue(str);
}
