#include <cstdlib>
#include <signal.h>
#include <iostream>
#include <sys/time.h>

#include <Ice/Ice.h>

#include "Sim.h"
#include "SimWorld.h"

// Globals
Shader *test;

// OpenGL callbacks
static int win;
static timeval lastDrawn;
void reshape(int width, int height);
void disp();
void keyb(unsigned char key, int x, int y);

// ICE callbacks
static Ice::CommunicatorPtr ic;
void ice_stop();

// Simulation world
ram::sim::SimWorld world;

// Signal handlers
void handle_kill(int);

void initialize()
{
	if (!Shader::supportsShaders())
	{
		std::cerr << "No support for shaders\n";
		return;
	}

	test = new Shader();
	if (!test->loadShaderFile("shaders/test.glsl"))
	{
		std::cerr << "Errors compiling shader" << std::endl;
		std::cerr << test->getErrors() << std::endl;
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
            Ice::ObjectPtr object = new ram::sim::SimVehicleFactory(world);
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
    
    gettimeofday(&lastDrawn, NULL);
    glutMainLoop();

    return 0;
}


void reshape(int width, int height)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, width, height);
    gluPerspective(35, (float)width/height, 0.1, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void disp()
{
    timeval now;
    gettimeofday(&now, NULL);
    btScalar elapsedSeconds = now.tv_sec - lastDrawn.tv_sec + (now.tv_usec - lastDrawn.tv_usec)/1000000.0;
    lastDrawn = now;
    
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    gluLookAt(0,30,10, 0,0,12, 0,0,1);
    world.stepSimulation(elapsedSeconds, 20);
    world.debugDraw();
    glPopMatrix();
    
    //glFlush();
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
