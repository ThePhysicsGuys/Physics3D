#pragma once

#include "../engine/layer/layer.h"
#include <chrono>

namespace P3D::Application {

class Screen;

class CameraLayer : public Engine::Layer {
private:
	std::chrono::time_point<std::chrono::steady_clock> lastUpdate = std::chrono::steady_clock::now();

public:
	inline CameraLayer() : Layer() {};
	inline CameraLayer(Screen* screen, char flags = None) : Layer("CameraLayer", screen, flags) {};

	virtual void onInit(Engine::Registry64& registry) override;
	virtual void onUpdate(Engine::Registry64& registry) override;
	virtual void onEvent(Engine::Registry64& registry, Engine::Event& event) override;
	virtual void onRender(Engine::Registry64& registry) override;
	virtual void onClose(Engine::Registry64& registry) override;
};

};