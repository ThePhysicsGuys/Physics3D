#include "core.h"

#include "shaders.h"
#include "resources.h"
#include "extendedPart.h"

#include "../graphics/texture.h"
#include "ecs/material.h"
#include "ecs/light.h"

#include "../util/resource/resource.h"
#include "../util/resource/resourceManager.h"

#include <sstream>

namespace Application {

namespace ApplicationShaders {
	
BasicShader basicShader;
DepthShader depthShader;
VectorShader vectorShader;
OriginShader originShader;
FontShader fontShader;
PostProcessShader postProcessShader;
SkyboxShader skyboxShader;
PointShader pointShader;
TestShader testShader;
LineShader lineShader;
MaskShader maskShader;
InstanceShader instanceShader;
LightingShader lightingShader;
SkyShader skyShader;

void onInit() {
	// Shader source init
	ShaderSource basicShaderSource = parseShader("basic.shader", getResourceAsString(applicationResources, BASIC_SHADER));
	ShaderSource depthShaderSource = parseShader("depth.shader", getResourceAsString(applicationResources, DEPTH_SHADER));
	ShaderSource vectorShaderSource = parseShader("vector.shader", getResourceAsString(applicationResources, VECTOR_SHADER));
	ShaderSource fontShaderSource = parseShader("font.shader", getResourceAsString(applicationResources, FONT_SHADER));
	ShaderSource originShaderSource = parseShader("origin.shader", getResourceAsString(applicationResources, ORIGIN_SHADER));
	ShaderSource postProcessShaderSource = parseShader("postProcess.shader", getResourceAsString(applicationResources, POSTPROCESS_SHADER));
	ShaderSource skyboxShaderSource = parseShader("skybox.shader", getResourceAsString(applicationResources, SKYBOX_SHADER));
	ShaderSource pointShaderSource = parseShader("point.shader", getResourceAsString(applicationResources, POINT_SHADER));
	ShaderSource testShaderSource = parseShader("test.shader", getResourceAsString(applicationResources, TEST_SHADER));
	ShaderSource lineShaderSource = parseShader("line.shader", getResourceAsString(applicationResources, LINE_SHADER));
	ShaderSource maskShaderSource = parseShader("mask.shader", getResourceAsString(applicationResources, MASK_SHADER));
	ShaderSource instanceShaderSource = parseShader("instance.shader", getResourceAsString(applicationResources, INSTANCE_SHADER));
	ShaderSource skyShaderSource = parseShader("sky.shader", getResourceAsString(applicationResources, SKY_SHADER));
	ShaderSource lightingShaderSource = parseShader("lighting.shader", getResourceAsString(applicationResources, LIGHTING_SHADER));


	// Shader init
	new(&basicShader) BasicShader(basicShaderSource);
	new(&depthShader) DepthShader(depthShaderSource);
	new(&vectorShader) VectorShader(vectorShaderSource);
	new(&fontShader) FontShader(fontShaderSource);
	new(&originShader) OriginShader(originShaderSource);
	new(&postProcessShader) PostProcessShader(postProcessShaderSource);
	new(&skyboxShader) SkyboxShader(skyboxShaderSource);
	new(&pointShader) PointShader(pointShaderSource);
	new(&testShader) TestShader(testShaderSource);
	new(&lineShader) LineShader(lineShaderSource);
	new(&maskShader) MaskShader(maskShaderSource);
	new(&instanceShader) InstanceShader(instanceShaderSource);
	new(&skyShader) SkyShader(skyShaderSource);
	new(&lightingShader) LightingShader(lightingShaderSource);

	ResourceManager::add(&basicShader);
	ResourceManager::add(&depthShader);
	ResourceManager::add(&vectorShader);
	ResourceManager::add(&fontShader);
	ResourceManager::add(&originShader);
	ResourceManager::add(&postProcessShader);
	ResourceManager::add(&skyboxShader);
	ResourceManager::add(&pointShader);
	ResourceManager::add(&testShader);
	ResourceManager::add(&lineShader);
	ResourceManager::add(&maskShader);
	ResourceManager::add(&instanceShader);
	ResourceManager::add(&skyShader);
	ResourceManager::add(&lightingShader);
}

void onClose() {
	basicShader.close();
	depthShader.close();
	vectorShader.close();
	fontShader.close();
	originShader.close();
	skyboxShader.close();
	postProcessShader.close();
	pointShader.close();
	testShader.close();
	lineShader.close();
	maskShader.close();
	instanceShader.close();
	skyShader.close();
	lightingShader.close();
}

}


// BasicShader

void BasicShader::updatePart(const ExtendedPart& part) {
	bind();
	BasicShader::updateTexture(false);
	BasicShader::updateModel(part.getCFrame(), DiagonalMat3f(part.hitbox.scale));
}

void BasicShader::updateMaterial(const Material& material) {
	bind();
	setUniform("material.albedo", material.albedo);
	setUniform("material.metalness", material.metalness);
	setUniform("material.roughness", material.roughness);
	setUniform("material.ambientOcclusion", material.ao);

	// todo Bind textures
}

void BasicShader::updateTexture(bool textured) {
	bind();
	setUniform("material.albedoMap", 0);
	setUniform("material.normalMap", 1);
	setUniform("material.metalnessMap", 2);
	setUniform("material.roughnessMap", 3);
	setUniform("material.ambientOcclusionMap", 4);
	setUniform("material.textured", textured);
}


void BasicShader::updateLight(const std::vector<Light*> lights) {
	bind();

	int size = lights.size();
	setUniform("lightCount", size);

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

void BasicShader::updateSunDirection(const Vec3f& sunDirection) {
	bind();
	setUniform("sunDirection", sunDirection);
}

void BasicShader::updateSunColor(const Color3& sunColor) {
	bind();
	setUniform("sunColor", sunColor);
}

void BasicShader::updateGamma(float gamma) {
	bind();
	setUniform("gamma", gamma);
}

void BasicShader::updateHDR(float hdr) {
	bind();
	setUniform("hdr", hdr);
}

void BasicShader::updateExposure(float exposure) {
	bind();
	setUniform("exposure", exposure);
}

void BasicShader::updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Position& viewPosition) {
	bind();
	setUniform("viewMatrix", viewMatrix);
	setUniform("projectionMatrix", projectionMatrix);
	setUniform("viewPosition", viewPosition);
}

void BasicShader::updateModel(const Mat4f& modelMatrix) {
	bind();
	setUniform("modelMatrix", modelMatrix);
}

static Mat4f GlobalCFrameToMat4f(const GlobalCFrame& modelCFrame, DiagonalMat3f scale) {
	return Mat4f(Mat3f(modelCFrame.getRotation().asRotationMatrix()) * scale, Vec3f(modelCFrame.getPosition() - Position(0, 0, 0)), Vec3f(0.0f, 0.0f, 0.0f), 1.0f);
}

void BasicShader::updateModel(const GlobalCFrame& modelCFrame, DiagonalMat3f scale) {
	updateModel(GlobalCFrameToMat4f(modelCFrame, scale));
}


// SkyboxShader

void SkyboxShader::updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix) {
	bind();
	setUniform("viewMatrix", viewMatrix);
	setUniform("projectionMatrix", projectionMatrix);
}

