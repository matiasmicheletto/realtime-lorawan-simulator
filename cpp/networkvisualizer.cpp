#include "networkvisualizer.h"

NetworkVisualizer::NetworkVisualizer(int argc, char **argv) {
    glutInit(&argc, argv);
    glutCreateWindow("OpenGL Setup Test");
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(50, 50);
    this->init();
    glutMainLoop();
}

NetworkVisualizer::~NetworkVisualizer() {
    glutDestroyWindow(glutGetWindow());
}

void NetworkVisualizer::display() {
    instance->draw();
}

void NetworkVisualizer::init() {
    instance = this;
    glutDisplayFunc(NetworkVisualizer::display);
}

void NetworkVisualizer::draw() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
    glClear(GL_COLOR_BUFFER_BIT);         
    
    glBegin(GL_POINTS);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(-0.5f, this->data);
    glVertex2f( 0.5f, -0.5f);
    glVertex2f( 0.5f,  0.5f);
    glVertex2f(-0.5f,  0.5f);
    glEnd();

    glFlush();
}