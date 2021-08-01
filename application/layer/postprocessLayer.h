#pragma once

#include "../engine/layer/layer.h"
#include "../graphics/buffers/frameBuffer.h"

namespace P3D::Application {

class Screen;

class PostprocessLayer : public Engine::Layer {
private:
	URef<Graphics::HDRFrameBuffer> horizontalBlurFrameBuffer = nullptr;
	URef<Graphics::HDRFrameBuffer> verticalBlurFrameBuffer = nullptr;
	
public:
	PostprocessLayer() : Layer() {}
	PostprocessLayer(Screen* screen, char flags = 0) : Layer("Postprocess", screen, flags) {};

	void onInit(Engine::Registry64& registry) override;
	void onUpdate(Engine::Registry64& registry) override;
	void onEvent(Engine::Registry64& registry, Engine::Event& event) override;
	void onRender(Engine::Registry64& registry) override;
	void onClose(Engine::Registry64& registry) override;
};

};