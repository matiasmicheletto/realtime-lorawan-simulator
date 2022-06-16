#include "node.h"

Node::Node(double x, double y, unsigned int id) {
    this->x = x;
    this->y = y;
    this->id = id;
};

Node::~Node() {
};

void Node::moveTo(double x, double y) {
    this->x = x;
    this->y = y;
};

double Node::distanceTo(Node *node) {
    return sqrt(pow(this->x - node->x, 2) + pow(this->y - node->y, 2));
};