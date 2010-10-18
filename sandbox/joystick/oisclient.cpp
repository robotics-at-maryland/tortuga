//////////////////////////////// OS Neutral Headers ////////////////
#include "OISInputManager.h"
#include "OISException.h"
#include "OISKeyboard.h"
#include "OISMouse.h"
#include "OISJoyStick.h"
#include "OISEvents.h"

//Advanced Usage
#include "OISForceFeedback.h"

#include <iostream>
#include <vector>
#include <sstream>


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "commands.h"

#define PORT 9219

#define TYPE_AXIS 0
#define TYPE_BTN  1


int curSpeed=0;

////////////////////////////////////Needed Windows Headers////////////
#if defined OIS_WIN32_PLATFORM
#  define WIN32_LEAN_AND_MEAN
#  include "windows.h"
#  include "resource.h"
LRESULT DlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
//////////////////////////////////////////////////////////////////////
////////////////////////////////////Needed Linux Headers//////////////
#elif defined OIS_LINUX_PLATFORM
#  include <X11/Xlib.h>
void checkX11Events();
#endif
//////////////////////////////////////////////////////////////////////
using namespace OIS;

//-- Some local prototypes --//
void doStartup();
void handleNonBufferedKeys();
void handleNonBufferedMouse();
void handleNonBufferedJoy( JoyStick* js );

//-- Some hacky globals --//
bool appRunning = true; //Global Exit Flag

//-- These two globals control the torpedo and marker dropper functionality --//
int armCommand = 0;

InputManager *g_InputManager = 0;
Keyboard *g_kb  = 0; //Keyboard Device
Mouse *g_m   = 0; //Mouse Device
JoyStick* g_joys[4] = {0,0,0,0}; // This demo supports up to 4 controllers
ForceFeedback* g_ff[4] = {0,0,0,0}; // Array to hold ff interface for each joy

///// OS Specific Globals //////
#if defined OIS_WIN32_PLATFORM
HWND hWnd = 0;
#elif defined OIS_LINUX_PLATFORM
Display *xDisp = 0;
Window xWin = 0;
#endif

int sockfd=0;

struct sockaddr_in their_addr; /*connector's address information */


int yawCmd = 0;
int pitchCmd = 0;
int rollCmd = 0;

void sendCmd(int fd, unsigned char cmd, signed char param)
{
    signed char buf[2];
    buf[0]=cmd;
    buf[1]=param;

    if(sendto(fd, buf, 2, 0, (struct sockaddr *) &their_addr, sizeof(struct sockaddr_in)) == -1)
    {
        printf("Cant send\n");
    }
}

/* Speeds to send.. ie, -SPEED_RANGE to +SPEED_RANGE */
#define SPEED_RANGE 5
#define TSPEED_RANGE 5

#define YAW_RANGE 10

/* Don't send same speed twice */
int lastAxisSpeed = 0;
int lastThrottleSpeed = 0;
int lastAxisTSpeed = 0;

/* Load specific settings */
#ifdef SAITEK
#  include "saitek.h"
#elif LOGITECH
#  include "logitech.h"
#else
#  include "generic.h"
#endif

