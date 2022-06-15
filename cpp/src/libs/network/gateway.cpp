#include "gateway.h"

Gateway::Gateway(
        double x, 
        double y, 
        unsigned int id, 
        unsigned int hyperperiod,
        unsigned char channel) : Node(x, y, id) {
    this->H = hyperperiod;
    this->channel = channel;
    this->resetSlots();
}

Gateway::~Gateway() {
    
}

void Gateway::resetSlots() {
    this->maxSlots = 0;
    for(int i = 0; i < 6; i++){
        this->availableSlots[i] = this->H/pow(2,i);
        this->maxSlots += this->availableSlots[i];
    }
}

bool Gateway::useSlots(unsigned char sf, unsigned int period){
    if(sf >= 7 && sf <= 12){
        if(this->availableSlots[sf-7] >= this->H/period){
            this->availableSlots[sf-7] -= this->H/period;
            return true;
        }
    }
    return false;
}

void Gateway::freeSlots(unsigned char sf, unsigned int period){
    if(sf >= 7 && sf <= 12)
        this->availableSlots[sf-7] += this->H/period;
}

unsigned char Gateway::getMinSF(double distance) {
    if(distance < 62.5)
        return 7;
    else if(distance < 125)
        return 8;
    else if(distance < 250)
        return 9;
    else if(distance < 500)
        return 10;
    else if(distance < 1000)
        return 11;
    else if(distance < 2000)
        return 12;
    else // No SF for this distance
        return 13;
}

unsigned char Gateway::getMaxSF(unsigned int period) {
    if(period > 3200)
        return 12;
    else if(period > 1600)
        return 11;
    else if(period > 800)
        return 10;
    else if(period > 400)
        return 9;
    else if(period > 200)
        return 8;
    else if(period > 100)
        return 7;
    else // No SF for this period
        return 0;
}

bool Gateway::addEndDevice(EndDevice *ed) {
    if (!ed->isConnected()) { // End device can only be connected to a single GW
        // Get min and max SF
        unsigned char sf = getMinSF(this->distanceTo(ed));
        unsigned char maxSF = getMaxSF(ed->getPeriod());
        while(sf <= maxSF){ // Try to connect with minimum SF
            if(this->useSlots(sf, ed->getPeriod())){
                ed->connect(this, sf);
                this->connectedEDs.push_back(ed);
                return true;
            }else{
                sf++; // Next SF
            }
        }
    }
    return false;
}

bool Gateway::removeEndDevice(EndDevice *ed) {
    for (long unsigned int i = 0; i < this->connectedEDs.size(); i++) {
        if (this->connectedEDs[i] == ed) {
            this->freeSlots(ed->getSF(), ed->getPeriod());
            this->connectedEDs.erase(this->connectedEDs.begin() + i);
            ed->disconnect();
            return true;
        }
    }
    return false;
}

void Gateway::disconnect() {
    for (long unsigned int i = 0; i < this->connectedEDs.size(); i++)
        this->connectedEDs[i]->disconnect();
    this->connectedEDs.clear();
    this->resetSlots();
}

double Gateway::getUF() { // total available slots / total slots
    double sum = 0;
    for(int i = 0; i < 6; i++)
        sum += this->availableSlots[i];
    return sum/this->maxSlots;
}