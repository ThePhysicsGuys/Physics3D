#pragma once

#include "shader.h"
#include "../graphics/resource/shaderResource.h"
#include "../physics/math/linalg/mat.h"

namespace Graphics {
class Texture;

struct GuiShader : public ShaderResource {
	GuiShader() : ShaderResource() {}
	GuiShader(ShaderSource shaderSource) : ShaderResource(shaderSource.name, shaderSource.path, shaderSource) {}

	void init(const Mat4f& orthoMatrix);
	void setTextured(bool textured);
};

struct QuadShader : public ShaderResource {
	QuadShader() : ShaderResource() {}
	QuadShader(ShaderSource shaderSource) : ShaderResource(shaderSource.name, shaderSource.path, shaderSource) {}

	void updateProjection(const Mat4f& orthoMatrix);
	void updateColor(const Vec4& color);
	void updateTexture(GLID texture, GLID unit);
	void updateTexture(GLID texture, GLID unit, const Vec4f& color);
};

struct BlurShader : public ShaderResource {
	BlurShader() : ShaderResource() {}
	BlurShader(ShaderSource shaderSource) : ShaderResource(shaderSource.name, shaderSource.path, shaderSource) {}

	void updateWidth(float width);
	void updateTexture(Graphics::Texture* texture);
};

namespace Shaders {
extern GuiShader guiShader;
extern QuadShader quadShader;
extern BlurShader horizontalBlurShader;
extern BlurShader verticalBlurShader;

void onInit();
void onClose();
}

};