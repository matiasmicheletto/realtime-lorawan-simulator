#ifndef NETWORKVISUALIZER_H
#define NETWORKVISUALIZER_H

#include <GL/glut.h>

class NetworkVisualizer {
    private:
        static void display();
        static NetworkVisualizer *instance;
        int data;

    public:
        NetworkVisualizer(int argc, char **argv);
        ~NetworkVisualizer();
        void init();
        void draw();
};

#endif