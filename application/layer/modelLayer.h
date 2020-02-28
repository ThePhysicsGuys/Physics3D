#pragma once

#include "../engine/layer/layer.h"

namespace Application {

class Screen;

class ModelLayer : public Layer {
public:
	inline ModelLayer() : Layer() {};
	inline ModelLayer(Screen* screen, char flags = None) : Layer("Model", screen, flags) {};

	virtual void onInit() override;
	virtual void onUpdate() override;
	virtual void onEvent(Event& event) override;
	virtual void onRender() override;
	virtual void onClose() override;
};

};