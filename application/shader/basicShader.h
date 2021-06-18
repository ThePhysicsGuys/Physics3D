#pragma once

#include "shaderBase.h"

namespace P3D::Application {
using namespace Graphics;

namespace Comp {
struct Material;
};

struct ExtendedPart;

struct BasicShader : public StandardMeshShaderBase, public BasicShaderBase {
	BasicShader() : ShaderResource("BasicShader", "../res/shaders/basic.shader"), StandardMeshShaderBase("BasicShader", "../res/shaders/basic.shader"), BasicShaderBase("BasicShader", "../res/shaders/basic.shader") {}

	void updatePart(const ExtendedPart& part);
	void updateTexture(bool textured);
	void updateMaterial(const Comp::Material& material);
};

struct InstanceShader : public InstancedMeshShaderBase, public BasicShaderBase {
	InstanceShader() : ShaderResource("InstanceShader", "../res/shaders/instance.shader"), InstancedMeshShaderBase("InstanceShader", "../res/shaders/instance.shader"), BasicShaderBase("InstanceShader", "../res/shaders/instance.shader") {}

	void updateTexture(bool textured);
};

};