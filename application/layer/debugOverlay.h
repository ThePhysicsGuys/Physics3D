#pragma once

#include "../engine/layer/layer.h"

class Screen;

class DebugOverlay : public Layer {
public:
	DebugOverlay() : Layer() {};
	DebugOverlay(Screen* screen, char flags = NoEvents) : Layer("Debug overlay", screen, flags) {};

	void onInit() override;
	void onUpdate() override;
	void onEvent(Event& event) override;
	void onRender() override;
	void onClose() override;
};