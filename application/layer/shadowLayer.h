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

	void renderScene();

	virtual void onInit() override;
	virtual void onUpdate() override;
	virtual void onEvent(Engine::Event& event) override;
	virtual void onRender() override;
	virtual void onClose() override;
};

};