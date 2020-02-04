#include "core.h"

#include "GL/glew.h"

#include "testLayer.h"
#include "view/screen.h"

#include "../engine/ecs/entity.h"
#include "ecs/model.h"
#include "ecs/transform.h"

#include "../graphics/gui/imgui/imguiExtension.h"
#include "../util/resource/resourceManager.h"
#include "../graphics/shader/shader.h"
#include "../graphics/resource/shaderResource.h"
#include "../graphics/debug/debug.h"
#include "../graphics/buffers/frameBuffer.h"
#include <sstream>

namespace Application {

FrameBuffer* frameBuffer;
Shader* shader;
unsigned int quadVAO;
unsigned int quadVBO;
unsigned int instanceVBO;

void TestLayer::onInit() {
	shader = ResourceManager::add<ShaderResource>("../res/shaders/instance.shader");
	frameBuffer = new FrameBuffer(300, 300);

	int index = 0;
	float offset = 0.1f;
	Vec2 translations[100];
	for (int y = -10; y < 10; y += 2) {
		for (int x = -10; x < 10; x += 2) {
			Vec2 translation;
			translation.x = (float) x / 10.0f + offset;
			translation.y = (float) y / 10.0f + offset;
			translations[index++] = translation;
		}
	}

	float quadVertices[] = {
		// positions     // colors
		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		-0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		 0.05f,  0.05f,  0.0f, 1.0f, 1.0f
	};

	// create
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &instanceVBO);
	glGenBuffers(1, &quadVBO);

	// bind
	glBindVertexArray(quadVAO);

	// quadVertices
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, 5 * 6 * sizeof(float), &quadVertices[0], GL_STATIC_DRAW);
	// vertices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
	// colors
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (2 * sizeof(float)));

	// offsets
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec2) * 100, &translations[0], GL_STATIC_DRAW);
	// offsets
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*) 0);

	// divisor
	glVertexAttribDivisor(2, 1);
}

void TestLayer::onUpdate() {
	
}

void TestLayer::onEvent(Event& event) {

}

void TestLayer::onRender() {
	shader->bind();
	frameBuffer->bind();
	glEnable(GL_DEPTH_TEST);
	
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw 100 instanced quads
	shader->bind();
	glBindVertexArray(quadVAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100); // 100 triangles of 6 vertices each
	glBindVertexArray(0);
	frameBuffer->unbind();

	ImGui::Begin("Test");
	ImGui::Image((void*) (intptr_t) frameBuffer->getID(), ImVec2(frameBuffer->dimension.x, frameBuffer->dimension.y));
	ImGui::End();
}

void TestLayer::onClose() {

}

};