void processButtonPress(int fd, int btn)
{
    switch(btn)
    {
    case BTN_INCSPEED:
    {
        printf("Increase speed\n");
        sendCmd(fd, CMD_INCSPEED, 0);
        break;
    }

    case BTN_DECSPEED:
    {
        printf("Decrease speed\n");
        sendCmd(fd, CMD_DECSPEED, 0);
        break;
    }

    case BTN_ZEROSPEED:
    {
        printf("Zero speed\n");
        sendCmd(fd, CMD_ZEROSPEED, 0);
        break;
    }

    case BTN_EMERGSTOP:
    {
        printf("Emergency stop\n");
        sendCmd(fd, CMD_EMERGSTOP, 0);
        break;
    }

    case BTN_ASCEND:
    {
        printf("Ascend\n");
        sendCmd(fd, CMD_ASCEND, 0);
        break;
    }

    case BTN_DESCEND:
    {
        printf("Descend\n");
        sendCmd(fd, CMD_DESCEND, 0);
        break;
    }

    case BTN_TURNLEFT:
    {
        printf("Turn left\n");
        sendCmd(fd, CMD_TURNLEFT, 0);
        break;
    }

    case BTN_TURNRIGHT:
    {
        printf("Turn right\n");
        sendCmd(fd, CMD_TURNRIGHT, 0);
        break;
    }

    case BTN_ARM_TORPEDO:
    {
        if (armCommand == CMD_FIRE_TORPEDO)
        {
            // No command is sent
            printf("Disarm torpedo launcher\n");
            armCommand = 0;
            // Break out of the switch statement
            break;
        }
        else if (armCommand == CMD_DROP_MARKER)
        {
            // Disarm the markers and arm the torpedo
            printf("Disarm marker dropper\n");
            armCommand = 0;
        }
        // Arm the torpedos
        printf("Arm torpedo launcher\n");
        armCommand = CMD_FIRE_TORPEDO;
        break;
    }

    case BTN_ARM_MARKER:
    {
        if (armCommand == CMD_DROP_MARKER)
        {
            // No command is sent
            printf("Disarm marker dropper\n");
            armCommand = 0;
            // Break out of the switch statement
            break;
        }
        else if (armCommand == CMD_FIRE_TORPEDO)
        {
            // Disarm the markers and arm the torpedo
            printf("Disarm torpedo launcher\n");
            armCommand = 0;
        }
        // Arm the torpedos
        printf("Arm marker dropper\n");
        armCommand = CMD_DROP_MARKER;
        break;
    }

    case BTN_FIRE:
    {
        if (armCommand)
        {
            printf("Fire armed payload\n");
            sendCmd(fd, armCommand, 0);
            armCommand = 0;
        }
        else
        {
            printf("No payload armed\n");
        }
        break;
    }
    }
}

void processButtonReleased(int fd, int btn)
{
    switch (btn)
    {
    case BTN_ARM_TORPEDO:
        printf("Disarm torpedo launchers\n");
        armCommand = 0;
        break;
    case BTN_ARM_MARKER:
        printf("Disarm marker dropper\n");
        armCommand = 0;
        break;
    default:
        break;
    }
}

//////////// Common Event handler class ////////
class EventHandler : public KeyListener, public MouseListener, public JoyStickListener
{
public:
    EventHandler() {}
    ~EventHandler() {}
    bool keyPressed( const KeyEvent &arg ) {
        return true;
    }

    bool keyReleased( const KeyEvent &arg ) {
        return true;
    }

    bool mouseMoved( const MouseEvent &arg ) {
        return true;
    }

    bool mousePressed( const MouseEvent &arg, MouseButtonID id ) {
        return true;
    }

    bool mouseReleased( const MouseEvent &arg, MouseButtonID id ) {
        return true;
    }

    bool buttonPressed( const JoyStickEvent &arg, int button )
    {
        std::cout << "* Joy ButtonPressed: " << button<<"\n";

        processButtonPress(sockfd, button);
        return true;
    }

    bool buttonReleased( const JoyStickEvent &arg, int button ) {
        //       std::cout << "* Joy ButtonReleased: " << button<<"\n";
        //   sendEvent(sockfd, TYPE_BTN, button, 0);
        processButtonReleased(sockfd, button);
        return true;
    }

    bool axisMoved( const JoyStickEvent &arg, int axis )
    {
//Provide a little dead zone
//if( arg.state.mAxes[axis].abs > 2500 || arg.state.mAxes[axis].abs < -2500 )
//std::cout << "Joy Axis #: " << axis << " Value: " << arg.state.mAxes[axis].abs<<"\n";
        processAxis(sockfd, axis, arg.state.mAxes[axis].abs);
        return true;
    }

    bool povMoved( const JoyStickEvent &arg, int pov )
    {
        return true;
    }
};

//Create a global instance
EventHandler handler;


