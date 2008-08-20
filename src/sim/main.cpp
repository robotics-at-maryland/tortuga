#if defined(__APPLE__) || defined(MACOSX)
    #include <GLUT/glut.h>
    #include <OpenGL/gl.h>
#else
    #include <GL/glut.h>
    #include <GL/gl.h>
#endif

#include <cstdlib>
#include <iostream>
#include <Ice/Ice.h>

#include "vehicle/SimVehicleFactory.h"

// OpenGL callbacks
static int win;
void reshape(int width, int height);
void disp();
void keyb(unsigned char key, int x, int y);

// ICE callbacks
static Ice::CommunicatorPtr ic;
void ice_stop();

int main(int argc, char **argv)
{
    // Initalize GLUT
    glutInit(&argc, argv);
    
    // RGB+Alpha mode, double buffered
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    
    // Set window size
    glutInitWindowSize(320, 240);
    
    // Position where the window will appear
    glutInitWindowPosition(100, 100);
    
    // If you want full screen:
    // glutFullScreen();
    
    // Create window and set the title
    win = glutCreateWindow("Robotics@Maryland Vehicle Simulator");
    
    // Set callbacks
    glutDisplayFunc(disp);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyb);
    
    // Define color we use to clear the screen
    glClearColor(0.0, 0.0, 0.0, 0.0);
    
    // ICE initialization
    {
        int status = 0;
        atexit(ice_stop);
        try {
            ic = Ice::initialize(argc, argv);
            Ice::ObjectAdapterPtr adapter
            = ic->createObjectAdapterWithEndpoints("SimAdapter", "default -p 10000");
            Ice::ObjectPtr object = new ram::sim::SimVehicleFactory;
            adapter->add(object, ic->stringToIdentity("factory"));
            adapter->activate();
        } catch (const Ice::Exception& e) {
            std::cerr << e << std::endl;
            status = 1;
        } catch (const char* msg) {
            std::cerr << msg << std::endl;
            status = 1;
        }
        if (status == 1)
        {
            return status;
        }
    }
    
    // Enter run loop
    glutMainLoop();
    
    return 0;
}


void reshape(int width, int height)
{
}


void disp()
{
}


void keyb(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'q':
            glutDestroyWindow(win);
            exit(0);
            break;
    }
}


void ice_stop()
{
    std::cerr << "Attempting to shut down ICE." << std::endl;
    if (ic) {
        try {
            ic->destroy();
            std::cerr << "ICE is now down." << std::endl;
        } catch (const Ice::Exception& e) {
            std::cerr << "Could not destroy ICE: " << e << std::endl;
        }
    }
}
