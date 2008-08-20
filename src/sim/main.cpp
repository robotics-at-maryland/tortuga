#include <cstdlib>
#include <signal.h>
#include <iostream>
#include <Ice/Ice.h>

#include "Sim.h"

// Globals
Shader *test;

// OpenGL callbacks
static int win;
void reshape(int width, int height);
void disp();
void keyb(unsigned char key, int x, int y);

// ICE callbacks
static Ice::CommunicatorPtr ic;
void ice_stop();

// Signal handlers
void handle_kill(int);

void initialize()
{
	test = new Shader();
	if (!test->loadShaderFile("shaders/test.glsl"))
	{
		std::cout << "Errors compiling shader\n";
		std::cout << test->getErrors();
	}
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(800, 600);
    
    // If you want full screen:
    // glutFullScreen();
    
    win = glutCreateWindow("Robotics@Maryland Vehicle Simulator");
    
    // Set callbacks
    glutDisplayFunc(disp);
	glutIdleFunc(disp);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyb);

    
    // ICE initialization
    {
        int status = 0;
        atexit(ice_stop);
        signal(SIGTERM, handle_kill);
        signal(SIGABRT, handle_kill);
        signal(SIGINT, handle_kill);
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

	initialize();
    
    std::cerr << "Starting GLUT main loop." << std::endl;
    
    glutMainLoop();

    return 0;
}


void reshape(int width, int height)
{
}


void disp()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor3f(1, 0.5f, 0.5f);
	GraphicsUtils::drawText(50, 50, "This is Text!!!!");

	glutSwapBuffers();
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
	std::cerr.flush();
    if (ic) {
        std::cerr << "Attempting to shut down ICE." << std::endl;
        try {
            ic->destroy();
            std::cerr << "ICE is now down." << std::endl;
        } catch (const Ice::Exception& e) {
            std::cerr << "Could not destroy ICE: " << e << std::endl;
        }
    }
}


void handle_kill(int a)
{
    std::cerr << "Terminating program." << std::endl;
    glutDestroyWindow(win);
    exit(1);
}
