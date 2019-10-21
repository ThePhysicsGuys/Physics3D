#include "core.h"

#include "shaderResource.h"

#include "../graphics/texture.h"
#include "../resourceLoader.h"
#include <sstream>

#pragma region SkyboxShader

//! SkyboxShader

SkyboxShader* SkyboxShaderAllocator::load(const std::string& name, const std::string& path) {
	ShaderSource source = parseShader((std::istream&) std::istringstream(getResourceAsString(SKYBOX_SHADER)), name);
	return new SkyboxShader(name, path, source);
}

void SkyboxShader::updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix) {
	bind();
	shader.setUniform("viewMatrix", viewMatrix);
	shader.setUniform("projectionMatrix", projectionMatrix);
}

void SkyboxShader::updateCubeMap(CubeMap* skybox) {
	bind();
	shader.setUniform("skyboxTexture", skybox->getUnit());
}

void SkyboxShader::updateLightDirection(const Vec3f& lightDirection) {
	bind();
	shader.setUniform("lightDirection", lightDirection);
}

#pragma endregion

#pragma region MaskShader

//! MaskShader

MaskShader* MaskShaderAllocator::load(const std::string& name, const std::string& path) {
	ShaderSource source = parseShader((std::istream&) std::istringstream(getResourceAsString(MASK_SHADER)), name);
	return new MaskShader(name, path, source);
}

void MaskShader::updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix) {
	bind();
	shader.setUniform("viewMatrix", viewMatrix);
	shader.setUniform("projectionMatrix", projectionMatrix);
}

void MaskShader::updateModel(const Mat4f& modelMatrix) {
	bind();
	shader.setUniform("modelMatrix", modelMatrix);
}

void MaskShader::updateColor(const Vec4f& color) {
	bind();
	shader.setUniform("color", color);
}

#pragma endregion