#pragma once

#include "GL\glew.h"
#include "GLFW\glfw3.h"

#include "shader.h"
#include "material.h"
#include "texture.h"
#include "light.h"

#include "../debug.h"
#include "../util/log.h"

#include "../extendedPart.h"

#include <cstdarg>
#include <vector>


struct ShaderProgram {
public:
	Shader shader;
	ShaderSource shaderSource;

	ShaderProgram() {};
	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram(ShaderSource shaderSource, int count, ...) : shaderSource(shaderSource), shader(shaderSource) {

		va_list args;
		va_start(args, count);
		Log::setSubject(shader.name);
		for (int i = 0; i < count; i++) {
			std::string uniform = std::string(va_arg(args, const char *));
			shader.createUniform(uniform);
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

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix) {
		bind();
		shader.setUniform("viewMatrix", viewMatrix);
		shader.setUniform("projectionMatrix", projectionMatrix);
	}

	void updateCubeMap(const CubeMap& skybox) {
		bind();
		shader.setUniform("skyboxTexture", skybox.unit);
	}

	void updateLightDirection(const Vec3f& lightDirection) {
		bind();
		shader.setUniform("lightDirection", lightDirection);
	}
};

struct LineShader : public ShaderProgram {
	LineShader() : ShaderProgram() {}
	LineShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, 4, "viewMatrix", "projectionMatrix", "modelMatrix", "color") {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix) {
		bind();
		shader.setUniform("viewMatrix", viewMatrix);
		shader.setUniform("projectionMatrix", projectionMatrix);
	}

	void updateModel(const Mat4f& modelMatrix) {
		bind();
		shader.setUniform("modelMatrix", modelMatrix);
	}

	void updateColor(const Vec4f& color) {
		bind();
		shader.setUniform("color", color);
	}
};

struct BasicShader : public ShaderProgram {
	BasicShader() : ShaderProgram() {}
	BasicShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, 19, "modelMatrix", "viewMatrix", "projectionMatrix", "viewPosition", "includeNormals", "includeUvs", "material.ambient", "material.diffuse", "material.specular", "material.reflectance", "material.textured", "material.normalmapped", "textureSampler", "normalSampler", "sunDirection", "sunColor", "gamma", "hdr", "exposure") {}

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

	void updateSunDirection(const Vec3f& sunDirection) {
		bind();
		shader.setUniform("sunDirection", sunDirection);
	}

	void updateSunColor(const Vec3f& sunColor) {
		bind();
		shader.setUniform("sunColor", sunColor);
	}

	void updateGamma(float gamma) {
		bind();
		shader.setUniform("gamma", gamma);
	}

	void updateHDR(bool hdr) {
		bind();
		shader.setUniform("hdr", hdr);
	}

	void updateExposure(float exposure) {
		bind();
		shader.setUniform("exposure", exposure);
	}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Vec3f& viewPosition) {
		bind();
		shader.setUniform("viewMatrix", viewMatrix);
		shader.setUniform("projectionMatrix", projectionMatrix);
		shader.setUniform("viewPosition", viewPosition);
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

	void updatePart(const ExtendedPart& part) {
		bind();
		shader.setUniform("includeNormals", part.visualShape.normals != nullptr);
		shader.setUniform("includeUvs", part.visualShape.uvs != nullptr);
		shader.setUniform("modelMatrix", CFrameToMat4(CFrame(part.cframe)));
	}

	void updateModel(const Mat4f& modelMatrix) {
		bind();
		shader.setUniform("modelMatrix", modelMatrix);
	}

	void updateMaterial(const Material& material) {
		bind();
		shader.setUniform("material.ambient", material.ambient);
		shader.setUniform("material.diffuse", material.diffuse);
		shader.setUniform("material.specular", material.specular);
		shader.setUniform("material.reflectance", material.reflectance);

		if (material.texture) {
			material.texture->bind();
			shader.setUniform("material.textured", true);
			shader.setUniform("textureSampler", material.texture->unit);
		} else {
			shader.setUniform("material.textured", false);
			shader.setUniform("textureSampler", 0);
		}
		
		if (material.normal) {
			material.normal->bind();
			shader.setUniform("material.normalmapped", true);
			shader.setUniform("normalSampler", material.normal->unit);
		} else {
			shader.setUniform("material.normalmapped", false);
			shader.setUniform("normalSampler", 0);
		}

	}
};

struct DepthShader : public ShaderProgram {
	DepthShader() : ShaderProgram() {}
	DepthShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, 2, "modelMatrix", "lightMatrix") {}

	void updateLight(const Mat4f& lightMatrix) {
		bind();
		shader.setUniform("lightMatrix", lightMatrix);
	}

	void updateModel(const Mat4f& modelMatrix) {
		bind();
		shader.setUniform("modelMatrix", modelMatrix);
	}
};

struct QuadShader : public ShaderProgram {
	QuadShader() : ShaderProgram() {}
	QuadShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, 4, "projectionMatrix", "color", "textureSampler", "textured") {}

	void updateProjection(const Mat4f& orthoMatrix) {
		bind();
		shader.setUniform("projectionMatrix", orthoMatrix);
	}

	void updateColor(const Vec4& color) {
		bind();
		shader.setUniform("textured", false);
		shader.setUniform("color", color);
	}

	void updateTexture(Texture* texture) {
		updateTexture(texture, Vec4f(1));
	}

	void updateTexture(Texture* texture, const Vec4f& color) {
		bind();
		texture->bind();
		shader.setUniform("textured", true);
		shader.setUniform("textureSampler", texture->unit);
		shader.setUniform("color", color);
	}
};

