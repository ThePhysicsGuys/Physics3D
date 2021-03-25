#include "core.h"

#include <GL/glew.h>

#include "gshader.h"
#include "renderer.h"
#include "debug/guiDebug.h"

#include <fstream>
#include <sstream>
#include <future>

#include "../../util/systemVariables.h"
#include "../util/fileUtils.h"

namespace P3D::Graphics {

#pragma region compile

GLID GShader::createShader(const ShaderSource& shaderSource) {
	if (SystemVariables::get("OPENGL_SHADER_VERSION") < 330) {
		Log::print(Log::Color::ERROR, "shader version not supported\n");
		Log::setDelimiter("\n");
		return 0;
	}
	
	GLID program = glCreateProgram();

	Log::subject s(shaderSource.name);
	Log::info("Compiling shader (%s)", shaderSource.name.c_str());

	GLID vs = 0;
	GLID fs = 0;
	GLID gs = 0;
	GLID tcs = 0;
	GLID tes = 0;

	vs = Shader::compile("Vertex shader", shaderSource.vertexSource, GL_VERTEX_SHADER);
	glAttachShader(program, vs);

	fs = Shader::compile("Fragment shader", shaderSource.fragmentSource, GL_FRAGMENT_SHADER);
	glAttachShader(program, fs);

	if (!shaderSource.geometrySource.empty()) {
		gs = Shader::compile("Geometry shader", shaderSource.geometrySource, GL_GEOMETRY_SHADER);
		glAttachShader(program, gs);
	}

	if (!shaderSource.tesselationControlSource.empty()) {
		tcs = Shader::compile("Tesselation control shader", shaderSource.tesselationControlSource, GL_TESS_CONTROL_SHADER);
		glAttachShader(program, tcs);
	}

	if (!shaderSource.tesselationEvaluateSource.empty()) {
		tes = Shader::compile("Tesselation evaluate", shaderSource.tesselationEvaluateSource, GL_TESS_EVALUATION_SHADER);
		glAttachShader(program, tes);
	}

	glCall(glLinkProgram(program));
	glCall(glValidateProgram(program));

	glDeleteShader(vs);
	glDeleteShader(fs);
	if (!shaderSource.geometrySource.empty())
		glDeleteShader(gs);
	if (!shaderSource.tesselationControlSource.empty())
		glDeleteShader(tcs);
	if (!shaderSource.tesselationEvaluateSource.empty())
		glDeleteShader(tes);

	Log::info("Created shader with id (%d)", program);

	return program;
}

void GShader::reload(const ShaderSource& shaderSource) {
	Log::subject s(name);
	Log::info("Reloading shader (%s)", name.c_str());

	GShader shader(shaderSource);
	if (shader.id == 0) {
		Log::error("Reloading failed");
		return;
	}

	unbind();
	glDeleteProgram(id);

	this->id = shader.id;
	shader.id = 0; // Avoid deletion
	this->uniforms = shader.uniforms;
	this->vertexStage = shader.vertexStage;
	this->fragmentStage = shader.fragmentStage;
	this->geometryStage = shader.geometryStage;
	this->tesselationControlStage = shader.tesselationControlStage;
	this->tesselationEvaluationStage = shader.tesselationEvaluationStage;
	this->flags = shader.flags;
	this->name = shader.name;

	Log::info("Reloading succesful");
}

#pragma endregion

#pragma region parse

ShaderSource parseShader(const std::string& name, const std::string& path, const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath, const std::string& tesselationControlPath, const std::string& tesselationEvaluatePath) {
	Log::subject s(name);

	std::string vertexFile = Util::parseFile(vertexPath);
	std::string fragmentFile = Util::parseFile(fragmentPath);
	std::string geometryFile = Util::parseFile(geometryPath);
	std::string tesselationControlFile = Util::parseFile(tesselationControlPath);
	std::string tesselationEvaluateFile = Util::parseFile(tesselationEvaluatePath);

	return ShaderSource(name, path, vertexFile, fragmentFile, geometryFile, tesselationControlFile, tesselationEvaluateFile, "");
}

bool GShader::addShaderStage(const std::string& source, const ShaderFlag& flag) {
	ShaderStage stage(source);
	
	if (SystemVariables::get("OPENGL_SHADER_VERSION") < stage.info.version.version) {
		Log::error("Shader version %d %snot supported for shader %s", stage.info.version.version, stage.info.version.core ? "core " : "", name.c_str());
		return false;
	}
	
	if (!stage.source.empty()) {
		addUniforms(stage);

		flags |= flag;

		switch (flag) {
			case VERTEX:
				vertexStage = stage;
				break;
			case FRAGMENT:
				fragmentStage = stage;
				break;
			case GEOMETRY:
				geometryStage = stage;
				break;
			case TESSELATION_CONTROL:
				tesselationControlStage = stage;
				break;
			case TESSELATION_EVALUATE:
				tesselationEvaluationStage = stage;
				break;
			default:
				break;
		}
	}

	return true;
}

#pragma endregion

#pragma region constructors

GShader::GShader() {};
GShader::GShader(const ShaderSource& shaderSource) : Shader(shaderSource) {
	id = createShader(shaderSource);

	bool succes = true;
	succes &= addShaderStage(shaderSource.vertexSource, VERTEX);
	succes &= addShaderStage(shaderSource.fragmentSource, FRAGMENT);
	succes &= addShaderStage(shaderSource.geometrySource, GEOMETRY);
	succes &= addShaderStage(shaderSource.tesselationControlSource, TESSELATION_CONTROL);
	succes &= addShaderStage(shaderSource.tesselationEvaluateSource, TESSELATION_EVALUATE);

	if (!succes) {
		Log::error("Error during shader stage initialization, closing shader");
		close();
	}
};

GShader::GShader(const std::string& name, const std::string& path, const std::string& vertexSource, const std::string& fragmentSource, const std::string& geometrySource, const std::string& tesselationControlSource, const std::string& tesselationEvaluateSource) : GShader(ShaderSource(name, path, vertexSource, fragmentSource, geometrySource, tesselationControlSource, tesselationEvaluateSource, "")) {}

GShader::~GShader() {
	close();
}

GShader::GShader(GShader&& other) {
	vertexStage = other.vertexStage;
	other.vertexStage = ShaderStage();

	fragmentStage = other.fragmentStage;
	other.fragmentStage = ShaderStage();

	geometryStage = other.geometryStage;
	other.geometryStage = ShaderStage();

	tesselationControlStage = other.tesselationControlStage;
	other.tesselationControlStage = ShaderStage();

	tesselationEvaluationStage = other.tesselationEvaluationStage;
	other.tesselationEvaluationStage = ShaderStage();
}

GShader& GShader::operator=(GShader&& other) {
	if (this != &other) {
		close();
	}

	return *this;
}

#pragma endregion

#pragma region bindable

void GShader::close() {
	if (id != 0) {
		Log::subject s(name);
		unbind();
		Log::info("Closing shader");

		glDeleteProgram(id);
		id = 0;

		Log::info("Closed shader");
	}
}

#pragma endregion

};