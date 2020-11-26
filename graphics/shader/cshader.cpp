#include "core.h"

#include <GL/glew.h>

#include "cshader.h"
#include "renderer.h"
#include "debug/guiDebug.h"

#include <fstream>
#include <sstream>
#include <future>

#include "../util/stringUtil.h"
#include "../util/fileUtils.h"

namespace P3D::Graphics {

#pragma region compile

GLID CShader::createShader(const ShaderSource& shaderSource) {
	GLID program = glCreateProgram();

	Log::subject s(shaderSource.name);
	Log::info("Compiling shader (%s)", shaderSource.name.c_str());

	GLID cs = 0;

	cs = Shader::compile("Compute shader", shaderSource.computeSource, GL_VERTEX_SHADER);
	glAttachShader(program, cs);

	glCall(glLinkProgram(program));
	glCall(glValidateProgram(program));

	glDeleteShader(cs);

	Log::info("Created shader with id (%d)", program);

	return program;
}

#pragma endregion

#pragma region constructors

CShader::CShader() {};
CShader::CShader(const ShaderSource& shaderSource) : Shader(shaderSource) {
	id = createShader(shaderSource);

	std::future<ShaderStage> futureComputeStage = std::async(std::launch::async, parseShaderStage, shaderSource.computeSource);
	bool result = addShaderStage(futureComputeStage.get(), COMPUTE);
};

CShader::CShader(const std::string& name, const std::string& path, const std::string& computeSource) : CShader(ShaderSource(name, path, "", "", "", "", "", computeSource)) {}

CShader::~CShader() {
	close();
}

CShader::CShader(CShader&& other) {
	// TODO
}

CShader& CShader::operator=(CShader&& other) {
	if (this != &other) {
		close();
	}

	return *this;
}

#pragma endregion


bool CShader::addShaderStage(const ShaderStage& stage, const ShaderFlag& flag) {
	if (!stage.source.empty()) {
		addUniforms(stage);

		flags |= flag;

		switch (flag) {
			case COMPUTE:
				computeStage = stage;
				break;
			default:
				break;
		}
	}

	return true;
}

void CShader::close() {
	Shader::close();
}

void CShader::dispatch(unsigned int x, unsigned int y, unsigned int z) {
	glDispatchCompute(x, y, z);
}

void CShader::barrier(unsigned int flags) {
	glMemoryBarrier(flags);
}

};