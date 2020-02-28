#pragma once

#include "../engine/layer/layer.h"

namespace Application {

class Screen;

class SkyboxLayer : public Layer {
public:
	inline SkyboxLayer() : Layer() {};
	inline SkyboxLayer(Screen* screen, char flags = NoUpdate | NoEvents) : Layer("Skybox", screen, flags) {};

	virtual void onInit() override;
	virtual void onUpdate() override;
	virtual void onEvent(Event& event) override;
	virtual void onRender() override;
	virtual void onClose() override;
};

};