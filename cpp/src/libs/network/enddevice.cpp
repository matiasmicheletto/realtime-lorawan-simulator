#include "enddevice.h"

EndDevice::EndDevice(double x, double y, unsigned int id, unsigned int period) : Node(x, y, id) {
    this->period = period;
    this->gw = NULL;
    this->sf = 0;
}

EndDevice::~EndDevice() {

}

void EndDevice::connect(Gateway *gw, unsigned char sf) {
    this->gw = gw;
    this->sf = sf;
}

void EndDevice::disconnect() {
    this->gw = NULL;
    this->sf = 0;
}
