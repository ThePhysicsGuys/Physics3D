#include "core.h"

#include "shaders.h"
#include "extendedPart.h"

#include "../graphics/texture.h"
#include "../graphics/renderer.h"
#include "../util/resource/resourceManager.h"

#include <sstream>

namespace P3D::Application {

namespace Shaders {
	
SRef<BasicShader> basicShader;
SRef<DepthShader> depthShader;
SRef<VectorShader> vectorShader;
SRef<OriginShader> originShader;
SRef<FontShader> fontShader;
SRef<PostProcessShader> postProcessShader;
SRef<SkyboxShader> skyboxShader;
SRef<PointShader> pointShader;
SRef<TestShader> testShader;
SRef<LineShader> lineShader;
SRef<MaskShader> maskShader;
SRef<InstanceShader> instanceShader;
SRef<LightingShader> lightingShader;
SRef<SkyShader> skyShader;
SRef<DebugShader> debugShader;
SRef<DepthBufferShader> depthBufferShader;

void onInit() {

	// GShader init
	basicShader = std::make_shared<BasicShader>();
	depthShader = std::make_shared<DepthShader>();
	vectorShader = std::make_shared<VectorShader>();
	fontShader = std::make_shared<FontShader>();
	originShader = std::make_shared<OriginShader>();
	postProcessShader = std::make_shared<PostProcessShader>();
	skyboxShader = std::make_shared<SkyboxShader>();
	pointShader = std::make_shared<PointShader>();
	testShader = std::make_shared<TestShader>();
	lineShader = std::make_shared<LineShader>();
	maskShader = std::make_shared<MaskShader>();
	instanceShader = std::make_shared<InstanceShader>();
	skyShader = std::make_shared<SkyShader>();
	lightingShader = std::make_shared<LightingShader>();
	debugShader = std::make_shared<DebugShader>();
	depthBufferShader = std::make_shared<DepthBufferShader>();

	ResourceManager::add(basicShader.get());
	ResourceManager::add(depthShader.get());
	ResourceManager::add(vectorShader.get());
	ResourceManager::add(fontShader.get());
	ResourceManager::add(originShader.get());
	ResourceManager::add(postProcessShader.get());
	ResourceManager::add(skyboxShader.get());
	ResourceManager::add(pointShader.get());
	ResourceManager::add(testShader.get());
	ResourceManager::add(lineShader.get());
	ResourceManager::add(maskShader.get());
	ResourceManager::add(instanceShader.get());
	ResourceManager::add(skyShader.get());
	ResourceManager::add(lightingShader.get());
	ResourceManager::add(debugShader.get());
	ResourceManager::add(depthBufferShader.get());
}

void onClose() {
	basicShader->close();
	depthShader->close();
	vectorShader->close();
	fontShader->close();
	originShader->close();
	skyboxShader->close();
	postProcessShader->close();
	pointShader->close();
	testShader->close();
	lineShader->close();
	maskShader->close();
	instanceShader->close();
	skyShader->close();
	lightingShader->close();
	debugShader->close();
	depthBufferShader->close();
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

void PostProcessShader::updateTexture(SRef<Texture> texture) {
	bind();
	texture->bind();
	setUniform("textureSampler", texture->getUnit());
}

void PostProcessShader::updateTexture(SRef<HDRTexture> texture) {
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

void FontShader::updateTexture(SRef<Texture> texture) {
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