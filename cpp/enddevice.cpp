
#include "enddevice.h"

EndDevice::EndDevice(int x, int y, int period) : Node(x, y) {
    this->period = period;
}

EndDevice::~EndDevice() {

}