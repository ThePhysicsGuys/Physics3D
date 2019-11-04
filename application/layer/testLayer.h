#pragma once

#include "../engine/layer/layer.h"

#include "../graphics/batch/batch.h"
#include "../graphics/batch/batchConfig.h"
#include "../graphics/buffers/bufferLayout.h"
#include "../graphics/path/path3D.h"

class Screen;

class TestLayer : public Layer {
private:
	Batch<Path3D::Vertex>* batch = nullptr;

public:
	TestLayer();
	TestLayer(Screen* screen, char flags = 0);

	void onInit() override;
	void onUpdate() override;
	void onEvent(Event& event) override;
	void onRender() override;
	void onClose() override;
};
