#pragma once

#include "shader.h"
#include "../graphics/resource/shaderResource.h"
#include <Physics3D/math/linalg/mat.h>

namespace P3D::Graphics {
class Texture;

struct GuiShader : public ShaderResource {
	explicit GuiShader(const std::string& source) : ShaderResource("GuiShader", source, false) {}

	void init(const Mat4f& orthoMatrix);
	void setTextured(bool textured);
};

struct QuadShader : public ShaderResource {
	explicit QuadShader(const std::string& source) : ShaderResource("QuadShader", source, false) {}

	void updateProjection(const Mat4f& projectionMatrix);
	void updateColor(const Vec4& color);
	void updateTexture(GLID texture, GLID unit);
	void updateTexture(GLID texture, GLID unit, const Vec4f& color);
};

namespace Shaders {
	
extern SRef<GuiShader> guiShader;
extern SRef<QuadShader> quadShader;

void onInit();
void onClose();
}

};