#pragma once

#include "shader.h"
#include "material.h"
#include "texture.h"
#include "light.h"

#include "../debug.h"

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
		Log::setSubject(shader.name);
		Log::debug("Using shader (%s) with id (%d)", shader.name.c_str(), shader.id);
		for (int i = 0; i < count; i++) {
			std::string uniform = std::string(va_arg(args, const char *));
			shader.createUniform(uniform);
			uniforms.push_back(uniform);
		}
		Log::resetSubject();
		va_end(args);
	}

	void bind() {
		shader.bind();
	}

	void close() {
		shader.close();
	}

	~ShaderProgram() {
		close();
	}
};

struct SkyboxShader : public ShaderProgram {
	SkyboxShader() : ShaderProgram() {}
	SkyboxShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, 4, "viewMatrix", "projectionMatrix", "skyboxTexture", "lightDirection") {}

	void update(Mat4f viewMatrix, Mat4f projectionMatrix) {
		bind();
		shader.setUniform(uniforms[0], viewMatrix);
		shader.setUniform(uniforms[1], projectionMatrix);
	}

	void update(CubeMap skybox) {
		bind();
		shader.setUniform(uniforms[2], skybox.unit);
	}

	void update(Vec3f lightDirection) {
		bind();
		shader.setUniform(uniforms[3], lightDirection);
	}
};

struct BasicShader : public ShaderProgram {
	BasicShader() : ShaderProgram() {}
	BasicShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, 8, "modelMatrix", "viewMatrix", "projectionMatrix", "viewPosition", "material.ambient", "material.diffuse", "material.specular", "material.reflectance") {}

	void createLightArray(int size) {
		bind();
		for (int i = 0; i < size; i++) {
			char buffer[64];
			// position
			snprintf(buffer, sizeof(buffer), "lights[%d].position", i);
			shader.createUniform(buffer);

			// color
			snprintf(buffer, sizeof(buffer), "lights[%d].color", i);
			shader.createUniform(buffer);

			// intensity
			snprintf(buffer, sizeof(buffer), "lights[%d].intensity", i);
			shader.createUniform(buffer);

			// attenuation.constant
			snprintf(buffer, sizeof(buffer), "lights[%d].attenuation.constant", i);
			shader.createUniform(buffer);

			// attenuation.linear
			snprintf(buffer, sizeof(buffer), "lights[%d].attenuation.linear", i);
			shader.createUniform(buffer);

			// attenuation.exponent
			snprintf(buffer, sizeof(buffer), "lights[%d].attenuation.exponent", i);
			shader.createUniform(buffer);
		}
	}

	void update(Mat4f viewMatrix, Mat4f projectionMatrix, Vec3f viewPosition) {
		bind();
		shader.setUniform(uniforms[1], viewMatrix);
		shader.setUniform(uniforms[2], projectionMatrix);
		shader.setUniform(uniforms[3], viewPosition);
	}

	void updateLight(Light lights[], int size) {
		bind();
		for (int i = 0; i < size; i++) {
			char buffer[64];
			// position
			snprintf(buffer, sizeof(buffer), "lights[%d].position", i);
			glUniform3f(glGetUniformLocation(shader.id, buffer), lights[i].position.x, lights[i].position.y, lights[i].position.z);

			// color
			snprintf(buffer, sizeof(buffer), "lights[%d].color", i);
			glUniform3f(glGetUniformLocation(shader.id, buffer), lights[i].color.x, lights[i].color.y, lights[i].color.z);

			// intensity
			snprintf(buffer, sizeof(buffer), "lights[%d].intensity", i);
			glUniform1f(glGetUniformLocation(shader.id, buffer), lights[i].intensity);

			// attenuation.constant
			snprintf(buffer, sizeof(buffer), "lights[%d].attenuation.constant", i);
			glUniform1f(glGetUniformLocation(shader.id, buffer), lights[i].attenuation.constant);

			// attenuation.linear
			snprintf(buffer, sizeof(buffer), "lights[%d].attenuation.linear", i);
			glUniform1f(glGetUniformLocation(shader.id, buffer), lights[i].attenuation.linear);

			// attenuation.exponent
			snprintf(buffer, sizeof(buffer), "lights[%d].attenuation.exponent", i);
			glUniform1f(glGetUniformLocation(shader.id, buffer), lights[i].attenuation.exponent);
		}
	}

	void updateModel(Mat4f modelMatrix) {
		bind();
		shader.setUniform(uniforms[0], modelMatrix);
	}

	void updateMaterial(Material material) {
		bind();
		shader.setUniform(uniforms[4], material.ambient);
		shader.setUniform(uniforms[5], material.diffuse);
		shader.setUniform(uniforms[6], material.specular);
		shader.setUniform(uniforms[7], material.reflectance);
	}
};

