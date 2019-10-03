#include "core.h"

#include "testLayer.h"
#include "../screen.h"
#include "../path/path3D.h"
#include "../path/path.h"
#include "../batch/batch.h"
#include "../batch/batchConfig.h"
#include "../buffers/bufferLayout.h"
#include "../shaderProgram.h"
#include "../renderUtils.h"
#include "../gui/image.h"
#include "../gui/frame.h"

#include "resource/resource.h"
#include "resource/resourceManager.h"
#include "resource/textureResource.h"

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
