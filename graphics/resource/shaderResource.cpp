#include "core.h"

#include "shaderResource.h"

#include <fstream>
#include <istream>

ShaderResource* ShaderAllocator::load(const std::string& name, const std::string& path) {
	std::ifstream ifstream(path);
	ShaderSource source = parseShader(name, ifstream);
	return new ShaderResource(name, path, source);
}