void SkyboxShader::updateCubeMap(Graphics::CubeMap* skybox) {
	bind();
	setUniform("skyboxTexture", skybox->getUnit());
}

void SkyboxShader::updateLightDirection(const Vec3f& lightDirection) {
	bind();
	setUniform("lightDirection", lightDirection);
}


// MeskShader

void MaskShader::updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix) {
	bind();
	setUniform("viewMatrix", viewMatrix);
	setUniform("projectionMatrix", projectionMatrix);
}

void MaskShader::updateModel(const Mat4f& modelMatrix) {
	bind();
	setUniform("modelMatrix", modelMatrix);
}
void MaskShader::updateModel(const GlobalCFrame& modelCFrame, DiagonalMat3f scale) {
	this->updateModel(GlobalCFrameToMat4f(modelCFrame, scale));
}

void MaskShader::updateColor(const Color& color) {
	bind();
	setUniform("color", color);
}


// DepthShader

void DepthShader::updateLight(const Mat4f& lightMatrix) {
	bind();
	setUniform("lightMatrix", lightMatrix);
}

void DepthShader::updateModel(const Mat4f& modelMatrix) {
	bind();
	setUniform("modelMatrix", modelMatrix);
}
void DepthShader::updateModel(const GlobalCFrame& modelCFrame, DiagonalMat3f scale) {
	this->updateModel(GlobalCFrameToMat4f(modelCFrame, scale));
}


// PostProcessShader

void PostProcessShader::updateTexture(Graphics::Texture* texture) {
	bind();
	texture->bind();
	setUniform("textureSampler", texture->getUnit());
}

void PostProcessShader::updateTexture(Graphics::HDRTexture* texture) {
	bind();
	texture->bind();
	setUniform("textureSampler", texture->getUnit());
}


// OriginShader

void OriginShader::updateProjection(const Mat4f& viewMatrix, const Mat4f& rotatedViewMatrix, const Mat4f& projectionMatrix, const Mat4f& orthoMatrix, const Position& viewPosition) {
	bind();
	setUniform("viewMatrix", viewMatrix);
	setUniform("rotatedViewMatrix", rotatedViewMatrix);
	setUniform("projectionMatrix", projectionMatrix);
	setUniform("orthoMatrix", orthoMatrix);
	setUniform("viewPosition", viewPosition);
}


