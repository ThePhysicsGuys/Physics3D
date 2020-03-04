#include "core.h"

#include "skyboxLayer.h"

#include <sstream>

#include "shader/shaders.h"
#include "view/screen.h"
#include "../graphics/renderer.h"
#include "../graphics/texture.h"
#include "../graphics/mesh/indexedMesh.h"
#include "../graphics/meshLibrary.h"
#include "../graphics/debug/visualDebug.h"
#include "../physics/geometry/boundingBox.h"

namespace Application {

CubeMap* skyboxTexture = nullptr;

void SkyboxLayer::onInit() {
	skyboxTexture = new CubeMap("../res/skybox/right.jpg", "../res/skybox/left.jpg", "../res/skybox/top.jpg", "../res/skybox/bottom.jpg", "../res/skybox/front.jpg", "../res/skybox/back.jpg");
}

void SkyboxLayer::onUpdate() {

}

void SkyboxLayer::onEvent(Event& event) {

}

void SkyboxLayer::onRender() {
	Screen* screen = static_cast<Screen*>(this->ptr);

	graphicsMeasure.mark(GraphicsProcess::SKYBOX);

	Renderer::beginScene();

	Renderer::disableDepthMask();
	Renderer::disableCulling();
	Renderer::enableBlending();
	ApplicationShaders::skyboxShader.updateLightDirection(Vec3());
	ApplicationShaders::skyboxShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix);
	skyboxTexture->bind();
	Library::sphere->render();

	Renderer::endScene();
}

void SkyboxLayer::onClose() {
	skyboxTexture->close();
}

};