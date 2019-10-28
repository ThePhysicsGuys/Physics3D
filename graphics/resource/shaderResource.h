#pragma once

#include "../util/resource/resource.h"

#include "../shader/shaderProgram.h"

#pragma region SkyboxShader

//! SkyboxShader

class SkyboxShaderAllocator : public ResourceAllocator<SkyboxShader> {
public:
	virtual SkyboxShader* load(const std::string& name, const std::string& path) override;
};

class SkyboxShader : public Resource, public ShaderProgram {
public:
	DEFINE_RESOURCE(Shader, "../res/shaders/skybox.shader");

	SkyboxShader(const std::string& name, const std::string& path, ShaderSource shaderSource) : Resource(name, path), ShaderProgram(shaderSource, "viewMatrix", "projectionMatrix", "skyboxTexture", "lightDirection") {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix);
	void updateCubeMap(CubeMap* skybox);
	void updateLightDirection(const Vec3f& lightDirection);

	virtual void close() override {
		ShaderProgram::close();
	}

	static SkyboxShaderAllocator getAllocator() {
		return SkyboxShaderAllocator();
	}
};

#pragma endregion

#pragma region MaskShader

//! MaskShader

class MaskShaderAllocator : public ResourceAllocator<MaskShader> {
public:
	virtual MaskShader* load(const std::string& name, const std::string& path) override;
};

class MaskShader : public Resource, public ShaderProgram {
public:
	DEFINE_RESOURCE(Shader, "../res/shaders/skybox.shade");

	MaskShader(const std::string& name, const std::string& path, ShaderSource shaderSource) : Resource(name, path), ShaderProgram(shaderSource, "viewMatrix", "projectionMatrix", "modelMatrix", "color") {}

	void updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix);
	void updateModel(const Mat4f& modelMatrix);
	void updateColor(const Vec4f& color);

	virtual void close() {
		ShaderProgram::close();
	}

	MaskShaderAllocator getAllocator() {
		return MaskShaderAllocator();
	}
};

#pragma endregion