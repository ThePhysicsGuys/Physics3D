#pragma once

#include "../engine/layer/layer.h"

namespace P3D::Graphics {
class ArrayMesh;
class PointMesh;
class VectorMesh;
};

class Screen;

namespace P3D::Application {

class DebugLayer : public Engine::Layer {
private:
	Graphics::VectorMesh* vectorMesh = nullptr;
	Graphics::PointMesh* pointMesh = nullptr;
	Graphics::ArrayMesh* originMesh = nullptr;

public:
	inline DebugLayer() : Layer() {};
	inline DebugLayer(Screen* screen, char flags = NoUpdate | NoEvents) : Layer("Debug layer", screen, flags) {};

	virtual void onInit() override;
	virtual void onUpdate() override;
	virtual void onEvent(Engine::Event& event) override;
	virtual void onRender() override;
	virtual void onClose() override;
};

};