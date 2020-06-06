#include "core.h"

#include "shaders.h"
#include "resources.h"
#include "extendedPart.h"

#include "../graphics/texture.h"

#include "../util/resource/resource.h"
#include "../util/resource/resourceManager.h"

#include <sstream>

namespace Application {

namespace Shaders {
	
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
DebugShader debugShader;

void onInit() {
	// Shader source init
	ShaderSource basicShaderSource       = parseShader("BasicShader", "basic.shader", getResourceAsString(applicationResources, BASIC_SHADER));
	ShaderSource depthShaderSource       = parseShader("DepthShader", "depth.shader", getResourceAsString(applicationResources, DEPTH_SHADER));
	ShaderSource vectorShaderSource      = parseShader("VectorShader", "vector.shader", getResourceAsString(applicationResources, VECTOR_SHADER));
	ShaderSource fontShaderSource        = parseShader("FontShader", "font.shader", getResourceAsString(applicationResources, FONT_SHADER));
	ShaderSource originShaderSource      = parseShader("OriginShader", "origin.shader", getResourceAsString(applicationResources, ORIGIN_SHADER));
	ShaderSource postProcessShaderSource = parseShader("PostProcessShader", "postProcess.shader", getResourceAsString(applicationResources, POSTPROCESS_SHADER));
	ShaderSource skyboxShaderSource      = parseShader("SkyBoxShader", "skybox.shader", getResourceAsString(applicationResources, SKYBOX_SHADER));
	ShaderSource pointShaderSource       = parseShader("PointShader", "point.shader", getResourceAsString(applicationResources, POINT_SHADER));
	ShaderSource testShaderSource        = parseShader("TestShader", "test.shader", getResourceAsString(applicationResources, TEST_SHADER));
	ShaderSource lineShaderSource        = parseShader("LineShader", "line.shader", getResourceAsString(applicationResources, LINE_SHADER));
	ShaderSource maskShaderSource        = parseShader("MaskShader", "mask.shader", getResourceAsString(applicationResources, MASK_SHADER));
	ShaderSource instanceShaderSource    = parseShader("InstanceShader", "instance.shader", getResourceAsString(applicationResources, INSTANCE_SHADER));
	ShaderSource skyShaderSource         = parseShader("SkyShader", "sky.shader", getResourceAsString(applicationResources, SKY_SHADER));
	ShaderSource lightingShaderSource    = parseShader("LightingShader", "lighting.shader", getResourceAsString(applicationResources, LIGHTING_SHADER));
	ShaderSource debugShaderSource       = parseShader("DebugShader", "debug.shader", getResourceAsString(applicationResources, DEBUG_SHADER));


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
	new(&debugShader) DebugShader(debugShaderSource);

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
	ResourceManager::add(&debugShader);
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
	debugShader.close();
}

}

// SkyboxShader

void SkyboxShader::updateCubeMap(Graphics::CubeMap* skybox) {
	bind();
	setUniform("skyboxTexture", skybox->getUnit());
}

void SkyboxShader::updateLightDirection(const Vec3f& lightDirection) {
	bind();
	setUniform("lightDirection", lightDirection);
}


// MeskShader

void MaskShader::updateColor(const Color& color) {
	bind();
	setUniform("color", color);
}


// DepthShader

void DepthShader::updateLight(const Mat4f& lightMatrix) {
	bind();
	setUniform("lightMatrix", lightMatrix);
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

// TestShader

void TestShader::updateDisplacement(Graphics::Texture* displacementMap) {
	bind();
	displacementMap->bind();
	setUniform("displacementMap", displacementMap->getUnit());
}

// SkyShader

void SkyShader::updateTime(float time) {
	bind();
	setUniform("time", time);
}

};