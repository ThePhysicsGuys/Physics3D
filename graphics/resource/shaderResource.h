#pragma once

#include "../util/resource/resource.h"
#include "../shader/shader.h"

namespace P3D::Graphics {

class ShaderResource;

class ShaderAllocator : public ResourceAllocator<ShaderResource> {
public:
	virtual ShaderResource* load(const std::string& name, const std::string& path) override;
};

class ShaderResource : public Resource, public Shader {
public:
	DEFINE_RESOURCE(Shader, "../res/shaders/basic.shader");

	ShaderResource() : Resource(""), Shader() {}
	ShaderResource(const std::string& name, const std::string& path, bool isPath = true) : Resource(name, path), Shader(name, path, isPath) {}

	void close() override {
		Shader::close();
	}

	static ShaderAllocator getAllocator() {
		return ShaderAllocator();
	}
};

};