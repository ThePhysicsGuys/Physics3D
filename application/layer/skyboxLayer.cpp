#include "core.h"

#include "skyboxLayer.h"

#include <sstream>

#include "shader/shaders.h"
#include "view/screen.h"
#include "../graphics/renderUtils.h"
#include "../graphics/texture.h"
#include "../graphics/mesh/indexedMesh.h"
#include "../graphics/meshLibrary.h"
#include "../graphics/debug/visualDebug.h"
#include "../physics/geometry/boundingBox.h"

CubeMap* skyboxTexture = nullptr;

SkyboxLayer::SkyboxLayer() : Layer() {

};

SkyboxLayer::SkyboxLayer(Screen* screen, char flags) : Layer("Skybox", screen, flags) {

}

void SkyboxLayer::onInit() {
	skyboxTexture = new CubeMap("../res/skybox/right.jpg", "../res/skybox/left.jpg", "../res/skybox/top.jpg", "../res/skybox/bottom.jpg", "../res/skybox/front.jpg", "../res/skybox/back.jpg");
}

void SkyboxLayer::onUpdate() {

}

void SkyboxLayer::onEvent(Event& event) {

}

void SkyboxLayer::onRender() {
	graphicsMeasure.mark(GraphicsProcess::SKYBOX);

	Renderer::disableDepthMask();
	Renderer::disableCulling();
	Renderer::enableBlending();
	Renderer::standardBlendFunction();
	ApplicationShaders::skyboxShader.updateLightDirection(Vec3());
	ApplicationShaders::skyboxShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix);
	skyboxTexture->bind();
	Library::sphere->render();
	Renderer::enableDepthMask();
	Renderer::enableCulling();
	Renderer::enableDepthTest();
}

void SkyboxLayer::onClose() {
	skyboxTexture->close();
}
