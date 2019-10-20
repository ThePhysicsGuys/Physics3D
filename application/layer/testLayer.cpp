#include "core.h"

#include "testLayer.h"
#include "view/screen.h"
#include "../graphics/path/path3D.h"
#include "../graphics/path/path.h"
#include "../graphics/batch/batch.h"
#include "../graphics/batch/batchConfig.h"
#include "../graphics/buffers/bufferLayout.h"
#include "../shader/shaders.h"
#include "../graphics/renderUtils.h"
#include "../graphics/gui/image.h"
#include "../graphics/gui/frame.h"

#include "../engine/resource/resource.h"
#include "../engine/resource/resourceManager.h"
#include "../engine/resource/textureResource.h"

Batch<Path3D::Vertex>* batch = nullptr;

Frame* frame = nullptr;
Image* image = nullptr;

TestLayer::TestLayer() : Layer() {}

TestLayer::TestLayer(Screen* screen, char flags) : Layer("TestLayer", screen, flags) {}

void TestLayer::onInit() {
	batch = new Batch<Path3D::Vertex> (
		BatchConfig(
			BufferLayout({
				BufferElement("pos", BufferDataType::FLOAT3),
				BufferElement("col", BufferDataType::FLOAT4)
			}),
		    Renderer::LINES)
		);


	/*ResourceManager::add<TextureResource>("../res/textures/logo.png", "test");
	ResourceManager::add<TextureResource>("../res/textures/logo.png", "test");
	auto r = ResourceManager::get<TextureResource>("test");
	auto g = ResourceManager::get<TextureResource>("test");
	image = new Image(0, 0, r);
	frame = new Frame(0, 0, "oi");
	frame->add(image);
	GUI::add(frame);*/
}

void TestLayer::onUpdate() {

}

void TestLayer::onEvent(Event& event) {

}

void TestLayer::onRender() {
	/*Path3D::bind(batch);
	Path3D::line(Vec3f(0), Vec3f(1));

	Shaders::lineShader.updateProjection(screen->camera.projectionMatrix, screen->camera.viewMatrix);
	Renderer::disableDepthTest();
	Path3D::batch->submit();*/


	
}

void TestLayer::onClose() {

}
