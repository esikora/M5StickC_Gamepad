#include <BLECharacteristic.h>
#include <BLEService.h>
#include <BLEDescriptor.h>
#include <BLE2902.h>
#include <BLE2904.h>

class AXP192_BLEService {

    public:

        const BLEUUID kPowerServiceUUID     {"36957D1B-B095-4719-80A0-000000000001"};
        const BLEUUID kChargeServiceUUID    {"36957D1B-B095-4719-80A0-000000000002"};
        const BLEUUID kStatusServiceUUID    {"36957D1B-B095-4719-80A0-000000000003"};
        const BLEUUID kModeServiceUUID      {"36957D1B-B095-4719-80A0-000000000004"};

        const BLEUUID kBatVoltageUUID       {"69A85E1E-7F28-4B5A-B28B-98DBF68F3BEC"};
        const BLEUUID kBatPowerUUID         {"B7C4D3CF-B5F7-4AA8-BDD6-AA465FB11B3F"};
        const BLEUUID kBatChargeCurrentUUID {"36957D1B-1E63-4FB9-8EDA-54DC9AF664D2"};
        const BLEUUID kCoulombDataUUID      {"791DD3FC-A7C6-44F8-8ABF-142197D845BE"};
        const BLEUUID kStartSourceUUID      {"22621BD6-47B2-4D76-8B7E-24F85D1B028F"};
        const BLEUUID kCurrentDirectionUUID {"BDC3B070-0A3C-43A1-B763-993B77361169"};
        const BLEUUID kVBusAvailableUUID    {"06AA3C57-B9D8-4406-8B15-CFC5C342E695"};
        const BLEUUID kVBusPresentUUID      {"CDA89D3B-A3A0-4491-919B-215235EF9DC7"};
        const BLEUUID kACInAvailableUUID    {"A27F0272-5A4D-4A79-8DF8-ACC056627B87"};
        const BLEUUID kACInPresentUUID      {"F3D5731A-4DCE-4BDA-8292-20A77F9E962C"};
        const BLEUUID kBatteryPresenceUUID  {"CD0938BE-316D-468C-AAA5-636FCFA464AE"};
        const BLEUUID kChargeIndicationUUID {"4DDA2290-3214-4F23-BBEC-C0F95B8D7B3D"};
        

        AXP192_BLEService();

        void start(BLEServer*);

        /**
         * Sets the battery voltage value.
         * 
         * @param v : Volatage [V].
         */
        void setBatVoltage(float v);

        /**
         * Sets the battery power value.
         * 
         * @param p : Power [mW].
         */
        void setBatPower(float p);

        /**
         * Sets the battery charge current.
         * 
         * @param c : Charge current [mA].
         */
        void setBatChargeCurrent(float c);

        /**
         * Sets the battery coulomb data.
         * 
         * @param c_mAh : Coulomb data [mAh].
         */
        void setCoulombData(float c_mAh);

        /**
         * Sets the current direction.
         * 
         * @param b : Start source: 0 = discharging, 1 = charging.
         */
        void setCurrentDirection(uint8_t b);

        /**
         * Sets the VBus Present indication.
         * 
         * @param b : : 0 = VBUS not present, 1 = VBUS present.
         */
        void setVBusPresent(uint8_t b);

        /**
         * Sets the ACIn Present indication.
         * 
         * @param b : : 0 = ACIN not present, 1 = ACIN present.
         */
        void setACInPresent(uint8_t b);

    private:
        BLEService*			powerService_;
        BLEService*         chargeService_;
        BLEService*         statusService_;

        BLECharacteristic* 	batVoltage_;
        BLECharacteristic* 	batPower_;
        BLECharacteristic* 	batChargeCurrent_;
        BLECharacteristic* 	coulombData_;

        BLECharacteristic*	vBusPresent_;
        BLECharacteristic*	acInPresent_;
        BLECharacteristic*	currentDirection_;

};