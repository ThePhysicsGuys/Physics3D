#include "core.h"

#include "testLayer.h"
#include "view/screen.h"
#include "../graphics/renderer.h"

namespace P3D::Application {

GLID texture = 0;
int textureWidth = 512;
int textureHeight = 512;

void TestLayer::onInit(Engine::Registry64& registry) {
	/*glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	int internalFormat = GL_RGBA32F;
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, textureWidth, textureHeight, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, internalFormat);

	int workGroupCount[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workGroupCount[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workGroupCount[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workGroupCount[2]);
	printf("max global (total) work group counts x:%i y:%i z:%i\n", workGroupCount[0], workGroupCount[1], workGroupCount[2]);

	int workGroupSize[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workGroupSize[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workGroupSize[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workGroupSize[2]);
	printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n", workGroupSize[0], workGroupSize[1], workGroupSize[2]);

	int workGroupInvocations;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &workGroupInvocations);
	printf("max local work group invocations %i\n", workGroupInvocations);
	shader = Graphics::CShader(Graphics::parseShader("ComputeShader", "../res/shaders/compute.shader"));*/
}

void TestLayer::onUpdate(Engine::Registry64& registry) {

}

void TestLayer::onEvent(Engine::Registry64& registry, Engine::Event& event) {
	using namespace Engine;

}

void TestLayer::onRender(Engine::Registry64& registry) {
	using namespace Graphics;
	using namespace Renderer;

	Screen* screen = static_cast<Screen*>(this->ptr);

	/*shader.bind();
	bindTexture2D(texture);
	shader.dispatch(textureWidth, textureHeight, 1);
	shader.barrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	
	ImGui::Begin("Test");
	ImGui::Image((void*) texture, ImVec2(100, 100));
	ImGui::End();*/
}

void TestLayer::onClose(Engine::Registry64& registry) {
}

};