#pragma once

#include "../engine/layer/layer.h"

namespace Application {

class Screen;

class ModelLayer : public Layer {
private:
	struct Uniform {
		Mat4f modelMatrix;
		Vec4f ambient;
		Vec3f diffuse;
		Vec3f specular;
		float reflectance;
	};

	std::vector<Uniform> uniforms;

public:
	inline ModelLayer() : Layer() {};
	inline ModelLayer(Screen* screen, char flags = None) : Layer("Model", screen, flags) {};

	virtual void onInit() override;
	virtual void onUpdate() override;
	virtual void onEvent(Event& event) override;
	virtual void onRender() override;
	virtual void onClose() override;
};

};