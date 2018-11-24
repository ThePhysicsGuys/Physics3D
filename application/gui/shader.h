#pragma once

#include <iostream>

struct ShaderSource {
	std::string vertexSource;
	std::string fragmentSource;
};

ShaderSource parseShader(const std::string& path);
ShaderSource parseShader(const std::string& vertexPath, const std::string& fragmentPath);

class Shader {
public:
	Shader(const std::string& vertexShader, const std::string& fragmentShader);
	Shader(ShaderSource shaderSource) : Shader(shaderSource.vertexSource, shaderSource.fragmentSource) {};
	void bind();
	void unbind();
	void close();
	unsigned int getId();
};
