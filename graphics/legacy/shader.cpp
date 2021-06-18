#include "core.h"

#include <GL/glew.h>

#include "shader.h"
#include "renderer.h"
#include "debug/guiDebug.h"

#include <fstream>
#include <sstream>
#include <future>

namespace P3D::Graphics {

#pragma region uniforms

int Shader::getUniform(const std::string& uniform) const {
	auto iterator = uniforms.find(uniform);
	if (iterator != uniforms.end())
		return iterator->second;

	return -1;
}

void Shader::addUniform(const std::string& uniform) {
	if (uniforms.find(uniform) == uniforms.end())
		createUniform(uniform);
}

void Shader::createUniform(const std::string& uniform) {
	bind();
	Log::subject s(name);
	int location = id != 0 ? glGetUniformLocation(id, uniform.c_str()) : -1;

	if (location < 0)
		Log::error("Could not find uniform (%s) in shader (%s)", uniform.c_str(), name.c_str());
	else
		Log::debug("Created uniform (%s) in shader (%s) with id (%d)", uniform.c_str(), name.c_str(), location);

	uniforms.insert({ uniform, location });
}

void Shader::setUniform(const std::string& uniform, GLID value) const {
	if (id == 0)
		return;
	
	glUniform1i(uniforms.at(uniform), value);
}

void Shader::setUniform(const std::string& uniform, int value) const {
	if (id == 0)
		return;

	glUniform1i(uniforms.at(uniform), value);
}

void Shader::setUniform(const std::string& uniform, float value) const {
	if (id == 0)
		return;

	glUniform1f(uniforms.at(uniform), value);
}

void Shader::setUniform(const std::string& uniform, const Vec2f& value) const {
	if (id == 0)
		return;

	glUniform2f(uniforms.at(uniform), value.x, value.y);
}

void Shader::setUniform(const std::string& uniform, const Vec3f& value) const {
	if (id == 0)
		return;

	glUniform3f(uniforms.at(uniform), value.x, value.y, value.z);
}

void Shader::setUniform(const std::string& uniform, const Position& value) const {
	if (id == 0)
		return;

	glUniform3f(uniforms.at(uniform), float(value.x), float(value.y), float(value.z));
}

void Shader::setUniform(const std::string& uniform, const Vec4f& value) const {
	if (id == 0)
		return;

	glUniform4f(uniforms.at(uniform), value.x, value.y, value.z, value.w);
}

void Shader::setUniform(const std::string& uniform, const Mat2f& value) const {
	if (id == 0)
		return;

	float buf[4];
	value.toColMajorData(buf);
	glUniformMatrix2fv(uniforms.at(uniform), 1, GL_FALSE, buf);
}

void Shader::setUniform(const std::string& uniform, const Mat3f& value) const {
	if (id == 0)
		return;

	float buf[9];
	value.toColMajorData(buf);
	glUniformMatrix3fv(uniforms.at(uniform), 1, GL_FALSE, buf);
}

void Shader::setUniform(const std::string& uniform, const Mat4f& value) const {
	if (id == 0)
		return;

	float buf[16];
	value.toColMajorData(buf);
	glUniformMatrix4fv(uniforms.at(uniform), 1, GL_FALSE, buf);
}

#pragma endregion

#pragma region compile

GLID Shader::compile(const std::string& name, const std::string& source, unsigned int type) {
	Log::setDelimiter("");
	Log::info("%s: ", name.c_str());

	GLID id = glCreateShader(type);

	const char* src = source.c_str();

	glShaderSource(id, 1, &src, NULL);
	glCall(glCompileShader(id));

	int result; glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length; glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*) alloca(length * sizeof(char)); 
		glGetShaderInfoLog(id, length, &length, message);
		
		Log::print(Log::Color::ERROR, "fail\n");
		Log::error(message);

		glDeleteShader(id);
		Log::setDelimiter("\n");

