#pragma once

#include "shader.h"
#include "../graphics/resource/shaderResource.h"
#include "../physics/math/linalg/mat.h"

namespace Graphics {
class Texture;
};

struct GuiShader : public ShaderResource {
	GuiShader() : ShaderResource() {}
	GuiShader(ShaderSource shaderSource) : ShaderResource("GuiShader", "gui.shader", shaderSource) {}

	void init(const Mat4f& orthoMatrix);
	void setTextured(bool textured);
};

struct QuadShader : public ShaderResource {
	QuadShader() : ShaderResource() {}
	QuadShader(ShaderSource shaderSource) : ShaderResource("QuadShader", "quad.shader", shaderSource) {}

	void updateProjection(const Mat4f& orthoMatrix);
	void updateColor(const Vec4& color);
	void updateTexture(Graphics::Texture* texture);
	void updateTexture(Graphics::Texture* texture, const Vec4f& color);
};

struct BlurShader : public ShaderResource {
	BlurShader() : ShaderResource() {}
	BlurShader(ShaderSource shaderSource) : ShaderResource("BlurShader", "blur.shader", shaderSource) {}

	void updateWidth(float width);
	void updateTexture(Graphics::Texture* texture);
};

namespace GraphicsShaders {
	extern GuiShader guiShader;
	extern QuadShader quadShader;
	extern BlurShader horizontalBlurShader;
	extern BlurShader verticalBlurShader;

	void onInit();
	void onClose();
}