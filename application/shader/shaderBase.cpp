#include "core.h"

#include "shaderBase.h"

#include "../physics/math/globalCFrame.h"
#include "../ecs/light.h"
#include "../graphics/gui/color.h"

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

void BasicShaderBase::updateLight(const std::vector<Light*> lights) {
	bind();

	std::size_t size = lights.size();
	assert(size <= std::numeric_limits<int>::max());
	setUniform("lightCount", static_cast<int>(size));

	for (int i = 0; i < lights.size(); i++) {
		std::string uniform;
		std::string index = std::to_string(i);
		std::string variable = "lights[" + index + "].";

		// position
		uniform = variable + "position";
		setUniform(uniform, lights[i]->position);

		// color
		uniform = variable + "color";
		setUniform(uniform, lights[i]->color);

		// intensity
		uniform = variable + "intensity";
		setUniform(uniform, lights[i]->intensity);

		// attenuation.constant
		uniform = variable + "attenuation.constant";
		setUniform(uniform, lights[i]->attenuation.constant);

		// attenuation.linear
		uniform = variable + "attenuation.linear";
		setUniform(uniform, lights[i]->attenuation.linear);

		// attenuation.exponent
		uniform = variable + "attenuation.exponent";
		setUniform(uniform, lights[i]->attenuation.exponent);
	}
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