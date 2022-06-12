#ifndef NODE_H
#define NODE_H

#include <math.h>

class Node {
    private:
        int x;
        int y;
    public:
        Node(int x, int y);
        ~Node();
        void moveTo(int x, int y);
        double distanceTo(Node *node);
};

#endif