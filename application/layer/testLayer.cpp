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

namespace Application {

struct Uniform {
	Mat4f model;
	Vec4f ambient;
	Vec3f diffuse;
	Vec3f specular;
	float reflectance;
};

std::vector<Uniform> uniforms;
MeshResource* mesh;

void TestLayer::onInit() {
	float s = 10;
	fsrepeat(x, s, 2) {
		fsrepeat(y, s, 2) {
			frepeat(z, 1) {
				uniforms.push_back(
					{
						Mat4f {
							1, 0, 0, x,
							0, 1, 0, y+10,
							0, 0, 1, z+2,
							0, 0, 0, 1,
						},
						Vec4f(x/s, y/s, z/s, 1.0),
						Vec3f(1, 1, 1),
						Vec3f(1, 1, 1),
						1.0f
					}
				);
			}
		}
	}

	BufferLayout layout = BufferLayout(
		{
			BufferElement("vModelMatrix", BufferDataType::MAT4, true),
			BufferElement("vAmbient", BufferDataType::FLOAT4, true),
			BufferElement("vDiffuse", BufferDataType::FLOAT3, true),
			BufferElement("vSpecular", BufferDataType::FLOAT3, true),
			BufferElement("vReflectance", BufferDataType::FLOAT, true),
		}
	);
	VertexBuffer* vbo = new VertexBuffer(uniforms.data(), uniforms.size() * sizeof(Uniform));

	mesh = ResourceManager::add<MeshResource>("plane", "../res/models/plane.obj");
	//mesh = ResourceManager::add<MeshResource>("sphere", "../res/models/sphere.obj");
	mesh->getMesh()->addUniformBuffer(vbo, layout);

	ResourceManager::add<TextureResource>("wall_color", "../res/textures/wall/wall_color.jpg");
	ResourceManager::add<TextureResource>("wall_normal", "../res/textures/wall/wall_normal.jpg");

	std::vector<Light*> lights;
	Attenuation attenuation = { 0, 0, 0.5 };
	lights.push_back(new Light(Vec3f(10, 5, -10), Color3(1, 0.84f, 0.69f), 6, attenuation));
	lights.push_back(new Light(Vec3f(10, 5, 10), Color3(1, 0.84f, 0.69f), 6, attenuation));
	lights.push_back(new Light(Vec3f(-10, 5, -10), Color3(1, 0.84f, 0.69f), 6, attenuation));
	lights.push_back(new Light(Vec3f(-10, 5, 10), Color3(1, 0.84f, 0.69f), 6, attenuation));
	lights.push_back(new Light(Vec3f(0, 5, 0), Color3(1, 0.90f, 0.75f), 10, attenuation));
	ApplicationShaders::lightingShader.updateLight(lights);
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
	glEnable(GL_DEPTH_TEST);

	ResourceManager::get<TextureResource>("wall_color")->bind(0);
	ApplicationShaders::lightingShader.setUniform("textureMap", 0);

	ResourceManager::get<TextureResource>("wall_normal")->bind(1);
	ApplicationShaders::lightingShader.setUniform("normalMap", 1);

	ApplicationShaders::lightingShader.updateProjection(screen->camera.viewMatrix, screen->camera.projectionMatrix);

	//mesh->getMesh()->renderInstanced(uniforms.size(), FILL);
	mesh->getMesh()->render(FILL);

	endScene();
}

void TestLayer::onClose() {

}

};