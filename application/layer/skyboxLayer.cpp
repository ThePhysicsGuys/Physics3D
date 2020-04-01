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
	/*
		inclination = 0.49;
		azimuth = 0.25;
		var theta = Math.PI * (inclination - 0.5);
		var phi = 2 * Math.PI * (azimuth - 0.5);
		
		var distance = 400000;
		sunPosition.x = distance * Math.cos(phi);
		sunPosition.y = distance * Math.sin(phi) * Math.sin(theta);
		sunPosition.z = distance * Math.sin(phi) * Math.cos(theta);
	*/

	if (pressed) {
		disableCulling();
		disableDepthMask();
		enableBlending();
		skyboxTexture->bind();
		ApplicationShaders::skyShader.setUniform("tex", 0);
		ApplicationShaders::skyShader.updateCamera(fromPosition(screen->camera.cframe.position), screen->camera.projectionMatrix, screen->camera.viewMatrix);
		Library::sphere->render();
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