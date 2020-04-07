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
#include "../graphics/mesh/primitive.h"
#include "../graphics/resource/textureResource.h"
#include "../graphics/gui/guiUtils.h"

#include "../physics/math/mathUtil.h"
#include "../util/resource/resourceManager.h"

#include "imgui/imgui.h"

namespace Application {

SkyboxCycle lightColorCycle;
SkyboxCycle skyColorCycle;
SkyboxCycle horizonColorCycle;
SkyboxCycle mistColorCycle;

Color3 lightColor;
Color3 skyColor;
Color3 mistColor;
Color3 horizonColor;
Vec2f mist;
float starBrightness;

CubeMap* skyboxTexture = nullptr;

bool pressed;
bool pauze;

void updateMist(float time) {
	float mistFactor;
	if (time > 0.5F) 
		mistFactor = 1.0f - GUI::smoothstep(0.7083333f, 0.875f, time);
	else 
		mistFactor = GUI::smoothstep(0.20833333f, 0.375f, time);
	
	mist = Vec2f(15.0f + mistFactor * 20.0f, 75.0f + mistFactor * 50.0f);
}

void updateStars(float time) {
	if (time > 0.5F)
		starBrightness = GUI::smoothstep(0.9166667f, 1.0f, time);
	else 
		starBrightness = 1.0f - GUI::smoothstep(0.125f, 0.20833333f, time);

}

void SkyboxLayer::onInit() {
	skyboxTexture = new CubeMap("../res/skybox/right.jpg", "../res/skybox/left.jpg", "../res/skybox/top.jpg", "../res/skybox/bottom.jpg", "../res/skybox/front.jpg", "../res/skybox/back.jpg");

	ResourceManager::add<TextureResource>("night", "../res/textures/night.png");
	ResourceManager::add<TextureResource>("uv", "../res/textures/uv.png");

	lightColorCycle = SkyboxCycle(Color3(0.42, 0.45, 0.90), Color3(1.0, 0.95, 0.95), Color3(1.0, 0.45, 0.56), Color3(1.0, 0.87, 0.6), 3.0f, 8.0f, 18.0f);
	skyColorCycle = SkyboxCycle(Color3(0.31, 0.44, 0.64), Color3(0.96, 0.93, 0.9), Color3(0.996, 0.77, 0.57), Color3(1.0, 0.94, 0.67), 3.0f, 8.0f, 18.0f);
	horizonColorCycle = SkyboxCycle(Color3(0.2, 0.2, 0.42), Color3(0.6, 0.9, 1.0), Color3(0.93, 0.49, 0.57), Color3(1.0, 0.64, 0.47), 3.0f, 8.0f, 18.0f);
	mistColorCycle = SkyboxCycle(Color3(0.29, 0.41, 0.61), Color3(0.96, 0.9, 0.77), Color3(1.0, 0.68, 0.85), Color3(1.0, 0.87, 0.82), 3.0f, 8.0f, 18.0f);
}

float time;

void SkyboxLayer::onUpdate() {
	if (!pauze)
		time = fmod((float) (glfwGetTime() / 30.0), 1.0f);

	lightColor = lightColorCycle.sample(time);
	skyColor = skyColorCycle.sample(time);
	mistColor = mistColorCycle.sample(time);
	horizonColor = horizonColorCycle.sample(time);

	updateMist(time);
	updateStars(time);
}

void SkyboxLayer::onEvent(Event& event) {

}

void SkyboxLayer::onRender() {
	using namespace Graphics;
	using namespace Graphics::Renderer;
	graphicsMeasure.mark(GraphicsProcess::SKYBOX);

	Screen* screen = static_cast<Screen*>(this->ptr);
	
	beginScene();

	ImGui::Begin("Skybox");

	ImGui::Checkbox("New sky", &pressed);
	ImGui::Text("Time: %f", time);
	ImGui::Checkbox("Pauze", &pauze);
	if (pressed) {
		disableCulling();
		disableDepthMask();
		enableBlending();
		ResourceManager::get<TextureResource>("night")->bind();

		ApplicationShaders::skyShader.setUniform("nightTexture", 0);
		ApplicationShaders::skyShader.updateCamera(fromPosition(screen->camera.cframe.position), screen->camera.projectionMatrix, screen->camera.viewMatrix);
		ApplicationShaders::skyShader.setUniform("starBrightness", starBrightness);
		ApplicationShaders::skyShader.setUniform("skyColor", skyColor);
		ApplicationShaders::skyShader.setUniform("horizonColor", horizonColor);
		float scroll = (float) (atan2(screen->camera.viewMatrix[1][0], screen->camera.viewMatrix[0][0]) / screen->camera.fov / 2.0);
		ImGui::Text("Scroll: %f", scroll);

		ApplicationShaders::skyShader.setUniform("scroll", scroll);
		ApplicationShaders::skyShader.setUniform("time", time);
		ApplicationShaders::skyShader.setUniform("skyboxSize", 550.0f);
		ApplicationShaders::skyShader.setUniform("segmentCount", 25.0f);
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