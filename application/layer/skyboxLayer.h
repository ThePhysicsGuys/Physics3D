#pragma once

#include "../engine/layer/layer.h"

namespace Application {

class Screen;

class SkyboxLayer : public Layer {
public:
	SkyboxLayer();
	SkyboxLayer(Screen* screen, char flags = NoUpdate | NoEvents);

	void onInit() override;
	void onUpdate() override;
	void onEvent(Event& event) override;
	void onRender() override;
	void onClose() override;
};

};