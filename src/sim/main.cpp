#include <SDL.h>
#include <SDL_opengl.h>

#include <cstdlib>
#include <signal.h>
#include <iostream>
#include <boost/lexical_cast.hpp>

#include <Ice/Ice.h>

#include "SimWorld.h"
#include "Camera.h"
#include "vehicle/SimVehicleFactory.h"

// Globals
//Shader *test;
ram::sim::Camera cam;
double dollyRate = 4;
double tumbleRate = 1;
double trackRate = 1;

// OpenGL callbacks
void reshape(int width, int height);
void disp(double);

// ICE callbacks
static Ice::CommunicatorPtr ic;
void ice_stop();

// Simulation world
ram::sim::SimWorld world;

// Signal handlers
void handle_kill(int);

/*
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
*/

int main(int argc, char*argv[])
{
    int sdlStatus = SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO);
    if (sdlStatus < 0)
    {
        std::cerr << "Unable to init SDL: " << SDL_GetError() << std::endl;
        exit(sdlStatus);
    }
    
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,   16);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE,     8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,   8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,    8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,   8);
    
    Uint32 flags = SDL_OPENGL;
    SDL_Surface* drawContext = SDL_SetVideoMode(1024, 768, 0, flags);
    
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
    
    bool running = true;
    SDL_Event event;
    Uint32 millisSinceStartup = SDL_GetTicks();
    double elapsedSeconds;
    reshape(1024,768);
    bool mouseButtonDownFlag[256];
    bzero(mouseButtonDownFlag, sizeof(*mouseButtonDownFlag) * 256);
    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_MOUSEBUTTONDOWN:
                {
                    SDL_MouseButtonEvent* mouseButtonEvent = (SDL_MouseButtonEvent*)&event;
                    if (mouseButtonEvent->button == SDL_BUTTON_WHEELUP)
                        cam.dollyIn(dollyRate);
                    else if (mouseButtonEvent->button == SDL_BUTTON_WHEELDOWN)
                        cam.dollyOut(dollyRate);
                    mouseButtonDownFlag[mouseButtonEvent->button] = true;
                } break;
                case SDL_MOUSEBUTTONUP:
                {
                    SDL_MouseButtonEvent* mouseButtonEvent = (SDL_MouseButtonEvent*)&event;
                    mouseButtonDownFlag[mouseButtonEvent->button] = false;
                } break;
                case SDL_MOUSEMOTION:
                {
                    SDL_MouseMotionEvent* mouseMotionEvent = (SDL_MouseMotionEvent*)&event;
                    if (mouseButtonDownFlag[SDL_BUTTON_LEFT] && mouseMotionEvent->state == SDL_PRESSED)
                    {
                        cam.tumbleYaw(mouseMotionEvent->xrel * tumbleRate);
                        cam.tumblePitch(mouseMotionEvent->yrel * tumbleRate);
                    }
                    if (mouseButtonDownFlag[SDL_BUTTON_RIGHT])
                    {
                        cam.trackLeft(mouseMotionEvent->xrel * trackRate);
                        cam.trackUp(mouseMotionEvent->yrel * trackRate);
                    }
                } break;
                case SDL_QUIT:
                    running = false;
                    break;
            }
        }
        Uint32 newTime = SDL_GetTicks();
        elapsedSeconds = (newTime - millisSinceStartup) * 0.001;
        millisSinceStartup = newTime;
        disp(elapsedSeconds);
        SDL_GL_SwapBuffers();
    }
    
    SDL_Quit();

    return 0;
}


void reshape(int width, int height)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, width, height);
    gluPerspective(35, (float)width/height, 0.1, 1000);
    
    GLfloat fogColor[4] = {0.1f, 0.1f, 0.1f, 1.0f};
    glFogi(GL_FOG_MODE, GL_EXP);
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogf(GL_FOG_DENSITY, 0.05f);
    glHint(GL_FOG_HINT, GL_DONT_CARE);
    glFogf(GL_FOG_START, 1.0f);
    glFogf(GL_FOG_END, 10000.0f);
    glEnable(GL_FOG);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
}


void disp(double elapsedSeconds)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    glTranslatef(0,20,0);
    cam.glTransform();
    world.stepSimulation(elapsedSeconds, 20);
    world.debugDraw();
    glPopMatrix();
    
    //GraphicsUtils::drawText(20, 40, "Frame rate       : " + boost::lexical_cast<std::string>(int(1/elapsedSeconds)) + " FPS");
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
    exit(1);
}