struct BlurShader : public ShaderProgram {
	BlurShader() : ShaderProgram() {}
	BlurShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, 2, "image", "horizontal") {}

	enum class BlurType {
		HORIZONTAL = 0,
		VERTICAL = 1
	};

	void updateType(BlurType type) {
		bind();
		shader.setUniform("horizontal", (int) type);
	}

	void updateTexture(Texture* texture) {
		bind();
		texture->bind();
		shader.setUniform("image", texture->unit);
	}
};

struct PostProcessShader : public ShaderProgram {
	PostProcessShader() : ShaderProgram() {}
	PostProcessShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, 1, "textureSampler") {}

	void updateTexture(Texture* texture) {
		bind();
		texture->bind();
		shader.setUniform("textureSampler", texture->unit);
	}

	void updateTexture(HDRTexture* texture) {
		bind();
		texture->bind();
		shader.setUniform("textureSampler", texture->unit);
	}
};

struct OriginShader : public ShaderProgram {
	OriginShader() : ShaderProgram() {}
	OriginShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, 5, "viewMatrix", "rotatedViewMatrix", "projectionMatrix", "orthoMatrix", "viewPosition") {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& rotatedViewMatrix, const Mat4f& projectionMatrix, const Mat4f& orthoMatrix, const Vec3f& viewPosition) {
		bind();
		shader.setUniform("viewMatrix", viewMatrix);
		shader.setUniform("rotatedViewMatrix", rotatedViewMatrix);
		shader.setUniform("projectionMatrix", projectionMatrix);
		shader.setUniform("orthoMatrix", orthoMatrix);
		shader.setUniform("viewPosition", viewPosition);
	}
};

struct FontShader : public ShaderProgram {
	FontShader() : ShaderProgram() {}
	FontShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, 3, "projectionMatrix", "color", "text") {}

	void updateColor(const Vec4f& color) {
		bind();
		shader.setUniform("color", color);
	}

	void updateProjection(const Mat4f& projectionMatrix) {
		bind();
		shader.setUniform("projectionMatrix", projectionMatrix);
	}

	void updateTexture(Texture* texture) {
		bind();
		texture->bind();
		shader.setUniform("text", texture->unit);
	}
};

struct VectorShader : public ShaderProgram {
	VectorShader() : ShaderProgram() {}
	VectorShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, 3, "viewMatrix", "projectionMatrix", "viewPosition") {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Vec3f& viewPosition) {
		bind();
		shader.setUniform("viewMatrix", viewMatrix);
		shader.setUniform("projectionMatrix", projectionMatrix);
		shader.setUniform("viewPosition", viewPosition);
	}
};

struct PointShader : public ShaderProgram {
	PointShader() : ShaderProgram() {}
	PointShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, 3, "viewMatrix", "projectionMatrix", "viewPosition") {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Vec3f& viewPosition) {
		bind();
		shader.setUniform("viewMatrix", viewMatrix);
		shader.setUniform("projectionMatrix", projectionMatrix);
		shader.setUniform("viewPosition", viewPosition);
	}
};

struct TestShader : public ShaderProgram {
	TestShader() : ShaderProgram() {}
	TestShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, 5, "displacementMap", "viewMatrix", "modelMatrix", "projectionMatrix", "viewPosition") {}

	void updateProjection(const Mat4f& projectionMatrix) {
		bind();
		shader.setUniform("projectionMatrix", projectionMatrix);
	}

	void updateView(const Mat4f& viewMatrix){
		bind();
		shader.setUniform("viewMatrix", viewMatrix);
	}

	void updateModel(const Mat4f& modelMatrix) {
		bind();
		shader.setUniform("modelMatrix", modelMatrix);
	}

	void updateViewPosition(const Vec3f& viewPosition) {
		bind();
		shader.setUniform("viewPosition", viewPosition);
	}

	void updateDisplacement(Texture* displacementMap) {
		bind();
		displacementMap->bind();
		shader.setUniform("displacementMap", displacementMap->unit);
	}
};

struct ColorWheelShader : public ShaderProgram {
	ColorWheelShader() : ShaderProgram() {}
	ColorWheelShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, 1, "projectionMatrix") {}

	void updateProjection(Mat4f projectionMatrix) {
		bind();
		shader.setUniform("projectionMatrix", projectionMatrix);
	}
};

namespace Shaders {
	extern BasicShader basicShader;
	extern DepthShader depthShader;
	extern VectorShader vectorShader;
	extern OriginShader originShader;
	extern FontShader fontShader;
	extern QuadShader quadShader;
	extern PostProcessShader postProcessShader;
	extern SkyboxShader skyboxShader;
	extern PointShader pointShader;
	extern TestShader testShader;
	extern BlurShader blurShader;
	extern ColorWheelShader colorWheelShader;
	extern LineShader lineShader;

	void init();
	void close();
}
