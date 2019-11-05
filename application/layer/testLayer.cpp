#include "core.h"

#include "testLayer.h"
#include "view/screen.h"
#include "../shader/shaders.h"
#include "../graphics/renderUtils.h"

TestLayer::TestLayer() : Layer() {}

TestLayer::TestLayer(Screen* screen, char flags) : Layer("TestLayer", screen, flags) {}

void TestLayer::onInit() {
	BufferLayout layout = BufferLayout({
		BufferElement("pos", BufferDataType::FLOAT3),
		BufferElement("col", BufferDataType::FLOAT4)
	});

	BatchConfig config = BatchConfig(layout, Renderer::LINES);
	batch = new Batch<Path3D::Vertex>(config);
}

void TestLayer::onUpdate() {

}

void TestLayer::onEvent(Event& event) {

}

void TestLayer::onRender() {
	Path3D::bind(batch);

	for (float t = 0.0f; t <= 10.0f; t += 0.01f) {
		float x = t;
		float y = cos(2 * t);
		float z = sin(t + cos(1 - t));
		Path3D::lineTo(Vec3f(x, y, z));
	}

	Path3D::stroke([] (int i, const Vec3f& p) {
		return GUI::COLOR::hsvaToRgba(
			Vec4f(1.0f / Path3D::size() * i, 1, 1, 1)
		);
	});

	ApplicationShaders::lineShader.updateProjection(screen->camera.projectionMatrix, screen->camera.viewMatrix);
	Renderer::disableDepthTest();
	Path3D::batch->submit();
}

void TestLayer::onClose() {
	batch->close();
}
