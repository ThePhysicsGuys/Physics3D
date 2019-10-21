#pragma once

#include "shaderProgram.h"

#include "../physics/math/linalg/mat.h"

class Texture;

struct GuiShader : public ShaderProgram {
	GuiShader() : ShaderProgram() {}
	GuiShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, "projectionMatrix", "textureSampler", "textured") {}

	void init(const Mat4f& orthoMatrix);
	void setTextured(bool textured);
};

struct QuadShader : public ShaderProgram {
	QuadShader() : ShaderProgram() {}
	QuadShader(ShaderSource shaderSource) : ShaderProgram(shaderSource, "projectionMatrix", "color", "textureSampler", "textured") {}

	void updateProjection(const Mat4f& orthoMatrix);
	void updateColor(const Vec4& color);
	void updateTexture(Texture* texture);
	void updateTexture(Texture* texture, const Vec4f& color);
};

namespace GraphicsShaders {
	extern GuiShader guiShader;
	extern QuadShader quadShader;

	void onInit();
	void onClose();
}