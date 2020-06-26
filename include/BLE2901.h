#include <BLEDescriptor.h>

class BLE2901: public BLEDescriptor {
public:
	BLE2901(const std::string&);
	
    void setUserDescription(const std::string&);

}; // BLE2901