struct BasicNormalShader : public ShaderProgram {
	BasicNormalShader() : ShaderProgram() {}
	BasicNormalShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, 4, "modelMatrix", "viewMatrix", "projectionMatrix", "viewPosition") {}

	void update(Mat4f viewMatrix, Mat4f projectionMatrix, Vec3f viewPosition) {
		bind();
		shader.setUniform(uniforms[1], viewMatrix);
		shader.setUniform(uniforms[2], projectionMatrix);
		shader.setUniform(uniforms[3], viewPosition);
	}

	void updateModel(Mat4f modelMatrix) {
		bind();
		shader.setUniform(uniforms[0], modelMatrix);
	}
};

struct QuadShader : public ShaderProgram {
	QuadShader() : ShaderProgram() {}
	QuadShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, 1, "textureSampler") {}

	void update(Texture texture) {
		bind();
		shader.setUniform(uniforms[0], texture.unit);
	}
};

struct PostProcessShader : public ShaderProgram {
	PostProcessShader() : ShaderProgram() {}
	PostProcessShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, 1, "textureSampler") {}

	void update(Texture texture) {
		bind();
		shader.setUniform(uniforms[0], texture.unit);
	}
};

struct OriginShader : public ShaderProgram {
	OriginShader() : ShaderProgram() {}
	OriginShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, 5, "viewMatrix", "rotatedViewMatrix", "projectionMatrix", "orthoMatrix", "viewPosition") {}

	void update(Mat4f viewMatrix, Mat4f rotatedViewMatrix, Mat4f projectionMatrix, Mat4f orthoMatrix, Vec3f viewPosition) {
		bind();
		shader.setUniform(uniforms[0], viewMatrix);
		shader.setUniform(uniforms[1], rotatedViewMatrix);
		shader.setUniform(uniforms[2], projectionMatrix);
		shader.setUniform(uniforms[3], orthoMatrix);
		shader.setUniform(uniforms[4], viewPosition);
	}
};

struct FontShader : public ShaderProgram {
	FontShader() : ShaderProgram() {}
	FontShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, 3, "projectionMatrix", "color", "text") {}

	void updateColor(Vec3f color) {
		bind();
		shader.setUniform(uniforms[1], color);
	}

	void update(Mat4f projectionMatrix) {
		bind();
		shader.setUniform(uniforms[0], projectionMatrix);
	}

	void updateTexture(Texture texture) {
		bind();
		shader.setUniform(uniforms[2], texture.unit);
	}
};

struct VectorShader : public ShaderProgram {
	VectorShader() : ShaderProgram() {}
	VectorShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, 3, "viewMatrix", "projectionMatrix", "viewPosition") {}

	void update(Mat4f viewMatrix, Mat4f projectionMatrix, Vec3f viewPosition) {
		bind();
		shader.setUniform(uniforms[0], viewMatrix);
		shader.setUniform(uniforms[1], projectionMatrix);
		shader.setUniform(uniforms[2], viewPosition);
	}
};