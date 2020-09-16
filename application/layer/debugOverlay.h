#pragma once

#include "../engine/layer/layer.h"

namespace P3D::Application {

class Screen;

class DebugOverlay : public Engine::Layer {
public:
	inline DebugOverlay() : Layer() {};
	inline DebugOverlay(Screen* screen, char flags = NoEvents) : Layer("Debug overlay", screen, flags) {};

	virtual void onInit(Engine::Registry64& registry) override;
	virtual void onUpdate(Engine::Registry64& registry) override;
	virtual void onEvent(Engine::Registry64& registry, Engine::Event& event) override;
	virtual void onRender(Engine::Registry64& registry) override;
	virtual void onClose(Engine::Registry64& registry) override;
};

};