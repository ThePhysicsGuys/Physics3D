#pragma once

#include "../engine/layer/layer.h"

namespace Application {

class Screen;

class TestLayer : public Layer {
public:
	static unsigned int depthMap;
	static Mat4f lightProjection;
	static Mat4f lighSpaceMatrix;
	static Mat4f lightView;

	inline TestLayer() : Layer() {};
	inline TestLayer(Screen* screen, char flags = None) : Layer("TestLayer", screen, flags) {};

	void renderScene();

	virtual void onInit() override;
	virtual void onUpdate() override;
	virtual void onEvent(Engine::Event& event) override;
	virtual void onRender() override;
	virtual void onClose() override;
};

};