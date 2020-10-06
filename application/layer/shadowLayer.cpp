#include "core.h"

#include "GL/glew.h"

#include "shadowLayer.h"
#include "view/screen.h"
#include "../graphics/gui/gui.h"
#include "../graphics/renderer.h"
#include "../graphics/shader/shader.h"
#include "../graphics/mesh/indexedMesh.h"
#include "../graphics/buffers/frameBuffer.h"
#include "../engine/resource/meshResource.h"
#include "../util/resource/resourceManager.h"
#include "../engine/meshRegistry.h"
#include "../graphics/mesh/primitive.h"
#include "worlds.h"
#include "imgui/imgui.h"
#include "../physics/misc/filters/visibilityFilter.h"
#include "ecs/components.h"
#include "application.h"
#include "shader/shaders.h"

namespace P3D::Application {

float near = 0.001;
float far = 100;

Mat4f ShadowLayer::lightProjection = ortho(-25.0, 25.0, -25.0, 25.0, near, far);
Mat4f ShadowLayer::lightView = lookAt({ -15.0, 15.0, -15.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 });
Mat4f ShadowLayer::lighSpaceMatrix = lightProjection * lightView;
unsigned int ShadowLayer::depthMap = 0;

GLID depthMapFBO;
GLID WIDTH = 2048;
GLID HEIGHT = 2048;

IndexedMesh* mesh = nullptr;

void ShadowLayer::onInit(Engine::Registry64& registry) {
	glGenFramebuffers(1, &depthMapFBO);

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0/*screen->screenFrameBuffer->getID()*/);
}

void ShadowLayer::onUpdate(Engine::Registry64& registry) {

}

void ShadowLayer::onEvent(Engine::Registry64& registry, Engine::Event& event) {
	using namespace Engine;

}

void ShadowLayer::renderScene(Engine::Registry64& registry) {
	std::vector<ExtendedPart*> visibleParts;
	screen.world->syncReadOnlyOperation([&visibleParts, &registry] () {
		for (ExtendedPart& part : screen.world->iterParts())
			visibleParts.push_back(&part);
		});

	for (ExtendedPart* part : visibleParts) {
		Ref<Comp::Mesh> mesh = registry.get<Comp::Mesh>(part->entity);

		if (!mesh.valid())
			continue;

		if (mesh->id == -1)
			continue;

		Shaders::depthShader.updateModel(part->getCFrame().asMat4WithPreScale(part->hitbox.scale));
		Engine::MeshRegistry::meshes[mesh->id]->render(mesh->mode);
	}
}

void ShadowLayer::onRender(Engine::Registry64& registry) {
	using namespace Graphics;
	using namespace Graphics::Renderer;

	Screen* screen = static_cast<Screen*>(this->ptr);

	Shaders::depthShader.bind();
	Shaders::depthShader.updateLight(lighSpaceMatrix);
	glViewport(0, 0, WIDTH, HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	Renderer::disableCulling();
	//glCullFace(GL_FRONT_AND_BACK);
	renderScene(registry);
	//glCullFace(GL_BACK);
	Renderer::enableCulling();
	glBindFramebuffer(GL_FRAMEBUFFER, screen->screenFrameBuffer->getID());
	glViewport(0, 0, GUI::windowInfo.dimension.x, GUI::windowInfo.dimension.y);

	/*Shaders::depthBufferShader.bind();
	Shaders::depthBufferShader.updatePlanes(near, far);
	Shaders::depthBufferShader.updateDepthMap(0, depthMap);
	Graphics::renderQuad();*/
}

void ShadowLayer::onClose(Engine::Registry64& registry) {
}

};