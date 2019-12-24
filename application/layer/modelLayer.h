#pragma once

#include "../engine/layer/layer.h"
#include "../graphics/batch/instanceBatchManager.h"

class Screen;

class ModelLayer : public Layer {
private:
	InstanceBatchManager* manager = nullptr;
public:
	ModelLayer();
	ModelLayer(Screen* screen, char flags = None);

	void onInit() override;
	void onUpdate() override;
	void onEvent(Event& event) override;
	void onRender() override;
	void onClose() override;
};