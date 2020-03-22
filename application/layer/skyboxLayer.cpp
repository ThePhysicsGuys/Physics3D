#include "core.h"
#include "GL/glew.h"
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
#include "../graphics/buffers/frameBuffer.h"

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
	using namespace Graphics;
	using namespace Graphics::Renderer;
	graphicsMeasure.mark(GraphicsProcess::SKYBOX);

	Screen* screen = static_cast<Screen*>(this->ptr);

	beginScene();

	disableCulling();
	disableDepthMask();
	enableBlending();

	ApplicationShaders::skyboxShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix);
	skyboxTexture->bind();
	
	Library::sphere->render();

	endScene();
}

void SkyboxLayer::onClose() {
	skyboxTexture->close();
}

};