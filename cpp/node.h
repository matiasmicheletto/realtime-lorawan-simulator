#ifndef NODE_H
#define NODE_H

#include <math.h>

class Node {
    private:
        unsigned int id;
        double x;
        double y;
    public:
        Node(double x, double y, unsigned int id);
        ~Node();
        
        void moveTo(double x, double y);
        double distanceTo(Node *node);

        inline unsigned int getId() { return this->id; };
};

#endif