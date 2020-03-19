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
#include "application.h"
#include "../graphics/buffers/vertexArray.h"
#include "../graphics/buffers/vertexBuffer.h"
#include "../graphics/buffers/bufferLayout.h"
#include "../engine/ecs/tree.h"
#include "../engine/ecs/entity.h"
#include "worlds.h"
#include "../graphics/path/path.h"

namespace Application {

Graphics::FrameBuffer* frameBuffer;
Graphics::VertexArray* vao;
Graphics::VertexBuffer* vbo;

std::vector<Vec2> translations;
GLuint quadVAO;
GLuint shader;

static unsigned int compileShader(const std::string& source, unsigned int type) {
	GLuint id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	GLint result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE) {
		GLint length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*) alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		Log::error(message);
		glDeleteShader(id);
		return 0;
	}
	return id;
}

static GLuint createShader	(const std::string& vertexShader, const std::string& fragmentShader) {
	GLuint program = glCreateProgram();
	GLuint vs = compileShader(vertexShader, GL_VERTEX_SHADER);
	GLuint fs = compileShader(fragmentShader, GL_FRAGMENT_SHADER);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);
	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

void TestLayer::onInit() {
	/*frameBuffer = new Graphics::FrameBuffer(300, 300);

	float size = 10.0;
	for (float y = -size; y < size; y += 1.0) {
		for (float x = -size; x < size; x += 1.0) {
			Vec2 translation;
			translation.x = x / size;
			translation.y = y / size;
			translations.push_back(translation);
		}
	}

	float quadVertices[] = {
		// positions     // colors
		-0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
		 0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
		-0.05f, -0.05f,  0.0f, 0.0f, 1.0f,
		 0.05f,  0.05f,  0.0f, 1.0f, 1.0f
	};

	unsigned int indices[] = {
		1, 3, 2,
		1, 3, 0
	};

	GLuint instanceVBO;
	glGenBuffers(1, &instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, translations.size() * sizeof(Vec2), translations.data(), GL_STATIC_DRAW);

	// create
	GLuint quadVBO;
	GLuint indexVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glGenBuffers(1, &indexVBO);
	glBindVertexArray(quadVAO);

	// buffer
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	// indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// vertices
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);

	// colors
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (intptr_t) (2 * sizeof(float)));

	// offsets
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*) 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// divisor
	glVertexAttribDivisor(2, 1);

	std::string vertexShader = R"(
		#version 330 core
		layout (location = 0) in vec2 vPosition;
		layout (location = 1) in vec3 vColor;
		layout (location = 2) in vec2 vOffset;

		out vec3 fColor;

		uniform mat3 matrix;

		void main() {
			vec2 position = (matrix * vec3(vPosition, 1.0)).xy;
			gl_Position = vec4(position + vOffset, 0.0, 1.0);
			fColor = vColor;
		}  
	)";

	std::string fragmentShader = R"(
		#version 330 core
		in vec3 fColor;
		out vec4 outColor;
		
		void main() {
			outColor = vec4(fColor, 1);
		}
	)";

	shader = createShader(vertexShader, fragmentShader);*/
}

void TestLayer::onUpdate() {
	
}

void TestLayer::onEvent(Event& event) {

}

void TestLayer::onRender() {
	/*using namespace Graphics;
	using namespace Graphics::Renderer;*/
	
	/*beginScene();
	
	frameBuffer->bind();
	bindShader(shader);
	
	glBindVertexArray(quadVAO);
	
	//clearColor();
	clearDepth();
	enableCulling();
	enableBlending();
	enableDepthTest();

	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, translations.size());
	
	frameBuffer->unbind();
	
	endScene();*/
	
}

void TestLayer::onClose() {

}

};