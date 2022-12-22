#pragma once

#include "shaderBase.h"
#include "util/stringUtil.h"

namespace P3D::Graphics::Comp {
struct Material;
}

namespace P3D::Application {
using namespace Graphics;

struct ExtendedPart;

struct BasicShader : public StandardMeshShaderBase, public BasicShaderBase {
	BasicShader() : ShaderResource("BasicShader", "../res/shaders/basic.shader"), StandardMeshShaderBase("BasicShader", "../res/shaders/basic.shader"), BasicShaderBase("BasicShader", "../res/shaders/basic.shader") {}

	void updatePart(const ExtendedPart& part);
	void updateTexture(bool textured);
	void updateMaterial(const Graphics::Comp::Material& material);
};

struct InstanceShader : public InstancedMeshShaderBase, public BasicShaderBase {
	InstanceShader() : ShaderResource("InstanceShader", "../res/shaders/instance.shader"), InstancedMeshShaderBase("InstanceShader", "../res/shaders/instance.shader"), BasicShaderBase("InstanceShader", "../res/shaders/instance.shader") {
		InstanceShader::bind();
		for (int i = 0; i < 31; i++) 
			setUniform(Util::format("textures[%d]", i), i);
	}
};

};