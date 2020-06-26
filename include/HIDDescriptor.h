#include <Arduino.h>
#include <HIDTypes.h>

/**
 * HID report descriptor for a generic gamepad Controller.
 * 
 * Based on: https://github.com/lemmingDev/ESP32-BLE-Gamepad/blob/master/BleGamepad.cpp
 * 
 */
static const uint8_t kReportDescriptorGeneric2[] = {
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
 * Message size in bytes of the report 'Generic2'.
 */
static const uint8_t kMsgSizeReportGeneric2 = 13;

/**
 * Input report ID 0x01 struct for Generic HID over GATT controller.
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
} StructGamepadInputGeneric2_t;
#pragma pack(pop)