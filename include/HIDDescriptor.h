#include <Arduino.h>
#include <HIDTypes.h>

/**
 * HID report map (HID descriptor) for a generic gamepad Controller.
 * Based on: https://github.com/lemmingDev/ESP32-BLE-Gamepad/blob/master/BleGamepad.cpp
 * 
 * Service: Human Interface Device (UUID 0x1812)
 * Characteristic: Report Map (UUID 0x2A4B)
 * https://www.bluetooth.com/xml-viewer/?src=https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.report_map.xml
 * 
 * Field: Report Map Value
 */
static const uint8_t kGamepadReportMapGeneric2[] = {
    USAGE_PAGE(1),       0x01, // USAGE_PAGE (Generic Desktop)
    USAGE(1),            0x05, // USAGE (Gamepad)
    COLLECTION(1),       0x01, // COLLECTION (Application)
    USAGE(1),            0x01, //   USAGE (Pointer)
    COLLECTION(1),       0x00, //   COLLECTION (Physical)
    REPORT_ID(1),        0x01, //     REPORT_ID (1)
    // ------------------------------------------------- Buttons (1 to 14)
    USAGE_PAGE(1),       0x09, //     USAGE_PAGE (Button)
    USAGE_MINIMUM(1),    0x01, //     USAGE_MINIMUM (Button 1)
    USAGE_MAXIMUM(1),    0x0e, //     USAGE_MAXIMUM (Button 14)
    LOGICAL_MINIMUM(1),  0x00, //     LOGICAL_MINIMUM (0)
    LOGICAL_MAXIMUM(1),  0x01, //     LOGICAL_MAXIMUM (1)
    REPORT_SIZE(1),      0x01, //     REPORT_SIZE (1)
    REPORT_COUNT(1),     0x0e, //     REPORT_COUNT (14)
    HIDINPUT(1),         0x02, //     INPUT (Data, Variable, Absolute)
    // ------------------------------------------------- Padding
    REPORT_SIZE(1),      0x01, //     REPORT_SIZE (1)
    REPORT_COUNT(1),     0x02, //     REPORT_COUNT (2)
    HIDINPUT(1),         0x03, //     INPUT (Constant, Variable, Absolute); 2 bit padding
    // ------------------------------------------------- X/Y position, Z/rZ position
    USAGE_PAGE(1),       0x01, //     USAGE_PAGE (Generic Desktop)
    USAGE(1),            0x30, //     USAGE (X)
    USAGE(1),            0x31, //     USAGE (Y)
    USAGE(1),            0x32, //     USAGE (Z)
    USAGE(1),            0x35, //     USAGE (rZ)
    LOGICAL_MINIMUM(2),  0x00, 0x80, //     LOGICAL_MINIMUM (-32768)
    LOGICAL_MAXIMUM(2),  0xff, 0x7f, //     LOGICAL_MAXIMUM (32767)
    REPORT_SIZE(1),      0x10, //     REPORT_SIZE (16)
    REPORT_COUNT(1),     0x04, //     REPORT_COUNT (4)
    HIDINPUT(1),         0x02, //     INPUT (Data, Variable, Absolute) ;4 bytes (X,Y,Z,rZ)

    USAGE_PAGE(1),       0x01, //     USAGE_PAGE (Generic Desktop)
    USAGE(1),            0x33, //     USAGE (rX) Left Trigger
    USAGE(1),            0x34, //     USAGE (rY) Right Trigger
    LOGICAL_MINIMUM(1),  0x81, //     LOGICAL_MINIMUM (-127)
    LOGICAL_MAXIMUM(1),  0x7f, //     LOGICAL_MAXIMUM (127)
    REPORT_SIZE(1),      0x08, //     REPORT_SIZE (8)
    REPORT_COUNT(1),     0x02, //     REPORT_COUNT (2)
    HIDINPUT(1),         0x02, //     INPUT (Data, Variable, Absolute) ;2 bytes rX, rY

    USAGE_PAGE(1),       0x01, //     USAGE_PAGE (Generic Desktop)
    USAGE(1),            0x39, //     USAGE (Hat switch)
    USAGE(1),            0x39, //     USAGE (Hat switch)
    LOGICAL_MINIMUM(1),  0x01, //     LOGICAL_MINIMUM (1)
    LOGICAL_MAXIMUM(1),  0x08, //     LOGICAL_MAXIMUM (8)
    REPORT_SIZE(1),      0x04, //     REPORT_SIZE (4)
    REPORT_COUNT(1),     0x02, //     REPORT_COUNT (2)
    HIDINPUT(1),         0x02, //     INPUT (Data, Variable, Absolute) ;1 byte Hat1, Hat2

    END_COLLECTION(0),         //     END_COLLECTION
    END_COLLECTION(0)          //     END_COLLECTION
};

