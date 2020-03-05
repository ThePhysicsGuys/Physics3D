#pragma once

#include "../util/resource/resourceLoader.h"
#include "../util/resource/resource.h"
#include "../shader/shader.h"

namespace Graphics {

class ShaderResource;

class ShaderAllocator : public ResourceAllocator<ShaderResource> {
public:
	virtual ShaderResource* load(const std::string& name, const std::string& path) override;
};

class ShaderResource : public Resource, public Shader {
public:
	DEFINE_RESOURCE(Shader, "../res/shaders/basic.shader");

	ShaderResource() : Resource(""), Shader() {};
	ShaderResource(const std::string& name, const std::string& path, Shader& shader) : Resource(name, path), Shader(std::move(shader)) {}
	ShaderResource(const std::string& name, const std::string& path, const ShaderSource& shaderSource) : Resource(name, path), Shader(shaderSource) {}

	virtual void close() override {
		Shader::close();
	}

	static ShaderAllocator getAllocator() {
		return ShaderAllocator();
	}
};

};