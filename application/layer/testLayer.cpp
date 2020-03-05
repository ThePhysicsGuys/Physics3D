#include "core.h"

#include "GL/glew.h"

#include "testLayer.h"
#include "view/screen.h"
#include "../graphics/resource/textureResource.h"
#include "../engine/ecs/entity.h"
#include "ecs/model.h"
#include "ecs/transform.h"
#include "../graphics/texture.h"
#include "../graphics/gui/imgui/imguiExtension.h"
#include "../util/resource/resourceManager.h"
#include "../graphics/shader/shader.h"
#include "../graphics/resource/shaderResource.h"
#include "../graphics/debug/debug.h"
#include "../graphics/buffers/frameBuffer.h"
#include <sstream>
#include "shader/shaders.h"
#include "../graphics/renderer.h"

#include "../graphics/buffers/vertexArray.h"
#include "../graphics/buffers/vertexBuffer.h"
#include "../graphics/buffers/bufferLayout.h"

namespace Application {

Graphics::FrameBuffer* frameBuffer;
Shader* shader;
unsigned int quadVAO;
unsigned int quadVBO;
unsigned int instanceVBO;

Graphics::VertexArray* vao;
Graphics::VertexBuffer* vbo;

void TestLayer::onInit() {
	shader = ResourceManager::add<ShaderResource>("../res/shaders/instance.shader");
	frameBuffer = new Graphics::FrameBuffer(300, 300);

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

	vao = new Graphics::VertexArray();
	vbo = new Graphics::VertexBuffer(quadVertices, sizeof(quadVertices), GL_STATIC_DRAW);
	
	Graphics::BufferLayout layout = Graphics::BufferLayout(
		{ Graphics::BufferElement("positions", Graphics::BufferDataType::FLOAT2),
		Graphics::BufferElement("colors", Graphics::BufferDataType::FLOAT3) }
	);
	
	vao->addBuffer(vbo, layout);

	//glGenBuffers(1, &instanceVBO);
	//glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(translations), translations, GL_STATIC_DRAW);

	// create
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	// buffer
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	// vertices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
	// colors
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (intptr_t) (2 * sizeof(float)));
	// offsets
	//glEnableVertexAttribArray(2);
	//glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*) 0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	// divisor
	//glVertexAttribDivisor(2, 1);
}

void TestLayer::onUpdate() {
	
}

void TestLayer::onEvent(Event& event) {

}

void TestLayer::onRender() {
	Graphics::Renderer::beginScene();

	frameBuffer->bind();
	shader->bind();

	Graphics::Renderer::disableDepthMask();
	Graphics::Renderer::disableCulling();
	Graphics::Renderer::enableBlending();
	Graphics::Renderer::standardBlendFunction();
	Graphics::Renderer::clearColor();
	Graphics::Renderer::clearDepth();
	Graphics::Renderer::disableDepthTest();
	glClearColor(0.1f, 0.1f, 0.7f, 1.0f);

	// draw 100 instanced quads	
	/*glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6); // 100 triangles of 6 vertices each
	glBindVertexArray(0);*/

	vao->bind();
	Graphics::Renderer::drawArrays(Graphics::Renderer::FILL, 0, 6);
	vao->unbind();
	
	frameBuffer->unbind();

	Graphics::Renderer::enableDepthTest();
	Graphics::Renderer::enableCulling();
	Graphics::Renderer::enableDepthMask();

	ImGui::Begin("Test");
	ImGui::Image((void*) (intptr_t) frameBuffer->texture->getID(), ImVec2(300, 300));
	ImGui::End();

	Graphics::Renderer::endScene();
}

void TestLayer::onClose() {

}

};