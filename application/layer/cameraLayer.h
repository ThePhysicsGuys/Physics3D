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

	virtual void onInit() override;
	virtual void onUpdate() override;
	virtual void onEvent(Engine::Event& event) override;
	virtual void onRender() override;
	virtual void onClose() override;
};

};