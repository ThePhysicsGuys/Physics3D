#pragma once

#include "../engine/layer/layer.h"
#include <chrono>

namespace P3D::Application {

class Screen;

class CameraLayer : public Engine::Layer {
private:
	std::chrono::time_point<std::chrono::steady_clock> lastUpdate = std::chrono::steady_clock::now();

public:
	CameraLayer() : Layer() {};
	CameraLayer(Screen* screen, char flags = None) : Layer("CameraLayer", screen, flags) {};

	void onInit(Engine::Registry64& registry) override;
	void onUpdate(Engine::Registry64& registry) override;
	void onEvent(Engine::Registry64& registry, Engine::Event& event) override;
	void onRender(Engine::Registry64& registry) override;
	void onClose(Engine::Registry64& registry) override;
};

};