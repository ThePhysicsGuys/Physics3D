#pragma once

#include "../engine/layer/layer.h"
#include "../graphics/buffers/frameBuffer.h"

namespace P3D::Application {

class Screen;

class OutlineLayer : public Engine::Layer {
private:
	URef<Graphics::FrameBuffer> outlineFrameBuffer = nullptr;
	
public:
	OutlineLayer() : Layer() {}
	OutlineLayer(Screen* screen, char flags = 0) : Layer("Outline", screen, flags) {}

	void onInit(Engine::Registry64& registry) override;
	void onEvent(Engine::Registry64& registry, Engine::Event& event) override;
	void onRender(Engine::Registry64& registry) override;
};

};