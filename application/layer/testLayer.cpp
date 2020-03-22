#include "core.h"

#include "GL/glew.h"

#include "testLayer.h"
#include "view/screen.h"

#include "../graphics/renderer.h"
#include "../graphics/shader/shader.h"
#include "../graphics/mesh/indexedMesh.h"
#include "../graphics/buffers/frameBuffer.h"
#include "../graphics/buffers/vertexArray.h"
#include "../graphics/buffers/indexBuffer.h"
#include "../graphics/buffers/vertexBuffer.h"
#include "../graphics/buffers/bufferLayout.h"

#include "application.h"
#include "shader/shaders.h"
#include "input/standardInputHandler.h"
#include "worlds.h"

namespace Application {

std::vector<Vec2f> translations;
Shader* shader;
VertexArray* vao;
IndexBuffer* ibo;

IndexedMesh* mesh;

float x = 0;
float y = 0;
float aspect = 1;
float s = 1;
float vs = 0.1;
float a = 0;
float va = 0.1;

void TestLayer::onInit() {
	//float size = 100.0;
	//for (float y = -size; y < size; y += 1.0) {
	//	for (float x = -size; x < size; x += 1.0) {
	//		Vec2f translation;
	//		translation.x = x / size;
	//		translation.y = y / size;
	//		translations.push_back(translation);
	//	}
	//}

	//float quadVertices[] = {
	//	// positions     // colors
	//	-0.01f, -0.01f,  1.0f, 0.0f, 0.0f,
	//	 0.01f, -0.01f,  0.0f, 1.0f, 0.0f,
	//	 0.01f,  0.01f,  0.0f, 0.0f, 1.0f,
	//	-0.01f,  0.01f,  0.0f, 1.0f, 1.0f
	//};

	//unsigned int indices[] = {
	//	0, 1, 2,
	//	2, 3, 0
	//};

	//mesh = new IndexedMesh(quadVertices, NULL, NULL, indices, 4, 2);

	//vao = new VertexArray();
	//BufferLayout l1 = BufferLayout({
	//	BufferElement("vPosition", BufferDataType::FLOAT2),
	//	BufferElement("vColor", BufferDataType::FLOAT3),
	//});
	//
	//BufferLayout l2 = BufferLayout({
	//	BufferElement("vOffset", BufferDataType::FLOAT2, true),
	//});

	//VertexBuffer* quadVBO = new VertexBuffer(quadVertices, 20 * sizeof(float));
	//VertexBuffer* offsetVBO = new VertexBuffer(translations.data(), translations.size() * sizeof(Vec2f));
	//ibo = new IndexBuffer(indices, 6);

	//vao->addBuffer(quadVBO, l1);
	//vao->addBuffer(offsetVBO, l2);

	//std::string vertexShader = R"(
	//	#version 330 core
	//	layout (location = 0) in vec2 vPosition;
	//	layout (location = 1) in vec3 vColor;
	//	layout (location = 2) in vec2 vOffset;

	//	out vec3 fColor;

	//	uniform mat3 matrix;

	//	void main() {
	//		vec2 position = (matrix * vec3(vPosition, 1.0)).xy;
	//		//vec2 position = vPosition;
	//		gl_Position = vec4(position + vOffset, 0.0, 1.0);
	//		//gl_Position = vec4(position, 0, 1);
	//		fColor = vColor;
	//	}  
	//)";

	//std::string fragmentShader = R"(
	//	#version 330 core
	//	in vec3 fColor;
	//	out vec4 outColor;
	//	
	//	void main() {
	//		outColor = vec4(fColor, 1);
	//	}
	//)";

	//shader = new Shader("name", vertexShader, fragmentShader);
}

bool onKeyPressEvent(const KeyPressEvent& event) {
	switch (event.getKey()) {
		case GLFW_KEY_Q:
			a -= va;
			break;
		case GLFW_KEY_E:
			a += va;
			break;
		case GLFW_KEY_R:
			s += vs;
			break;
		case GLFW_KEY_F:
			s -= vs;
			break;
	}
	return false;
}

void TestLayer::onUpdate() {
	aspect = screen.camera.aspect;
	x = screen.camera.cframe.position.x;
	y = -screen.camera.cframe.position.z;
}

void TestLayer::onEvent(Event& event) {
	DISPATCH_EVENT_FUNCTION(event, KeyPressEvent, onKeyPressEvent);
}

void TestLayer::onRender() {
	using namespace Graphics;
	using namespace Graphics::Renderer;

	beginScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	Mat3f matrix = {
		s / aspect * cos(a), -s * sin(a), 0,
		s / aspect * sin(a),  s * cos(a), 0,
		x                  ,  y         , s
	};

	shader->bind();
	shader->setUniform("matrix", matrix);

	vao->bind();
	ibo->bind();
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, translations.size());

	endScene();
}

void TestLayer::onClose() {

}

};