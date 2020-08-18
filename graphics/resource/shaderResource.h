#pragma once

#include "../util/resource/resourceLoader.h"
#include "../util/resource/resource.h"
#include "../shader/gshader.h"

namespace P3D::Graphics {

class ShaderResource;

class ShaderAllocator : public ResourceAllocator<ShaderResource> {
public:
	virtual ShaderResource* load(const std::string& name, const std::string& path) override;
};

class ShaderResource : public Resource, public GShader {
public:
	DEFINE_RESOURCE(GShader, "../res/shaders/basic.shader");

	ShaderResource() : Resource(""), GShader() {};
	ShaderResource(const std::string& name, const std::string& path, GShader& shader) : Resource(name, path), GShader(std::move(shader)) {}
	ShaderResource(const std::string& name, const std::string& path, const ShaderSource& shaderSource) : Resource(name, path), GShader(shaderSource) {}

	virtual void close() override {
		GShader::close();
	}

	static ShaderAllocator getAllocator() {
		return ShaderAllocator();
	}
};

};