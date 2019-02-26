#pragma once

#include "shader.h"
#include "../../util/log.h"

#include <cstdarg>
#include <vector>

struct ShaderProgram {
public:
	std::vector<std::string> uniforms;
	Shader shader;
	ShaderSource shaderSource;

	ShaderProgram() {};
	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram(ShaderSource shaderSource, int count, ...) : shaderSource(shaderSource), shader(shaderSource) {

		va_list args;
		va_start(args, count);

		Log::debug("Using shader (%s) with id (%d)", shader.name.c_str(), shader.getId());
		for (int i = 0; i < count; i++) {
			std::string uniform = std::string(va_arg(args, const char *));
			Log::debug("init '%s'", uniform.c_str());
			shader.createUniform(uniform);
			uniforms.push_back(uniform);
		}

		va_end(args);
	}

	void close() {
		shader.close();
	}

	~ShaderProgram() {
		close();
	}
};

struct BasicShader : public ShaderProgram {
	BasicShader() : ShaderProgram() {}
	BasicShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, 5, "modelMatrix", "viewMatrix", "projectionMatrix", "viewPosition", "color") {}

	void update(Mat4f viewMatrix, Mat4f projectionMatrix, Vec3f viewPosition) {
		shader.bind();
		shader.setUniform(uniforms[1].c_str(), viewMatrix);
		shader.setUniform(uniforms[2].c_str(), projectionMatrix);
		shader.setUniform(uniforms[3].c_str(), viewPosition);
	}

	void updateColor(Vec3f color) {
		shader.bind();
		shader.setUniform(uniforms[4].c_str(), color);
	}

	void updateModel(Mat4f modelMatrix) {
		shader.bind();
		shader.setUniform(uniforms[0].c_str(), modelMatrix);
	}
};

struct OriginShader : public ShaderProgram {
	OriginShader() : ShaderProgram() {}
	OriginShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, 5, "viewMatrix", "rotatedViewMatrix", "projectionMatrix", "orthoMatrix", "viewPosition") {}

	void update(Mat4f viewMatrix, Mat4f rotatedViewMatrix, Mat4f projectionMatrix, Mat4f orthoMatrix, Vec3f viewPosition) {
		shader.bind();
		shader.setUniform(uniforms[0].c_str(), viewMatrix);
		shader.setUniform(uniforms[1].c_str(), rotatedViewMatrix);
		shader.setUniform(uniforms[2].c_str(), projectionMatrix);
		shader.setUniform(uniforms[3].c_str(), orthoMatrix);
		shader.setUniform(uniforms[4].c_str(), viewPosition);
	}
};

struct VectorShader : public ShaderProgram {
	VectorShader() : ShaderProgram() {}
	VectorShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, 3, "viewMatrix", "projectionMatrix", "viewPosition") {}

	void update(Mat4f viewMatrix, Mat4f projectionMatrix, Vec3f viewPosition) {
		shader.bind();
		shader.setUniform(uniforms[0].c_str(), viewMatrix);
		shader.setUniform(uniforms[1].c_str(), projectionMatrix);
		shader.setUniform(uniforms[2].c_str(), viewPosition);
	}
};

 

