#if defined(__APPLE__) || defined(MACOSX)
	#include <GL/glew.h>
	#include <GLUT/glut.h>
	#include <OpenGL/gl.h>
#else
	#include <GL/glew.h>
	#include <GL/glut.h>
	#include <GL/gl.h>
#endif

#include <string>
#include "Shader.h"

Shader* Shader::currentShader = NULL;

Shader::Shader() : vertexShader(0), pixelShader(0), compiled(false), errors("")
{
}

Shader::~Shader()
{
	if (compiled)
	{
		//cleanup
	}
}
/*

bool Shader::loadShaderString(std::string vertex, std::string pixel)
{
	if (compiled)
	{
		//cleanup
	}
	glCreateShader(GL_VERTEX_SHADER);
}

bool Shader::loadShaderString(std::string shader);

bool Shader::loadShaderFile(std::string vertex, std::string pixel);
bool Shader::loadShaderFile(std::string shader);
*/
bool Shader::supportsShaders()
{
	if ( glewIsSupported("GL_VERSION_2_0"))
		return true;
	return false;
}