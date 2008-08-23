#if defined(__APPLE__) || defined(MACOSX)
	#include <GL/glew.h>
	#include <GLUT/glut.h>
	#include <OpenGL/gl.h>
#else
	#include <GL/glew.h>
	#include <GL/glut.h>
	#include <GL/gl.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "Shader.h"

void cleanup(int program, int vertexShader, int pixelShader)
{
	glDetachShader(program, vertexShader);
	glDetachShader(program, pixelShader);
	glDeleteShader(vertexShader); 
	glDeleteShader(pixelShader); 
	glDeleteProgram(program);
}

Shader* Shader::currentShader = NULL;

Shader::Shader() : vertexShader(0), pixelShader(0), program(0), compiled(false), errors("")
{
}

Shader::~Shader()
{
	if (compiled)
		cleanup(program, vertexShader, pixelShader);
}

bool Shader::loadShaderString(std::string vertex, std::string pixel)
{
	if (compiled)
		cleanup(program, vertexShader, pixelShader);
	
	compiled = false;
	errors = "";

	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	pixelShader = glCreateShader(GL_FRAGMENT_SHADER);

	const char* vs = vertex.c_str();
	const char* ps = pixel.c_str();

	glShaderSource(vertexShader, 1, &vs, NULL);
	glShaderSource(pixelShader, 1, &ps, NULL);

	glCompileShader(vertexShader);
	glCompileShader(pixelShader);

	GLint status;
	char log[2048];

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status); 
	if (status == GL_FALSE)
	{
		glGetShaderInfoLog(vertexShader, 2048, &status, log);
		errors += log;	errors += "\n";
	}
	glGetShaderiv(pixelShader, GL_COMPILE_STATUS, &status); 
	if (status == GL_FALSE)
	{
		glGetShaderInfoLog(pixelShader, 2048, &status, log);
		errors += log;	errors += "\n";
	}

	program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, pixelShader);

	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &status); 
	if (status == GL_FALSE)
	{
		glGetProgramInfoLog(vertexShader, 2048, &status, log);
		errors += log;	errors += "\n";
	}

	if (errors.length() == 0)
		compiled = true;
	return compiled;
}

bool Shader::loadShaderString(std::string shader)
{
	size_t vp = shader.find("[vertex]");
	size_t fp = shader.find("[pixel]");

	if (vp == std::string::npos || fp == std::string::npos)
	{
		if (compiled)
			cleanup(program, vertexShader, pixelShader);
		compiled = false;
		errors = "[vertex] / [pixel] could not be found in the source string";
		return false;
	}

	std::string vertex, pixel;
	if (vp < fp)
	{
		vertex = shader.substr(vp+8, fp-8);
		pixel = shader.substr(fp+8);
	}
	else
	{
		vertex = shader.substr(vp+8);
		pixel = shader.substr(fp+8, vp-8);
	}

	return loadShaderString(vertex, pixel);
}

bool Shader::loadShaderFile(std::string vertex, std::string pixel)
{
	FILE *fp;
	char *content = NULL;
	int count = 0;

	fp = fopen(vertex.c_str(),"rt");
	if (fp == NULL)
	{
		if (compiled)
			cleanup(program, vertexShader, pixelShader);
		compiled = false;
		errors = "could not open source file: " + vertex;
		return false;
	}
	else
	{
		fseek(fp, 0, SEEK_END);
		count = ftell(fp);
		rewind(fp);
		if (count > 0) 
		{
			content = new char[sizeof(char) * (count+1)];
			count = (int)fread(content,sizeof(char),count,fp);
			content[count] = '\0';
		}
		fclose(fp);

		vertex = content;
		delete[] content;
	}

	fp = fopen(pixel.c_str(),"rt");
	if (fp == NULL)
	{
		if (compiled)
			cleanup(program, vertexShader, pixelShader);
		compiled = false;
		errors = "could not open source file: " + pixel;
		return false;
	}
	else
	{
		fseek(fp, 0, SEEK_END);
		count = ftell(fp);
		rewind(fp);
		if (count > 0) 
		{
			content = new char[sizeof(char) * (count+1)];
			count = (int)fread(content,sizeof(char),count,fp);
			content[count] = '\0';
		}
		fclose(fp);

		pixel = content;
		delete[] content;
	}

	return loadShaderString(vertex, pixel);
}

bool Shader::loadShaderFile(std::string shader)
{
	FILE *fp;
	char *content = NULL;
	int count = 0;

	fp = fopen(shader.c_str(),"rt");
	if (fp == NULL)
	{
		if (compiled)
			cleanup(program, vertexShader, pixelShader);
		compiled = false;
		errors = "could not open source file: " + shader;
		return false;
	}
	else
	{
		fseek(fp, 0, SEEK_END);
		count = ftell(fp);
		rewind(fp);
		if (count > 0) 
		{
			content = new char[sizeof(char) * (count+1)];
			count = (int)fread(content,sizeof(char),count,fp);
			content[count] = '\0';
		}
		fclose(fp);

		shader = content;
		delete[] content;
	}

	return loadShaderString(shader);
}

void Shader::setUniform(std::string name, float v0)
{
	glUniform1f(glGetUniformLocation(program, name.c_str()), v0); 
}

void Shader::setUniform(std::string name, float v0, float v1)
{
	glUniform2f(glGetUniformLocation(program, name.c_str()), v0, v1); 
}

void Shader::setUniform(std::string name, float v0, float v1, float v2)
{
	glUniform3f(glGetUniformLocation(program, name.c_str()), v0, v1, v2); 
}

void Shader::setUniform(std::string name, float v0, float v1, float v2, float v3)
{
	glUniform4f(glGetUniformLocation(program, name.c_str()), v0, v1, v2, v3); 
}

void Shader::setUniform(std::string name, int v0)
{
	glUniform1i(glGetUniformLocation(program, name.c_str()), v0); 
}

void Shader::setUniform(std::string name, int v0, int v1)
{
	glUniform2i(glGetUniformLocation(program, name.c_str()), v0, v1);
}

void Shader::setUniform(std::string name, int v0, int v1, int v2)
{
	glUniform3i(glGetUniformLocation(program, name.c_str()), v0, v1, v2);
}

void Shader::setUniform(std::string name, int v0, int v1, int v2, int v3)
{
	glUniform4i(glGetUniformLocation(program, name.c_str()), v0, v1, v2, v3);
}

void Shader::setUniform(std::string name, const Vec2& v)
{
	glUniform2f(glGetUniformLocation(program, name.c_str()), v.x, v.y);
}

void Shader::setUniform(std::string name, const Vec3& v)
{
	glUniform3f(glGetUniformLocation(program, name.c_str()), v.x, v.y, v.z);
}

void Shader::setUniform(std::string name, const Vec4& v)
{
	glUniform4f(glGetUniformLocation(program, name.c_str()), v.x, v.y, v.z, v.w);
}

/*
void Shader::setUniform(std::string name, const Mat2& v);
void Shader::setUniform(std::string name, const Mat3& v);
void Shader::setUniform(std::string name, const Mat4& v);
*/

void Shader::bind()
{
	if (Shader::currentShader != this && compiled)
	{
		glUseProgram(program);
		Shader::currentShader = this;
	}
}

bool Shader::supportsShaders()
{
    return false;
	// TODO fix
	const GLubyte* extensions = glGetString(GL_EXTENSIONS);
	if (gluCheckExtension((const GLubyte*)"GL_ARB_shading_language_100", extensions) == GL_TRUE)
		return true;
	//if ( glewIsSupported("GL_VERSION_2_0"))
	//	return true;
	return false;
}