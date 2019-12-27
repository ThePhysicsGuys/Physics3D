#pragma once

#include "../engine/layer/layer.h"

namespace Application {

class Screen;

class SkyboxLayer : public Layer {
public:
	SkyboxLayer() : Layer() {};
	SkyboxLayer(Screen* screen, char flags = NoUpdate | NoEvents) : Layer("Skybox", screen, flags) {};

	void onInit() override;
	void onUpdate() override;
	void onEvent(Event& event) override;
	void onRender() override;
	void onClose() override;
};

};