/**
 * Message size in bytes for the report ID 0x01 specified by the report map kReportDescriptorGeneric2.
 */
static const uint8_t kGamepadReportSizeGeneric2 = 13;

/**
 * Report ID 0x01 struct for Generic HID over GATT controller.
 */
#pragma pack(push, 1)
typedef struct
{
    uint8_t  btn01      :  1; // Button 1 Primary/trigger, Value = 0 to 1
    uint8_t  btn02      :  1; // Button 2 Secondary, Value = 0 to 1
    uint8_t  btn03      :  1; // Button 3 Tertiary, Value = 0 to 1
    uint8_t  btn04      :  1; // Button 4, Value = 0 to 1
    uint8_t  btn05      :  1; // Button 5, Value = 0 to 1
    uint8_t  btn06      :  1; // Button 6, Value = 0 to 1
    uint8_t  btn07      :  1; // Button 7, Value = 0 to 1
    uint8_t  btn08      :  1; // Button 8, Value = 0 to 1
    uint8_t  btn09      :  1; // Button 9, Value = 0 to 1
    uint8_t  btn10      :  1; // Button 10, Value = 0 to 1
    uint8_t  btn11      :  1; // Button 11, Value = 0 to 1
    uint8_t  btn12      :  1; // Button 12, Value = 0 to 1
    uint8_t  btn13      :  1; // Button 13, Value = 0 to 1
    uint8_t  btn14      :  1; // Button 14, Value = 0 to 1

    uint8_t             :  2; // Pad

    int16_t stickLX;           // Left stick X value -32768..32767
    int16_t stickLY;           // Left stick Y value -32768..32767

    int16_t stickRX;           // Right stick X value -32768..32767
    int16_t stickRY;           // Right stick Y value -32768..32767

    int8_t btnLT;             // LT value -127..127
    int8_t btnRT;             // RT value -127..127

    uint8_t hatSwitch1  :  4; // Hat switch, value = 1 to 8, Physical = (Value - 1) x 45 in degrees
    uint8_t hatSwitch2  :  4; // Hat switch, value = 1 to 8, Physical = (Value - 1) x 45 in degrees
} tGamepadReportStructGeneric2;
#pragma pack(pop)


/**
 * Struct type definition for Bluetooth LE device information.
 * Encompasses several Bluetooth characteristics.
 */
