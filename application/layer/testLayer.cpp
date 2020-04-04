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

#include "../graphics/gui/valueCycle.h"

namespace Application {

std::vector<Mat4f> models;

void TestLayer::onInit() {

	models.push_back(Mat4f {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	});

	BufferLayout layout = BufferLayout({
		BufferElement("vModelMatrix", BufferDataType::MAT4, true)
		//BufferElement("vambient", BufferDataType::FLOAT4, true),
		//BufferElement("vdiffuse", BufferDataType::FLOAT3, true),
		//BufferElement("vspecular", BufferDataType::FLOAT3, true),
		//BufferElement("vreflectance", BufferDataType::FLOAT, true),
		});
	VertexBuffer* vbo = new VertexBuffer(models.data(), sizeof(Mat4f));

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

	ApplicationShaders::instanceShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix, screen->camera.cframe.position);
	//ApplicationShaders::instanceShader.setUniform("modelMatrix", Mat4f::IDENTITY());

	mesh->renderInstanced(1);

	endScene();
}

void TestLayer::onClose() {

}

};