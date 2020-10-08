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

	virtual void onInit(Engine::Registry64& registry) override;
	virtual void onUpdate(Engine::Registry64& registry) override;
	virtual void onEvent(Engine::Registry64& registry, Engine::Event& event) override;
	virtual void onRender(Engine::Registry64& registry) override;
	virtual void onClose(Engine::Registry64& registry) override;
};

};