#pragma once

#include "../graphics/batch/instanceBatchManager.h"
#include "../engine/layer/layer.h"

namespace P3D::Application {

class Screen;

class ModelLayer final : public Engine::Layer {
public:
	
public:
	ModelLayer() : Layer() {}
	ModelLayer(Screen* screen, char flags = None) : Layer("Model", screen, flags) {}

	virtual ~ModelLayer() = default;
	
	void onInit(Engine::Registry64& registry) override;
	void onUpdate(Engine::Registry64& registry) override;
	void onEvent(Engine::Registry64& registry, Engine::Event& event) override;
	void onRender(Engine::Registry64& registry) override;
	void onClose(Engine::Registry64& registry) override;
};

};