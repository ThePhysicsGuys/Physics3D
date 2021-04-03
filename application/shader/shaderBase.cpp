#include "core.h"

#include "shaderBase.h"

#include "../physics/math/globalCFrame.h"
#include "ecs/components.h"

namespace P3D::Application {

#pragma region ProjectionShaderBase

void ProjectionShaderBase::updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Position& viewPosition) {
	bind();
	setUniform("viewMatrix", viewMatrix);
	setUniform("projectionMatrix", projectionMatrix);
	setUniform("viewPosition", viewPosition);
}

void ProjectionShaderBase::updateProjectionMatrix(const Mat4f& projectionMatrix) {
	bind();
	setUniform("projectionMatrix", projectionMatrix);
}

void ProjectionShaderBase::updateViewMatrix(const Mat4f& viewMatrix) {
	bind();
	setUniform("viewMatrix", viewMatrix);
}

void ProjectionShaderBase::updateViewPosition(const Position& viewPosition) {
	bind();
	setUniform("viewPosition", viewPosition);
}

#pragma endregion

#pragma region StandardMeshShaderBase

void StandardMeshShaderBase::updateModel(const Mat4f& modelMatrix) {
	bind();
	setUniform("modelMatrix", modelMatrix);
}

void StandardMeshShaderBase::updateModel(const GlobalCFrame& modelCFrame, const DiagonalMat3f& scale) {
	updateModel(modelCFrame.asMat4WithPreScale(scale));
}

#pragma endregion

#pragma region BasicShaderBase

void BasicShaderBase::updateLightCount(std::size_t lightCount) {
	bind();

	setUniform("lightCount", static_cast<int>(lightCount));
}
	
void BasicShaderBase::updateLight(std::size_t index, const Position& position, const Comp::Light& light) {
	bind();

	std::string variable = "lights[" + std::to_string(static_cast<int>(index)) + "].";

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
	setUniform("sunDirection", sunDirection);
}

void BasicShaderBase::updateSunColor(const Vec3f& sunColor) {
	bind();
	setUniform("sunColor", sunColor);
}

void BasicShaderBase::updateGamma(float gamma) {
	bind();
	setUniform("gamma", gamma);
}

void BasicShaderBase::updateHDR(float hdr) {
	bind();
	setUniform("hdr", hdr);
}

void BasicShaderBase::updateExposure(float exposure) {
	bind();
	setUniform("exposure", exposure);
}

#pragma endregion

}
