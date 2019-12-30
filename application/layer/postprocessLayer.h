#pragma once

#include "../engine/layer/layer.h"

namespace Application {

class Screen;

class PostprocessLayer : public Layer {
public:
	PostprocessLayer() : Layer() {};
	PostprocessLayer(Screen* screen, char flags = NoUpdate | NoEvents) : Layer("Postprocess", screen, flags) {};

	void onInit() override;
	void onUpdate() override;
	void onEvent(::Event& event) override;
	void onRender() override;
	void onClose() override;
};

};