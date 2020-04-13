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

#include "../graphics/resource/textureResource.h"
#include "../engine/resource/meshResource.h"
#include "../util/resource/resourceManager.h"
#include "../application/ecs/light.h"
#include "application.h"
#include "shader/shaders.h"
#include "input/standardInputHandler.h"
#include "worlds.h"
#include "../physics/math/mathUtil.h"
#include "../engine/io/export.h"
namespace Application {

struct Uniform {
	Mat4f model;
	Vec3f albedo;
	Vec3f mrao;
};

std::vector<Light*> llights;
std::vector<Uniform> testUniforms;
MeshResource* mesh;

void TestLayer::onInit() {
	float size = 1.0f;
	float space = 2.5f;
	frepeat(row, size) {
		frepeat(col, size) {
			frepeat(z, 1) {
				testUniforms.push_back(
					{
						Mat4f {
							1, 0, 0, (col - size / 2.0f) * space,
							0, 1, 0, 20+(row - size / 2.0f) * space,
							0, 0, 1, 0,
							0, 0, 0, 1,
						},
						Vec3f(0.5, 0, 0),
						Vec3f(row / size, col / size + 0.05f, 1.0f),
					}
				);
			}
		}
	}

	BufferLayout layout = BufferLayout(
		{
			BufferElement("vModelMatrix", BufferDataType::MAT4, true),
			BufferElement("vAlbedo", BufferDataType::FLOAT3, true),
			BufferElement("vMRAo", BufferDataType::FLOAT3, true),
		}
	);
	VertexBuffer* vbo = new VertexBuffer(testUniforms.data(), testUniforms.size() * sizeof(Uniform));

	mesh = ResourceManager::add<MeshResource>("ball", "../res/models/ball.bobj");

	//mesh = ResourceManager::add<MeshResource>("plane", "../res/models/plane.obj");
	//mesh = ResourceManager::add<MeshResource>("sphere", "../res/models/sphere.obj");
	
	mesh->getMesh()->addUniformBuffer(vbo, layout);

	ResourceManager::add<TextureResource>("ball_color", "../res/textures/ball/ball_color.png");
	ResourceManager::add<TextureResource>("ball_normal", "../res/textures/ball/ball_normal.png");
	ResourceManager::add<TextureResource>("ball_metallic", "../res/textures/ball/ball_metal.png");
	ResourceManager::add<TextureResource>("ball_roughness", "../res/textures/ball/ball_gloss.png");
	ResourceManager::add<TextureResource>("ball_ao", "../res/textures/ball/ball_ao.jpg");


	llights.push_back(new Light(Vec3f(-10,10,10), Color3(300), 1, { 1, 1, 1 }));
	llights.push_back(new Light(Vec3f(10,10,10), Color3(300), 1, { 1, 1, 1 }));
	llights.push_back(new Light(Vec3f(-10,-10,10), Color3(300), 1, { 1, 1, 1 }));
	llights.push_back(new Light(Vec3f(10,-10,10), Color3(300), 1, { 1, 1, 1 }));
	ApplicationShaders::lightingShader.updateLight(llights);
}

void TestLayer::onUpdate() {

}

void TestLayer::onEvent(Event& event) {
	if (event.getType() == EventType::KeyPress) {
		if (static_cast<KeyPressEvent&>(event).getKey() == Keyboard::TAB.code) {
			llights[0]->position = fromPosition(screen.camera.cframe.position);
			ApplicationShaders::lightingShader.updateLight(llights);
		}
	}
}

void TestLayer::onRender() {
	using namespace Graphics;
	using namespace Graphics::Renderer;

	Screen* screen = static_cast<Screen*>(this->ptr);

	beginScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	ApplicationShaders::lightingShader.bind();

	ResourceManager::get<TextureResource>("ball_color")->bind(0);
	ResourceManager::get<TextureResource>("ball_normal")->bind(1);
	ResourceManager::get<TextureResource>("ball_metallic")->bind(2);
	ResourceManager::get<TextureResource>("ball_roughness")->bind(3);
	ResourceManager::get<TextureResource>("ball_ao")->bind(4);
	ApplicationShaders::lightingShader.updateTexture(true);

	ApplicationShaders::lightingShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix, screen->camera.cframe.position);

	mesh->getMesh()->renderInstanced(testUniforms.size(), FILL);

	endScene();
}

void TestLayer::onClose() {

}

};