int main(int argc, char ** argv)
{
    int yawTime = 0;

    printf(INFO_MESSAGE);

    if(argc < 2)
    {
        printf("\nNeed hostname\n");
        return -1;
    }


    /* Blatant copy and paste job from 417 */
    int numbytes;
    struct hostent *he;



    if ((he=gethostbyname(argv[1])) == NULL)
    {  /* get the host info */
        herror("gethostbyname");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket");
        exit(1);
    }

    memset((void *)&their_addr, 0, sizeof(their_addr));/*zero*/
    their_addr.sin_family = AF_INET;    /* host byte order */
    their_addr.sin_port = htons(PORT);  /* short, network byte order */
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);


    printf("\nSocket FD is %d\n", sockfd);

    std::cout << "\n\n*** OIS Console Demo App is starting up... *** \n";
    try
    {
        doStartup();
        std::cout << "\nStartup done... Hit 'q' or ESC to exit (or joy button 1)\n\n";

        while(appRunning)
        {
//Throttle down CPU usage
#if defined OIS_WIN32_PLATFORM
            Sleep( 30 );
            MSG  msg;
            while( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
            {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
#elif defined OIS_LINUX_PLATFORM
            checkX11Events();
            usleep( 10 * 1000 );
            sendCmd(sockfd, CMD_NOTHING, 0);
            sendCmd(sockfd, CMD_ANGLEYAW, yawCmd);
            sendCmd(sockfd, CMD_ANGLEPITCH, pitchCmd);
            sendCmd(sockfd, CMD_ANGLEROLL, rollCmd);
//printf("Sending: %d\t%d\t%d\n", yawCmd, pitchCmd, rollCmd);
#endif
/*
  if( g_kb )
  {
  g_kb->capture();
  if( !g_kb->buffered() )
  handleNonBufferedKeys();
  }

  if( g_m )
  {
  g_m->capture();
  if( !g_m->buffered() )
  handleNonBufferedMouse();
  }*/

            for( int i = 0; i < 4 ; ++i )
            {
                if( g_joys[i] )
                {
                    g_joys[i]->capture();
                    if( !g_joys[i]->buffered() )
                        handleNonBufferedJoy( g_joys[i] );
                }
            }
        }
    }
    catch( const Exception &ex )
    {
#if defined OIS_WIN32_PLATFORM
        MessageBox( NULL, ex.eText, "An exception has occured!", MB_OK |
                    MB_ICONERROR | MB_TASKMODAL);
#else
        std::cout << "\nOIS Exception Caught!\n" << "\t" << ex.eText << "[Line "
                  << ex.eLine << " in " << ex.eFile << "]\nExiting App";
#endif
    }

    if( g_InputManager )
    {
        g_InputManager->destroyInputObject( g_kb );
        g_InputManager->destroyInputObject( g_m );

        for(int i = 0; i < 4; ++i)
            g_InputManager->destroyInputObject( g_joys[i] );

        InputManager::destroyInputSystem(g_InputManager);
    }

#if defined OIS_LINUX_PLATFORM
// Be nice to X and clean up the x window
    XDestroyWindow(xDisp, xWin);
    XCloseDisplay(xDisp);
#endif

    std::cout << "\n\nGoodbye\n\n";
    return 0;
}

void doStartup()
{
    ParamList pl;

#if defined OIS_WIN32_PLATFORM
//Create a capture window for Input Grabbing
    hWnd = CreateDialog( 0, MAKEINTRESOURCE(IDD_DIALOG1), 0,(DLGPROC)DlgProc);
    if( hWnd == NULL )
        OIS_EXCEPT(E_General, "Failed to create Win32 Window Dialog!");

    ShowWindow(hWnd, SW_SHOW);

    std::ostringstream wnd;
    wnd << (size_t)hWnd;

    pl.insert(std::make_pair( std::string("WINDOW"), wnd.str() ));

//Default mode is foreground exclusive..but, we want to show mouse - so nonexclusive
    pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" )));
    pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE")));
#elif defined OIS_LINUX_PLATFORM

//Connects to default X window
    if( !(xDisp = XOpenDisplay(0)) )
        OIS_EXCEPT(E_General, "Error opening X!");
//Create a window
    xWin = XCreateSimpleWindow(xDisp,DefaultRootWindow(xDisp), 0,0, 100,100, 0, 0, 0);
//bind our connection to that window
    XMapWindow(xDisp, xWin);
//Select what events we want to listen to locally
    XSelectInput(xDisp, xWin, StructureNotifyMask);
    XEvent evtent;
    do
    {
        XNextEvent(xDisp, &evtent);
    } while(evtent.type != MapNotify);

    std::ostringstream wnd;
    wnd << xWin;

    pl.insert(std::make_pair(std::string("WINDOW"), wnd.str()));

//For this demo, show mouse and do not grab (confine to window)
//pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
//pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("false")));
#endif

//This never returns null.. it will raise an exception on errors
    g_InputManager = InputManager::createInputSystem(pl);
/*
  unsigned int v = g_InputManager->getVersionNumber();
  std::cout << "OIS Version: " << (v>>16 ) << "." << ((v>>8) & 0x000000FF) << "." << (v & 0x000000FF)
  << "\n\tRelease Name: " << g_InputManager->getVersionName()
  << "\n\tPlatform: " << g_InputManager->inputSystemName()
  << "\n\tNumber of Mice: " << g_InputManager->numMice()
  << "\n\tNumber of Keyboards: " << g_InputManager->numKeyboards()
  << "\n\tNumber of Joys/Pads = " << g_InputManager->numJoySticks();*/
/*
  if( g_InputManager->numKeyboards() > 0 )
  {
  g_kb = (Keyboard*)g_InputManager->createInputObject( OISKeyboard, true );
  std::cout << "\n\nCreated buffered keyboard";
  g_kb->setEventCallback( &handler );
  }

  if( g_InputManager->numMice() > 0 )
  {
  g_m = (Mouse*)g_InputManager->createInputObject( OISMouse, true );
  std::cout << "\nCreated buffered mouse";
  g_m->setEventCallback( &handler );

  const MouseState &ms = g_m->getMouseState();
  ms.width = 100;
  ms.height = 100;
  }
*/
//This demo only uses at max 4 joys
    int numSticks = g_InputManager->numJoySticks();
    if( numSticks > 4 )numSticks = 4;

    std::cout << "Found " << numSticks  << " joysticks" << std::endl;

    if (numSticks == 0)
        std::cout << "Warning" << std::endl;

    for( int i = 0; i < numSticks; ++i )
    {
        g_joys[i] = (JoyStick*)g_InputManager->createInputObject( OISJoyStick, true );
        g_joys[i]->setEventCallback( &handler );

        g_ff[i] = (ForceFeedback*)g_joys[i]->queryInterface( Interface::ForceFeedback );
        if( g_ff[i] )
        {
            std::cout << "\nCreated buffered joystick with ForceFeedback support.\n";
//Dump out all the supported effects:
            const ForceFeedback::SupportedEffectList &list = g_ff[i]->getSupportedEffects();
            ForceFeedback::SupportedEffectList::const_iterator i = list.begin(),
                e = list.end();
            for( ; i != e; ++i)
                std::cout << "Force =  " << i->first << " Type = " << i->second << std::endl;
        }
        else
            std::cout << "\nCreated buffered joystick. **without** FF support";
    }
}

void handleNonBufferedKeys()
{
    if( g_kb->isKeyDown( KC_ESCAPE ) || g_kb->isKeyDown( KC_Q ) )
        appRunning = false;

    if( g_kb->isModifierDown(Keyboard::Shift) )
        std::cout << "Shift is down..\n";
    if( g_kb->isModifierDown(Keyboard::Alt) )
        std::cout << "Alt is down..\n";
    if( g_kb->isModifierDown(Keyboard::Ctrl) )
        std::cout << "Ctrl is down..\n";
}

void handleNonBufferedMouse()
{
//Just dump the current mouse state
    const MouseState &ms = g_m->getMouseState();
    std::cout << "\nMouse: Abs(" << ms.X.abs << " " << ms.Y.abs << " " << ms.Z.abs
              << ") B: " << ms.buttons << " Rel(" << ms.X.rel << " " << ms.Y.rel << " " << ms.Z.rel << ")";
}

void handleNonBufferedJoy( JoyStick* js )
{
//Just dump the current joy state
    const JoyStickState &joy = js->getJoyStickState();
    std::cout << "\nJoyStick: button bits: " << joy.buttons;
    for( int i = 0; i < joy.mAxes.size(); ++i )
        std::cout << "\nAxis " << i << " X: " << joy.mAxes[i].abs;
}

#if defined OIS_WIN32_PLATFORM
LRESULT DlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    return FALSE;
}
#endif

#if defined OIS_LINUX_PLATFORM
//This is just here to show that you still recieve x11 events, as the lib only needs mouse/key events
void checkX11Events()
{
    XEvent event;

//Poll x11 for events (keyboard and mouse events are caught here)
    while( XPending(xDisp) > 0 )
    {
        XNextEvent(xDisp, &event);
//Handle Resize events
        if( event.type == ConfigureNotify )
        {
            if( g_m )
            {
                const MouseState &ms = g_m->getMouseState();
                ms.width = event.xconfigure.width;
                ms.height = event.xconfigure.height;
            }
        }
        else if( event.type == DestroyNotify )
        {
            std::cout << "Exiting...\n";
            appRunning = false;
        }
        else
            std::cout << "\nUnknown X Event: " << event.type << std::endl;
    }
}
#endif
