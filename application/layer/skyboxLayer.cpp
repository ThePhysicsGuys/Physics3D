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

Graphics::CubeMap* skyboxTexture = nullptr;

void SkyboxLayer::onInit() {
	skyboxTexture = new Graphics::CubeMap("../res/skybox/right.jpg", "../res/skybox/left.jpg", "../res/skybox/top.jpg", "../res/skybox/bottom.jpg", "../res/skybox/front.jpg", "../res/skybox/back.jpg");
}

void SkyboxLayer::onUpdate() {

}

void SkyboxLayer::onEvent(Event& event) {

}

void SkyboxLayer::onRender() {
	Screen* screen = static_cast<Screen*>(this->ptr);

	Graphics::graphicsMeasure.mark(Graphics::GraphicsProcess::SKYBOX);

	Graphics::Renderer::beginScene();

	Graphics::Renderer::disableDepthMask();
	Graphics::Renderer::disableCulling();
	Graphics::Renderer::enableBlending();
	ApplicationShaders::skyboxShader.updateLightDirection(Vec3());
	ApplicationShaders::skyboxShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix);
	skyboxTexture->bind();
	Graphics::Library::sphere->render();

	Graphics::Renderer::endScene();
}

void SkyboxLayer::onClose() {
	skyboxTexture->close();
}

};