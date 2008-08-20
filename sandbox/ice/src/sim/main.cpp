#include <GL/glut.h>


// Callback prototypes
void reshape(int width, int height);
void disp();
void keyb(unsigned char key, int x, int y);


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
    int win = glutCreateWindow("Robotics@Maryland Vehicle Simulator");
    
    // Set callbacks
    glutDisplayFunc(disp);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyb);
    
    // Define color we use to clear the screen
    glClearColor(0.0, 0.0, 0.0, 0.0);
    
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
}
