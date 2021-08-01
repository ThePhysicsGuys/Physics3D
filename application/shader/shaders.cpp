#include "core.h"

#include "shaders.h"
#include "extendedPart.h"

#include "../graphics/texture.h"
#include "../graphics/renderer.h"
#include "../util/resource/resourceManager.h"

namespace P3D::Application {

namespace Shaders {
	
URef<BasicShader> basicShader = nullptr;
URef<DepthShader> depthShader = nullptr;
URef<VectorShader> vectorShader = nullptr;
URef<OriginShader> originShader = nullptr;
URef<FontShader> fontShader = nullptr;
URef<PostProcessShader> postProcessShader = nullptr;
URef<SkyboxShader> skyboxShader = nullptr;
URef<PointShader> pointShader = nullptr;
URef<TestShader> testShader = nullptr;
URef<LineShader> lineShader = nullptr;
URef<MaskShader> maskShader = nullptr;
URef<InstanceShader> instanceShader = nullptr;
URef<LightingShader> lightingShader = nullptr;
URef<SkyShader> skyShader = nullptr;
URef<DebugShader> debugShader = nullptr;
URef<DepthBufferShader> depthBufferShader = nullptr;
URef<OutlineShader> outlineShader = nullptr;
URef<BlurShader> blurShader = nullptr;

void onInit() {

	// GShader init
	basicShader = std::make_unique<BasicShader>();
	depthShader = std::make_unique<DepthShader>();
	vectorShader = std::make_unique<VectorShader>();
	fontShader = std::make_unique<FontShader>();
	originShader = std::make_unique<OriginShader>();
	postProcessShader = std::make_unique<PostProcessShader>();
	skyboxShader = std::make_unique<SkyboxShader>();
	pointShader = std::make_unique<PointShader>();
	testShader = std::make_unique<TestShader>();
	lineShader = std::make_unique<LineShader>();
	maskShader = std::make_unique<MaskShader>();
	instanceShader = std::make_unique<InstanceShader>();
	skyShader = std::make_unique<SkyShader>();
	lightingShader = std::make_unique<LightingShader>();
	debugShader = std::make_unique<DebugShader>();
	depthBufferShader = std::make_unique<DepthBufferShader>();
	outlineShader = std::make_unique<OutlineShader>();
	blurShader = std::make_unique<BlurShader>();

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
	ResourceManager::add(outlineShader.get());
	ResourceManager::add(blurShader.get());
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
	outlineShader->close();
	blurShader->close();
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

void BlurShader::updateDirection(bool horizontal) {
	bind();
	setUniform("horizontal", horizontal);
}

void BlurShader::updateUnit(int unit) {
	bind();
	setUniform("image", unit);
}

// OutlineShader
	
void OutlineShader::updateColor(const Color& color) {
	bind();
	setUniform("color", color);
}

void OutlineShader::updateUnit(GLID id) {
	bind();
	setUniform("image", id);
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