		return 0;
	} else {
		Log::print(Log::Color::DEBUG, "done\n");
	}

	Log::setDelimiter("\n");

	return id;
}

#pragma endregion

#pragma region parse

ShaderSource parseShader(const std::string& name, const std::string& path, const std::string& shaderText) {
	std::istringstream shaderTextStream(shaderText);
	return parseShader(name, path, shaderTextStream);
}

ShaderSource parseShader(const std::string& name, const std::string& path) {
	std::ifstream input;
	input.open(path);

	if (input.is_open()) {
		std::string line;
		std::stringstream shaderTextStream;
		while (getline(input, line))
			shaderTextStream << line << "\n";
		return parseShader(name, path, shaderTextStream);
	} else
		return ShaderSource();
}

ShaderSource parseShader(const std::string& name, const std::string& path, std::istream& shaderTextStream) {
	Log::subject s(name);
	Log::info("Reading (%s)", name.c_str());

	enum class ShaderType {
		NONE = -1,
		COMMON = 0,
		VERTEX = 1,
		FRAGMENT = 2,
		GEOMETRY = 3,
		TESSELATION_CONTROL = 4,
		TESSELATION_EVALUATE = 5,
		COMPUTE = 6
	};

	ShaderType type = ShaderType::NONE;

	std::string line;
	std::stringstream stringStream[7];

	int lineNumber = 0;
	while (getline(shaderTextStream, line)) {
		lineNumber++;
		if (line == "\r") {
			continue;
		} if (line.find("[vertex]") != std::string::npos) {
			type = ShaderType::VERTEX;
		} else if (line.find("[fragment]") != std::string::npos) {
			type = ShaderType::FRAGMENT;
		} else if (line.find("[geometry]") != std::string::npos) {
			type = ShaderType::GEOMETRY;
		} else if (line.find("[tesselation control]") != std::string::npos) {
			type = ShaderType::TESSELATION_CONTROL;
		} else if (line.find("[tesselation evaluate]") != std::string::npos) {
			type = ShaderType::TESSELATION_EVALUATE;
		} else if (line.find("[compute]") != std::string::npos) {
			type = ShaderType::COMPUTE;
		} else if (line.find("[common]") != std::string::npos) {
			type = ShaderType::COMMON;
		} else {
			if (type == ShaderType::NONE) {
				Log::warn("(line %d): code before the first #shader instruction will be ignored", lineNumber);
				continue;
			}
			stringStream[(int) type] << line << "\n";
		}
	}

	std::string commonSource = stringStream[(int) ShaderType::COMMON].str();
	std::string vertexSource = stringStream[(int) ShaderType::VERTEX].str();
	std::string fragmentSource = stringStream[(int) ShaderType::FRAGMENT].str();
	std::string geometrySource = stringStream[(int) ShaderType::GEOMETRY].str();
	std::string tesselationControlSource = stringStream[(int) ShaderType::TESSELATION_CONTROL].str();
	std::string tesselationEvaluateSource = stringStream[(int) ShaderType::TESSELATION_EVALUATE].str();
	std::string computeSource = stringStream[(int) ShaderType::COMPUTE].str();

	Log::setDelimiter("");

	if (!commonSource.empty()) {
		Log::info("Common file: ");
		Log::print(Log::Color::DEBUG, "done\n");
	}

	if (!vertexSource.empty()) {
		Log::info("Vertex shader: ");
		Log::print(Log::Color::DEBUG, "done\n");
		vertexSource = commonSource + vertexSource;
	}

	if (!fragmentSource.empty()) {
		Log::info("Fragment shader: ");
		Log::print(Log::Color::DEBUG, "done\n");
		fragmentSource = commonSource + fragmentSource;
	}

	if (!geometrySource.empty()) {
		Log::info("Geometry shader: ");
		Log::print(Log::Color::DEBUG, "done\n");
		geometrySource = commonSource + geometrySource;
	}

	if (!tesselationControlSource.empty()) {
		Log::info("Tesselation control shader: ");
		Log::print(Log::Color::DEBUG, "done\n");
		tesselationControlSource = commonSource + tesselationControlSource;
	}

	if (!tesselationEvaluateSource.empty()) {
		Log::info("Tesselation evaluation shader: ");
		Log::print(Log::Color::DEBUG, "done\n");
		tesselationEvaluateSource = commonSource + tesselationEvaluateSource;
	}

	if (!computeSource.empty()) {
		Log::info("Tesselation evaluation shader: ");
		Log::print(Log::Color::DEBUG, "done\n");
		computeSource = commonSource + computeSource;
	}

	Log::setDelimiter("\n");

	return ShaderSource(name, path, vertexSource, fragmentSource, geometrySource, tesselationControlSource, tesselationEvaluateSource, computeSource);
}

