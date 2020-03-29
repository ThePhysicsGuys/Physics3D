#include "core.h"

#include "GL/glew.h"

#include "testLayer.h"
#include "view/screen.h"

#include "../graphics/renderer.h"
#include "../graphics/shader/shader.h"
#include "../graphics/mesh/indexedMesh.h"
#include "../graphics/buffers/frameBuffer.h"
#include "../graphics/buffers/vertexBuffer.h"
#include "../graphics/buffers/bufferLayout.h"

#include "../engine/resource/meshResource.h"
#include "../util/resource/resourceManager.h"
#include "../application/ecs/light.h"
#include "application.h"
#include "shader/shaders.h"
#include "input/standardInputHandler.h"
#include "worlds.h"

namespace Application {

void TestLayer::onInit() {

	float uniforms[27] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 0,

		1, 0, 0, 1,

		1, 1, 1,

		1, 1, 1,

		1
	};

	BufferLayout layout = BufferLayout({
		BufferElement("vmodelMatrix", BufferDataType::MAT4, true),
		BufferElement("vambient", BufferDataType::FLOAT4, true),
		BufferElement("vdiffuse", BufferDataType::FLOAT3, true),
		BufferElement("vspecular", BufferDataType::FLOAT3, true),
		BufferElement("vreflectance", BufferDataType::FLOAT, true),
	});
	VertexBuffer* vbo = new VertexBuffer(uniforms, 27 * sizeof(float));

	IndexedMesh* mesh = ResourceManager::get<MeshResource>("translate")->getMesh();
	mesh->addUniformBuffer(vbo, layout);
}

void TestLayer::onUpdate() {
	
}

void TestLayer::onEvent(Event& event) {

}

void TestLayer::onRender() {
	using namespace Graphics;
	using namespace Graphics::Renderer;

	Screen* screen = static_cast<Screen*>(this->ptr);

	beginScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	IndexedMesh* mesh = ResourceManager::get<MeshResource>("translate")->getMesh();

	ApplicationShaders::instanceBasicShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix, screen->camera.cframe.position);
	ApplicationShaders::instanceBasicShader.updateLight({ new Light(Vec3f(0.3, 0.4, 0.1), Color3(1, 0.84f, 0.69f), 6, {1, 0, 0}) }, 1);
	ApplicationShaders::instanceBasicShader.updateSunDirection(Vec3f(0.1, -1, 0.1));

	mesh->renderInstanced(1);

	endScene();
}

void TestLayer::onClose() {

}

};