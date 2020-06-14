#include "core.h"

#include "GL/glew.h"

#include "testLayer.h"
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

#include "application.h"
#include "shader/shaders.h"

namespace Application {

float near = 0.001;
float far = 100;
Mat4f lightProjection = ortho(-25.0, 25.0, -25.0, 25.0, near, far);
Mat4f lightView = lookAt({ -15.0, 15.0, -15.0 }, { 0.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 });

unsigned int TestLayer::depthMap = 0;
Mat4f TestLayer::lighSpaceMatrix = lightProjection * lightView;

GLID depthMapFBO;

GLID WIDTH = 1024;
GLID HEIGHT = 1024;

IndexedMesh* mesh = nullptr;

void TestLayer::onInit() {
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
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void TestLayer::onUpdate() {

}

void TestLayer::onEvent(Engine::Event& event) {
	using namespace Engine;

}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad() {
	if (quadVAO == 0) {
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) 0);
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void TestLayer::renderScene() {
	std::multimap<int, ExtendedPart*> visibleParts;
	screen.world->syncReadOnlyOperation([&visibleParts] () {
		for (ExtendedPart& part : screen.world->iterParts(ALL_PARTS)) 
			visibleParts.insert({ part.visualData.drawMeshId, &part });
	});

	for (auto iterator = visibleParts.begin(); iterator != visibleParts.end(); ++iterator) {
		ExtendedPart* part = (*iterator).second;

		if (part->visualData.drawMeshId == -1)
			continue;


		auto mat = Mat4f(Mat3f(part->getCFrame().getRotation().asRotationMatrix()) * DiagonalMat3f(part->hitbox.scale), Vec3f(part->getCFrame().getPosition() - Position(0, 0, 0)), Vec3f(0.0f, 0.0f, 0.0f), 1.0f);
		Shaders::depthShader.updateModel(mat);
		Engine::MeshRegistry::meshes[part->visualData.drawMeshId]->render(part->renderMode);
	}
}

void TestLayer::onRender() {
	using namespace Graphics;
	using namespace Graphics::Renderer;

	Screen* screen = static_cast<Screen*>(this->ptr);

	Shaders::depthShader.bind();
	Shaders::depthShader.updateLight(lighSpaceMatrix);
	glViewport(0, 0, WIDTH, HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);
	renderScene();
	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, GUI::windowInfo.dimension.x, GUI::windowInfo.dimension.y);

	/*Shaders::depthBufferShader.bind();
	Shaders::depthBufferShader.updatePlanes(near, far);
	Shaders::depthBufferShader.updateDepthMap(0, depthMap);
	renderQuad();*/
}

void TestLayer::onClose() {
}

};