#pragma endregion

#pragma region constructors

ShaderSource::ShaderSource() = default;
ShaderSource::ShaderSource(const std::string& name, const std::string& path, const std::string& vertexSource, const std::string& fragmentSource, const std::string& geometrySource, const std::string& tesselationControlSource, const std::string& tesselationEvaluateSource, const std::string& computeSource) : name(name), path(path), vertexSource(vertexSource), fragmentSource(fragmentSource), geometrySource(geometrySource), tesselationControlSource(tesselationControlSource), tesselationEvaluateSource(tesselationEvaluateSource), computeSource(computeSource) {}

Shader::Shader() = default;
Shader::Shader(const ShaderSource& shaderSource) : name(shaderSource.name) {}	

Shader::~Shader() {
	close();
}

Shader::Shader(Shader&& other) {
	id = other.id;
	other.id = 0;

	uniforms = other.uniforms;
	other.uniforms = std::unordered_map<std::string, int>();

	flags = other.flags;
	other.flags = NONE;

	name = other.name;
	other.name = std::string();
}

Shader& Shader::operator=(Shader&& other) noexcept {
	if (this != &other) {
		close();
		std::swap(id, other.id);
		name = std::move(other.name);
		uniforms = std::move(other.uniforms);
	}

	return *this;
}

#pragma endregion

#pragma region bindable

void Shader::bind() {
	Renderer::bindShader(id);
}

void Shader::unbind() {
	Renderer::bindShader(0);
}

void Shader::close() {
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

#pragma region ShaderStage

ShaderStage::ShaderStage() = default;
ShaderStage::ShaderStage(const std::string& source) : source(source) {
	this->info = Parser::parse(this->source.c_str());
}

std::vector<std::string> getSuffixes(const ShaderStage& stage, const Parser::Local& local) {
	std::vector<std::string> suffixes;

	auto strct = stage.info.structs.find(local.type.string(stage.source.c_str()));
	bool isStruct = strct != stage.info.structs.end();
	std::string localName(local.name.view(stage.source.c_str()));
	if (local.amount != 0) {
		if (isStruct) { // struct array
			for (int i = 0; i < local.amount; i++) {
				std::string variable = localName + "[" + std::to_string(i) + "].";

				for (const Parser::Local& local : strct->second.locals)
					for (const std::string& localSuffix : getSuffixes(stage, local))
						suffixes.push_back(variable + localSuffix);
			}
		} else { // normal array
			for (int i = 0; i < local.amount; i++)
				suffixes.push_back(localName + "[" + std::to_string(i) + "]");
		}
	} else {
		if (isStruct) {
			std::string variable = localName + ".";

			for (const Parser::Local& local : strct->second.locals) 
				for (const std::string& localSuffix : getSuffixes(stage, local))
					suffixes.push_back(variable + localSuffix);
		} else {
			suffixes.push_back(localName);
		}
	}

	return suffixes;
}

void Shader::addUniforms(const ShaderStage& stage) {
	for (const Parser::Local& uniform : stage.info.uniforms) {
		std::vector<std::string> variables = getSuffixes(stage, uniform);

		for (const std::string& variable : variables)
			addUniform(variable);
	}
}

#pragma endregion

};