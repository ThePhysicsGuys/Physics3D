#pragma once

#include "../engine/layer/layer.h"

namespace P3D::Application {

class Screen;

class ShadowLayer : public Engine::Layer {
public:
	static unsigned int depthMap;
	static Mat4f lightProjection;
	static Mat4f lighSpaceMatrix;
	static Mat4f lightView;

	inline ShadowLayer() : Layer() {};
	inline ShadowLayer(Screen* screen, char flags = None) : Layer("ShadowLayer", screen, flags) {};

	void renderScene(Engine::Registry64& registry);

	void onInit(Engine::Registry64& registry) override;
	void onUpdate(Engine::Registry64& registry) override;
	void onEvent(Engine::Registry64& registry, Engine::Event& event) override;
	void onRender(Engine::Registry64& registry) override;
	void onClose(Engine::Registry64& registry) override;
};

};