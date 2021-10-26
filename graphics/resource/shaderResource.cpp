#include "core.h"

#include "shaderResource.h"

#include <fstream>
#include <istream>

namespace P3D::Graphics {

ShaderResource* ShaderAllocator::load(const std::string& name, const std::string& path) {;
	return new ShaderResource(name, path, true);
}

};