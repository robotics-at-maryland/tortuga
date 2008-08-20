#ifndef SHADER_H
#define SHADER_H

#include <string>

class Shader
{
public:

	Shader();
	~Shader();

	bool loadShaderString(std::string vertex, std::string pixel);
	bool loadShaderString(std::string shader);

	bool loadShaderFile(std::string vertex, std::string pixel);
	bool loadShaderFile(std::string shader);

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

	bool compiled;
	std::string errors;

	static Shader* currentShader;

};

#endif