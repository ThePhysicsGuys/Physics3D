#pragma once

#include "shader.h"

struct ShaderProgram {
public:
	Shader shader;
	ShaderSource shaderSource;

	ShaderProgram() {};
	ShaderProgram(const ShaderProgram&) = delete;
	~ShaderProgram();

	template <class ...Args>
	ShaderProgram(ShaderSource shaderSource, const Args&... args) : shaderSource(shaderSource), shader(shaderSource) {
		std::vector<std::string> uniforms = { args... };

		Log::subject s(shader.name);

		for (const std::string& uniform : uniforms)
			shader.createUniform(uniform);
	
	}

	void bind();
	void close();

};