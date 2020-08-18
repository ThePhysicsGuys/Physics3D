#include "core.h"

#include "shaderResource.h"

#include <fstream>
#include <istream>

namespace P3D::Graphics {

ShaderResource* ShaderAllocator::load(const std::string& name, const std::string& path) {
	std::ifstream ifstream(path);
	ShaderSource source = parseShader(name, path, ifstream);
	return new ShaderResource(name, path, source);
}

};