#ifndef NODE_H
#define NODE_H

#include <math.h>

class Node {
    public:
        Node(double x, double y, unsigned int id);
        ~Node();
        
        void moveTo(double x, double y);
        double distanceTo(double x, double y);
        double distanceTo(Node *node);

        inline unsigned int getId() { return this->id; };
        inline double getX() { return this->x; };
        inline double getY() { return this->y; };
    
    private:
        unsigned int id;
        double x;
        double y;
};

#endif