// FontShader

void FontShader::updateColor(const Color& color) {
	bind();
	setUniform("color", color);
}

void FontShader::updateProjection(const Mat4f& projectionMatrix) {
	bind();
	setUniform("projectionMatrix", projectionMatrix);
}

void FontShader::updateTexture(Graphics::Texture* texture) {
	bind();
	texture->bind();
	setUniform("text", texture->getUnit());
}


// VectorShader

void VectorShader::updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Position& viewPosition) {
	bind();
	setUniform("viewMatrix", viewMatrix);
	setUniform("projectionMatrix", projectionMatrix);
	setUniform("viewPosition", viewPosition);
}


// PointShader

void PointShader::updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Position& viewPosition) {
	bind();
	setUniform("viewMatrix", viewMatrix);
	setUniform("projectionMatrix", projectionMatrix);
	setUniform("viewPosition", viewPosition);
}


// TestShader

void TestShader::updateProjection(const Mat4f& projectionMatrix) {
	bind();
	setUniform("projectionMatrix", projectionMatrix);
}

void TestShader::updateView(const Mat4f& viewMatrix) {
	bind();
	setUniform("viewMatrix", viewMatrix);
}

void TestShader::updateModel(const Mat4f& modelMatrix) {
	bind();
	setUniform("modelMatrix", modelMatrix);
}
void TestShader::updateModel(const GlobalCFrame& modelCFrame, DiagonalMat3f scale) {
	this->updateModel(GlobalCFrameToMat4f(modelCFrame, scale));
}

void TestShader::updateViewPosition(const Position& viewPosition) {
	bind();
	setUniform("viewPosition", viewPosition);
}

void TestShader::updateDisplacement(Graphics::Texture* displacementMap) {
	bind();
	displacementMap->bind();
	setUniform("displacementMap", displacementMap->getUnit());
}


// LineShader

void LineShader::updateProjection(const Mat4f& projectionMatrix, const Mat4f& viewMatrix) {
	bind();
	setUniform("projectionMatrix", projectionMatrix);
	setUniform("viewMatrix", viewMatrix);
}


// InstanceShader

void InstanceShader::updateLight(const std::vector<Light*> lights) {
	bind();

	int size = lights.size();
	setUniform("lightCount", size);

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

void InstanceShader::updateTexture(unsigned char flags) {
	bind();
	setUniform("albedoMap", 0);
	setUniform("normalMap", 1);
	setUniform("metalnessMap", 2);
	setUniform("roughnessMap", 3);
	setUniform("ambientOcclusionMap", 4);
	setUniform("textured", flags);
}

void InstanceShader::updateSunDirection(const Vec3f& sunDirection) {
	bind();
	setUniform("sunDirection", sunDirection);
}

void InstanceShader::updateSunColor(const Vec3f& sunColor) {
	bind();
	setUniform("sunColor", sunColor);
}

void InstanceShader::updateGamma(float gamma) {
	bind();
	setUniform("gamma", gamma);
}

void InstanceShader::updateHDR(float hdr) {
	bind();
	setUniform("hdr", hdr);
}

void InstanceShader::updateExposure(float exposure) {
	bind();
	setUniform("exposure", exposure);
}

void InstanceShader::updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Position& viewPosition) {
	bind();
	setUniform("viewMatrix", viewMatrix);
	setUniform("projectionMatrix", projectionMatrix);
	setUniform("viewPosition", viewPosition);
}


// SkyShader

void SkyShader::updateCamera(const Vec3f& viewPosition, const Mat4f& projectionMatrix, const Mat4f& viewMatrix) {
	bind();
	setUniform("projectionMatrix", projectionMatrix);
	setUniform("viewMatrix", viewMatrix);
}

void SkyShader::updateTime(float time) {
	bind();
	setUniform("time", time);
}


// LightingShader

void LightingShader::updateLight(const std::vector<Light*> lights) {
	bind();

	int size = lights.size();
	setUniform("lightCount", size);

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

void LightingShader::updateTexture(bool textured) {
	bind();
	setUniform("albedoMap", 0);
	setUniform("normalMap", 1);
	setUniform("metallicMap", 2);
	setUniform("roughnessMap", 3);
	setUniform("ambientOcclusionMap", 4);
	setUniform("textured", textured);
}

void LightingShader::updateProjection(const Mat4f& viewMatrix, const Mat4f& projectionMatrix, const Position& viewPosition) {
	bind();
	setUniform("viewMatrix", viewMatrix);
	setUniform("projectionMatrix", projectionMatrix);
	setUniform("viewPosition", viewPosition);
}

};