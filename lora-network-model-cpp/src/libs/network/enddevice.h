#ifndef ENDDEVICE_H
#define ENDDEVICE_H

class Gateway; // Forward declaration

#include "node.h"
#include "gateway.h"

class EndDevice : public Node {
    private:
        unsigned int period;
        unsigned char sf;
        Gateway *gw;
        
    public: 
        EndDevice(double x, double y, unsigned int id, unsigned int period);
        ~EndDevice();

        void connect(Gateway *gw, unsigned char sf);
        void disconnect();

        inline unsigned int getPeriod() { return this->period; };
        inline bool isConnected() { return this->gw != NULL; };
        inline unsigned char getSF() { return this->sf; };
        inline Gateway *getGateway() { return this->gw; };
        unsigned int getGatewayId();
        double getGWDist();
};

#endif