typedef struct {

    /**
     * Service: Generic Access (0x1800)
     * Characteristic: Device Name (UUID 0x2A00)
     * https://www.bluetooth.com/xml-viewer/?src=https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.gap.device_name.xml
     *
     * Field: Name
     */
    std::string deviceName;

    /**
     * Service: Device Information (UUID 0x180A)
     * Characteristic: PnP ID (UUID 0x2A50)
     * https://www.bluetooth.com/xml-viewer/?src=https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.pnp_id.xml
     *
     * Field: Vendor ID Source
     *   1 = Bluetooth SIG assigned Company Identifier value from the Assigned Numbers document
     *       https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers/
     * 
     *   2 = USB Implementer’s Forum assigned Vendor ID value
     *       https://www.usb.org/developers --> Valid USB Vendor ID Numbers
     */
    uint8_t vendorIdSource;

    /**
     * Service: Device Information (UUID 0x180A)
     * Characteristic: PnP ID (UUID 0x2A50)
     * https://www.bluetooth.com/xml-viewer/?src=https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.pnp_id.xml
     * 
     * Field: Vendor ID
     * Identifies the product vendor from the namespace in the Vendor ID Source.
     */
    uint16_t vendorId;

    /**
     * Service: Device Information (UUID 0x180A)
     * Characteristic: PnP ID (UUID 0x2A50)
     * https://www.bluetooth.com/xml-viewer/?src=https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.pnp_id.xml
     * 
     * Field: Product ID
     * Manufacturer managed identifier for this product
     */
    uint16_t productId;

    /**
     * Service: Device Information (UUID 0x180A)
     * Characteristic: PnP ID (UUID 0x2A50)
     * https://www.bluetooth.com/xml-viewer/?src=https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.pnp_id.xml
     * 
     * Field: Product version
     * Manufacturer managed version for this product
     */
    uint16_t productVersion;

    /**
     * Service: Device Information (UUID 0x180A)
     * Characteristic: Manufacturer Name String (UUID 0x2A29)
     * https://www.bluetooth.com/xml-viewer/?src=https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.manufacturer_name_string.xml
     * 
     * Field: Manufacturer Name
     * The value of this characteristic is a UTF-8 string representing the name of the manufacturer of the device. 
     */
    std::string manufacturerNameString;

    /**
     * Service: Human Interface Device (UUID 0x1812) 
     * Characteristic: HID Information (UUID 0x2A4A)
     * https://www.bluetooth.com/xml-viewer/?src=https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.hid_information.xml
     * 
     * Field: bCountryCode
     * Identifies which country the hardware is localized for. Most hardware is not localized and thus this value would be zero (0).
     */
    uint8_t country;

    /**
     * Service: Human Interface Device (UUID 0x1812) 
     * Characteristic: HID Information (UUID 0x2A4A)
     * https://www.bluetooth.com/xml-viewer/?src=https://www.bluetooth.com/wp-content/uploads/Sitecore-Media-Library/Gatt/Xml/Characteristics/org.bluetooth.characteristic.hid_information.xml
     * 
     * Field: Flags
     * - Index 0: RemoteWake
     * - Index 1: NormallyConnectable
     */    
    uint8_t flags;

} tDeviceInfo;


/* GATT Characteristic 'PNP ID':

    Source: https://www.partsnotincluded.com/understanding-the-xbox-360-wired-controllers-usb-data/

    1. Vendor ID Source: 0x02 = 'USB Implementer’s Forum assigned Vendor ID value'
    2. Vendor ID: 0x045E = 'Microsoft Corporation'
    3. Product ID: 0x02FD = 'XBOX ONE S Controller [Bluetooth]'
    4. Product Version = 'xxx' (TBD)
*/
//pHIDdevice_->pnp(0x02, 0x045E, 0x02FD, 0x0903);

/* GATT Characteristic 'HID information':
         1. Country: 0x00 = not localized 
         2. Flags (bitfield): 0x03 = 'bit0: capable of providing wake-up signal to a HID host' | 'bit1: normally connectable'
    */    
    //pHIDdevice_->hidInfo(0x00, 0x02);

/**
 * Device info for a gamepad based on:
 * https://github.com/lemmingDev/ESP32-BLE-Gamepad/blob/master/BleGamepad.cpp
 */
static const tDeviceInfo kGamepadDeviceInfoGeneric2 =
{
    "ESP32 Wireless Gamepad",
    0x01,
    0x02E5,
    0xABCD,
    0x0110,
    "DIY",
    0x00,
    0x01
};

/**
 * Mappings of constants and structs to be applied for the specific gamepad device.
 */

// Device information for this Gamepad
static const tDeviceInfo kGamepadDeviceInfo = kGamepadDeviceInfoGeneric2;

// Report map value for this Gamepad
static const uint8_t *pGamepadReportMap = kGamepadReportMapGeneric2;

// Report map size
static const uint16_t kGamepadReportMapSize = sizeof(kGamepadReportMapGeneric2);

// Expected report size
static const uint8_t kGamepadReportSize = kGamepadReportSizeGeneric2;

// Struct type definition for the report value
typedef tGamepadReportStructGeneric2 tGamepadReportStruct;