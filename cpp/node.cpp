#include "node.h"

Node::Node(int x, int y) {
    this->x = x;
    this->y = y;
};

Node::~Node() {
};

void Node::moveTo(int x, int y) {
    this->x = x;
    this->y = y;
};

double Node::distanceTo(Node *node) {
    return sqrt(pow(this->x - node->x, 2) + pow(this->y - node->y, 2));
};