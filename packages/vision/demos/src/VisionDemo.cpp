

// STD Includes
#include <iostream>
#include <string>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

// Library Includes
#include <highgui.h>
#include <openGL/openGL.h>
#include <GLUT/glut.h>

// Project Includes
#include "vision/include/DetectorTest.h"
#include "vision/demos/include/VisionDemo.h"

using namespace std;

namespace ram{
namespace vision{

void VisionDemo::startup(int cameraOrMovie)
{
	if (0)
	{
		forward=new ram::vision::DetectorTest(0,true);
		forward->background(25);
	}
	else
	{
		forward=new ram::vision::DetectorTest("/Applications/underwater.avi");
		forward->background(10);
	}
}

void VisionDemo::setOperation(int operation)
{
	cout<<"Setting Operation "<<operation<<endl;
	
	if (forward==NULL)
	{
		cout<<"no camera set"<<endl;
		return;
	}	
	if (operation==0)
	{
		forward->lightDetectOff();
		forward->orangeDetectOff();
		forward->binDetectOff();
		forward->gateDetectOff();
	}
	else if (operation==1)//Light
	{
		forward->lightDetectOn();
		forward->orangeDetectOff();
		forward->binDetectOff();
		forward->gateDetectOff();
	}
	else if (operation==2)//Bin
	{
		forward->binDetectOn();
		forward->orangeDetectOff();
		forward->lightDetectOff();
		forward->gateDetectOff();
	}
	else if (operation==3)//Gate
	{
		forward->gateDetectOn();
		forward->orangeDetectOff();
		forward->lightDetectOff();
		forward->binDetectOff();
	}
	else if (operation==4)//Orange
	{
		forward->orangeDetectOn();
		forward->lightDetectOff();
		forward->binDetectOff();
		forward->gateDetectOff();
	}
}	
}
}

namespace Rendering
{
ram::vision::VisionDemo* v=NULL;

GLfloat g_fViewDistance = 9;
GLfloat g_nearPlane = 1;
GLfloat g_farPlane = 1000;
int g_Width = 640;                         
int g_Height = 480;   

bool flipped = false;
bool flipping = false;
int flipStatus = 0;
int howFar = 10;

float quadZ = 0;
float quadRot = 0;

const int numTextures = 2;
GLuint textures[numTextures];

int animation = 0;

int imageWidth = 1024;
int imageHeight = 1024;
unsigned char image[1024*1024*3];

float rotateAmount = 180;

bool useFlipTex = false;

void createTexture(void* data, int w, int h, int texNum)
{
//	Rendering::image[9]='\0';
//	printf("%s",Rendering::image);
	if (useFlipTex)
	{
		texNum = 1;
	}
	else
	{
		texNum = 0;
	}
	glBindTexture(GL_TEXTURE_2D, textures[texNum]);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
}

void fillWithChecker(int texNum, int t = 4)
{
	const int w = 512, h = 512;
	unsigned char data[w][h][3];
	unsigned char c;

	for (int i=0;i<w;i++)
	{
		for (int j=0;j<h;j++)
		{
			c = i^j;
			c *= t;

			data[i][j][0] = c;
			data[i][j][1] = c;
			data[i][j][2] = c;
		}
	}

	createTexture(&data, w, h, texNum);
}

void flip()
{
	if (!flipping)
		return;
	
	flipStatus += 5;

	if (flipStatus == 100)
	{
		useFlipTex = !useFlipTex;
		flipped = !flipped;
		}

	if (flipStatus < 100.0f)
		quadZ = flipStatus/100.0f;
	else
		quadZ = 1 - (flipStatus-100)/100.0f;

	quadRot = 180 * flipStatus/200.0f;

	if (flipStatus > 200.0f)
	{
		flipStatus = 0;
		flipping = false;
		quadRot = 0;
	}
}

void drawQuad(float fSize)
{
	if (flipStatus >= 100)
		glScalef(-1,1,1);
	  fSize /= 2.0;
	  glBegin(GL_QUADS);

		glVertex3f(-fSize, -fSize, 0);    glTexCoord2f (0, 0);
		glVertex3f(fSize, -fSize, 0);     glTexCoord2f (1, 0);
		glVertex3f(fSize, fSize, 0);      glTexCoord2f (1, 1);
		glVertex3f(-fSize, fSize, 0);     glTexCoord2f (0, 1);

	  glEnd();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(0, 0, -g_fViewDistance, 0, 0, -1, 0, 1, 0);

	Rendering::createTexture(Rendering::image, Rendering::imageWidth, Rendering::imageHeight, 0);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	if (!flipped)
	{
		glColor3f(1,1,1);
		glBindTexture(GL_TEXTURE_2D, textures[0]);
	}
	else
	{
		glColor3f(1,1,1);
		glBindTexture(GL_TEXTURE_2D, textures[1]);
	}
	glTranslatef(0,0,quadZ * howFar);
	glRotatef(rotateAmount, 0,0,1);
	glRotatef(quadRot, 0, 1, 0);
	drawQuad(10.0);
	glPushMatrix();
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix(); 
	glPopMatrix();

	glutSwapBuffers();
}

void reshape(int width, int height)
{
	g_Width = width;
    g_Height = height;
    glViewport(0, 0, g_Width, g_Height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65.0, (float)g_Width / g_Height, g_nearPlane, g_farPlane);
    glMatrixMode(GL_MODELVIEW);
}

void init()
{
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LESS);
   glShadeModel(GL_SMOOTH);

