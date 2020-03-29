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

#include "imgui/imgui.h"
#include "../graphics/mesh/primitive.h"
#include "../physics/math/mathUtil.h"
#include "../graphics/resource/textureResource.h"
#include "../util/resource/resourceManager.h"

namespace Application {

Graphics::CubeMap* skyboxTexture = nullptr;

void SkyboxLayer::onInit() {
	skyboxTexture = new Graphics::CubeMap("../res/skybox/right.jpg", "../res/skybox/left.jpg", "../res/skybox/top.jpg", "../res/skybox/bottom.jpg", "../res/skybox/front.jpg", "../res/skybox/back.jpg");
	ResourceManager::add<TextureResource>("uv", "../res/textures/uv.png");
	ResourceManager::add<TextureResource>("noise", "../res/textures/noise.png");
	ApplicationShaders::skyShader.updateTexture();
}

void SkyboxLayer::onUpdate() {

}

void SkyboxLayer::onEvent(Event& event) {

}

bool pressed;

void SkyboxLayer::onRender() {
	using namespace Graphics;
	using namespace Graphics::Renderer;
	graphicsMeasure.mark(GraphicsProcess::SKYBOX);

	Screen* screen = static_cast<Screen*>(this->ptr);
	
	beginScene();

	ImGui::Begin("Skybox");

	ImGui::Checkbox("New sky", &pressed);

	if (pressed) {
		disableDepthMask();
		ResourceManager::get<TextureResource>("noise")->bind();
		ApplicationShaders::skyShader.updateCamera(fromPosition(screen->camera.cframe.position), Vec3f(screen->camera.viewMatrix * Vec4f(0, 0, 1, 0)), screen->dimension);
		ApplicationShaders::skyShader.updateTime(glfwGetTime());
		screen->quad->render();
	} else {
		disableCulling();
		disableDepthMask();
		enableBlending();

		ApplicationShaders::skyboxShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix);
		skyboxTexture->bind();

		Library::sphere->render();
	}

	ImGui::End();

	endScene();
}

void SkyboxLayer::onClose() {
	skyboxTexture->close();
}

};