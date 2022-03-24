#include "core.h"

#include "shaderBase.h"

#include <Physics3D/math/globalCFrame.h>
#include "ecs/components.h"

namespace P3D::Application {

#pragma region ProjectionShaderBase

void ProjectionShaderBase::updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Position& viewPosition) {
	bind();
	setUniform("uViewMatrix", viewMatrix);
	setUniform("uProjectionMatrix", projectionMatrix);
	setUniform("uViewPosition", viewPosition);
}

void ProjectionShaderBase::updateProjectionMatrix(const Mat4f& projectionMatrix) {
	bind();
	setUniform("uProjectionMatrix", projectionMatrix);
}

void ProjectionShaderBase::updateViewMatrix(const Mat4f& viewMatrix) {
	bind();
	setUniform("uViewMatrix", viewMatrix);
}

void ProjectionShaderBase::updateViewPosition(const Position& viewPosition) {
	bind();
	setUniform("uViewPosition", viewPosition);
}

#pragma endregion

#pragma region StandardMeshShaderBase

void StandardMeshShaderBase::updateModel(const Mat4f& modelMatrix) {
	bind();
	setUniform("uModelMatrix", modelMatrix);
}

void StandardMeshShaderBase::updateModel(const GlobalCFrame& modelCFrame, const DiagonalMat3f& scale) {
	this->updateModel(modelCFrame.asMat4WithPreScale(scale));
}

#pragma endregion

#pragma region BasicShaderBase

void BasicShaderBase::updateLightCount(std::size_t lightCount) {
	bind();

	setUniform("uLightCount", static_cast<int>(lightCount));
}
	
void BasicShaderBase::updateLight(std::size_t index, const Position& position, const Comp::Light& light) {
	bind();

	std::string variable = "uLights[" + std::to_string(static_cast<int>(index)) + "].";

	// position
	setUniform(variable + "position", position);

	// color
	setUniform(variable + "color", Vec3f(light.color));

	// intensity
	setUniform(variable + "intensity", light.intensity);

	// attenuation.constant
	setUniform(variable + "attenuation.constant", light.attenuation.constant);

	// attenuation.linear
	setUniform(variable + "attenuation.linear", light.attenuation.linear);

	// attenuation.exponent
	setUniform(variable + "attenuation.exponent", light.attenuation.exponent);
}

void BasicShaderBase::updateSunDirection(const Vec3f& sunDirection) {
	bind();
	setUniform("uSunDirection", sunDirection);
}

void BasicShaderBase::updateSunColor(const Vec3f& sunColor) {
	bind();
	setUniform("uSunColor", sunColor);
}

void BasicShaderBase::updateGamma(float gamma) {
	bind();
	setUniform("uGamma", gamma);
}

void BasicShaderBase::updateHDR(float hdr) {
	bind();
	setUniform("uHDR", hdr);
}

void BasicShaderBase::updateExposure(float exposure) {
	bind();
	setUniform("uExposure", exposure);
}

#pragma endregion

}
