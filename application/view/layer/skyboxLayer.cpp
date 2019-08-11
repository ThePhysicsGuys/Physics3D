#include "skyboxLayer.h"

#include <sstream>

#include "../shaderProgram.h"
#include "../renderUtils.h"
#include "../texture.h"
#include "../screen.h"

#include "../mesh/indexedMesh.h"
#include "../application/meshLibrary.h"

#include "../engine/geometry/boundingBox.h"

#include "../debug/visualDebug.h"

CubeMap* skyboxTexture = nullptr;

SkyboxLayer::SkyboxLayer() : Layer() {

};

SkyboxLayer::SkyboxLayer(Screen* screen) : Layer("Skybox", screen, noUpdate | noEvents) {

}

void SkyboxLayer::init() {
	skyboxTexture = new CubeMap("../res/skybox/right.jpg", "../res/skybox/left.jpg", "../res/skybox/top.jpg", "../res/skybox/bottom.jpg", "../res/skybox/front.jpg", "../res/skybox/back.jpg");
}

void SkyboxLayer::update() {

}

void SkyboxLayer::render() {
	graphicsMeasure.mark(GraphicsProcess::SKYBOX);

	Renderer::disableDepthMask();
	Renderer::disableCulling();
	Renderer::enableBlending();
	Renderer::standardBlendFunction();
	Shaders::skyboxShader.updateLightDirection(Vec3());
	Shaders::skyboxShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix);
	skyboxTexture->bind();
	Library::sphere->render();
	Renderer::enableDepthMask();
	Renderer::enableCulling();
	Renderer::enableDepthTest();
}

void SkyboxLayer::close() {

}
