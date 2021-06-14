#include "core.h"

#include "shaders.h"
#include "extendedPart.h"

#include "../graphics/texture.h"
#include "../graphics/renderer.h"
#include "../util/resource/resourceManager.h"

#include <sstream>

namespace P3D::Application {

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
DepthBufferShader depthBufferShader;

void onInit() {
	// GShader source init
	ShaderSource basicShaderSource       = parseShader("BasicShader", "../res/shaders/basic.shader");
	ShaderSource depthShaderSource       = parseShader("DepthShader", "../res/shaders/depth.shader");
	ShaderSource vectorShaderSource      = parseShader("VectorShader", "../res/shaders/vector.shader");
	ShaderSource fontShaderSource        = parseShader("FontShader", "../res/shaders/font.shader");
	ShaderSource originShaderSource      = parseShader("OriginShader", "../res/shaders/origin.shader");
	ShaderSource postProcessShaderSource = parseShader("PostProcessShader", "../res/shaders/postProcess.shader");
	ShaderSource skyboxShaderSource      = parseShader("SkyboxShader", "../res/shaders/skybox.shader");
	ShaderSource pointShaderSource       = parseShader("PointShader", "../res/shaders/point.shader");
	ShaderSource testShaderSource        = parseShader("TestShader", "../res/shaders/test.shader");
	ShaderSource lineShaderSource        = parseShader("LineShader", "../res/shaders/line.shader");
	ShaderSource maskShaderSource        = parseShader("MaskShader", "../res/shaders/mask.shader");
	ShaderSource instanceShaderSource    = parseShader("InstanceShader", "../res/shaders/instance.shader");
	ShaderSource skyShaderSource         = parseShader("SkyShader", "../res/shaders/sky.shader");
	ShaderSource lightingShaderSource    = parseShader("LightingShader", "../res/shaders/lighting.shader");
	ShaderSource debugShaderSource       = parseShader("DebugShader", "../res/shaders/debug.shader");
	ShaderSource depthBufferShaderSource = parseShader("DepthBufferShader", "../res/shaders/depthbuffer.shader");

	// GShader init
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
	new(&depthBufferShader) DepthBufferShader(depthBufferShaderSource);

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
	ResourceManager::add(&depthBufferShader);
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
	depthBufferShader.close();
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
	setUniform("color", Vec4f(color));
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
	setUniform("color", Vec4f(color));
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

// SkyShader

void SkyShader::updateTime(float time) {
	bind();
	setUniform("time", time);
}

// DepthBufferShader

void DepthBufferShader::updateDepthMap(GLID unit, GLID id) {
	bind();
	Renderer::activeTexture(unit);
	Renderer::bindTexture2D(id);
	setUniform("depthMap", unit);
}

void DepthBufferShader::updatePlanes(float near, float far) {
	bind();
	setUniform("far", far);
	setUniform("near", near);
}

};