   for (int i=0;i<numTextures;i++)
		glGenTextures(1, &textures[i]);

   //glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_TEXTURE_2D);
}

void run()
{
	flip();
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
  switch (key)
  {
  case 'z':
	  if (!flipping)
	  {
		  flipping = true;
		  flipStatus = 0;
	  }
	break;
	case '0':
	if (!flipping)
	  {
		  flipping = true;
		  flipStatus = 0;
		  Rendering::v->setOperation(0);
		  rotateAmount=90;
	  }
	break;
		case '1':
	if (!flipping)
	  {
		  flipping = true;
		  flipStatus = 0;
		  Rendering::v->setOperation(1);
		  rotateAmount = 180;
	  }
	break;
		case '2':
	if (!flipping)
	  {
		  flipping = true;
		  flipStatus = 0;
		  Rendering::v->setOperation(2);
		  rotateAmount = 180;
	  }
	break;
		case '3':
	if (!flipping)
	  {
		  flipping = true;
		  flipStatus = 0;
		  Rendering::v->setOperation(3);
		  rotateAmount = 180;
	  }
	break;
		case '4':
	if (!flipping)
	  {
		  flipping = true;
		  flipStatus = 0;
		  Rendering::v->setOperation(4);
		  rotateAmount =180;
	  }
	break;
  }
}
 

}

int main(int argc, char** argv)
{
  cout<<"Initializing OpenGL"<<endl;
  Rendering::v=new ram::vision::VisionDemo();
  Rendering::v->startup(1);
  Rendering::v->setOperation(1);
  glutInit (&argc, argv);
  glutInitWindowSize (Rendering::g_Width, Rendering::g_Height);
  glutInitDisplayMode ( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutCreateWindow ("OGL DEMO");
  glutFullScreen();
  Rendering::init();
  glutDisplayFunc (Rendering::display);
  glutReshapeFunc (Rendering::reshape);
  glutKeyboardFunc (Rendering::keyboard);
  glutIdleFunc (Rendering::run);
  glutMainLoop ();
  return 0;
}

namespace ram{
namespace vision{
void dataCopy(unsigned char *myData, int w, int h)
{
	cout<<"Copying Data width,height:"<<w<<","<<h<<endl;
	Rendering::imageWidth = w;
	Rendering::imageHeight = h;
	int count = 0;
	for (int i=0;i<w;i++)
	{
		for (int j=0;j<h;j++)
		{
			Rendering::image[count] = myData[count+2];
			Rendering::image[count+1] = myData[count+1];
			Rendering::image[count+2] = myData[count];
			count += 3;
		}
	}
}

}}
	
	
