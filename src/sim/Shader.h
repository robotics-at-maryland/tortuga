#ifndef SHADER_H
#define SHADER_H

#include <string>
#include "Sim.h"

class Shader
{
public:

	Shader();
	~Shader();

	bool loadShaderString(std::string vertex, std::string pixel);
	bool loadShaderString(std::string shader);

	bool loadShaderFile(std::string vertex, std::string pixel);
	bool loadShaderFile(std::string shader);

	void setUniform(std::string name, float v0);
	void setUniform(std::string name, float v0, float v1);
	void setUniform(std::string name, float v0, float v1, float v2);
	void setUniform(std::string name, float v0, float v1, float v2, float v3);
	void setUniform(std::string name, float *v, int count);

	void setUniform(std::string name, int v0);
	void setUniform(std::string name, int v0, int v1);
	void setUniform(std::string name, int v0, int v1, int v2);
	void setUniform(std::string name, int v0, int v1, int v2, int v3);
	void setUniform(std::string name, int *v, int count);

	void setUniform(std::string name, const Vec2& v);
	void setUniform(std::string name, const Vec3& v);
	void setUniform(std::string name, const Vec4& v);

	void setUniform(std::string name, const Mat2& v);
	void setUniform(std::string name, const Mat3& v);
	void setUniform(std::string name, const Mat4& v);

	void bind();

	bool hasErrors() const
	{	return errors.length() > 0;	}

	std::string getErrors() const
	{	return errors;	}

	bool getCompiled() const
	{	return compiled;	}


	static Shader* getCurrentShader();

	static bool supportsShaders();

private:

	unsigned int vertexShader;
	unsigned int pixelShader;
	unsigned int program;

	bool compiled;
	std::string errors;

	static Shader* currentShader;

};

#endif