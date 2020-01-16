#pragma once

#include "shader.h"

struct ShaderProgram {
public:
	Shader shader;

	ShaderProgram() {};
	ShaderProgram(const ShaderProgram&) = delete;
	~ShaderProgram();

	template <class ...Args>
	ShaderProgram(ShaderSource shaderSource, const Args&... args) : shader(shaderSource) {
		std::vector<std::string> uniforms = { args... };

		Log::subject s(shader.name);

		for (const std::string& uniform : uniforms)
			shader.createUniform(uniform);
	
	}

	void bind();
	void close();

};