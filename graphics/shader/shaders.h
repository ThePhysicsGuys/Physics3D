#pragma once

#include "shadeR.h"

#include "../physics/math/linalg/mat.h"

class Texture;

struct GuiShader : public Shader {
	GuiShader() : Shader() {}
	GuiShader(ShaderSource shaderSource) : Shader(shaderSource) {}

	void init(const Mat4f& orthoMatrix);
	void setTextured(bool textured);
};

struct QuadShader : public Shader {
	QuadShader() : Shader() {}
	QuadShader(ShaderSource shaderSource) : Shader(shaderSource) {}

	void updateProjection(const Mat4f& orthoMatrix);
	void updateColor(const Vec4& color);
	void updateTexture(Texture* texture);
	void updateTexture(Texture* texture, const Vec4f& color);
};

struct BlurShader : public Shader {
	BlurShader() : Shader() {}
	BlurShader(ShaderSource shaderSource) : Shader(shaderSource) {}

	void updateWidth(float width);
	void updateTexture(Texture* texture);
};

namespace GraphicsShaders {
	extern GuiShader guiShader;
	extern QuadShader quadShader;
	extern BlurShader horizontalBlurShader;
	extern BlurShader verticalBlurShader;

	void onInit();
	void onClose();
}