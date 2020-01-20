#include "core.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "debug/debug.h"

#include <fstream>
#include <sstream>
#include <future>

#include "../util/stringUtil.h"

#pragma region uniforms

int Shader::getUniform(const std::string& uniform) const {
	auto iterator = uniforms.find(uniform);
	if (iterator != uniforms.end())
		return iterator->second;

	return -1;
}

void Shader::createUniform(const std::string& uniform) {
	bind();
	Log::subject s(name);
	int location = glGetUniformLocation(id, uniform.c_str());

	if (location < 0)
		Log::error("Could not find uniform (%s) in shader (%s)", uniform.c_str(), name.c_str());
	else 
		Log::debug("Created uniform (%s) in shader (%s) with id (%d)", uniform.c_str(), name.c_str(), location);
	
	uniforms.insert(std::make_pair(uniform, location));
}

void Shader::setUniform(const std::string& uniform, int value) const {
	glUniform1i(uniforms.at(uniform), value);
}

void Shader::setUniform(const std::string& uniform, float value) const {
	glUniform1f(uniforms.at(uniform), value);
}

void Shader::setUniform(const std::string& uniform, const Vec2f& value) const {
	glUniform2d(uniforms.at(uniform), value.x, value.y);
}

void Shader::setUniform(const std::string& uniform, const Vec3f& value) const {
	glUniform3f(uniforms.at(uniform), value.x, value.y, value.z);
}

void Shader::setUniform(const std::string& uniform, const Position& value) const {
	glUniform3f(uniforms.at(uniform), float(value.x), float(value.y), float(value.z));
}

void Shader::setUniform(const std::string& uniform, const Vec4f& value) const {
	glUniform4f(uniforms.at(uniform), value.x, value.y, value.z, value.w);
}

void Shader::setUniform(const std::string& uniform, const Mat4f& value) const {
	float buf[16];
	value.toColMajorData(buf);
	glUniformMatrix4fv(uniforms.at(uniform), 1, GL_FALSE, buf);
}

#pragma endregion

#pragma region compile

unsigned int compileShader(const std::string& source, unsigned int type) {
	unsigned int id = glCreateShader(type);
	
	const char* src = source.c_str();
	
	glShaderSource(id, 1, &src, nullptr);
	
	glCall(glCompileShader(id));

	// Error handling
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*) alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		Log::print("fail\n", Log::Color::ERROR);
		Log::error(message);
		
		glDeleteShader(id);
		return 0;
	}

	return id;
}

unsigned int compileShaderWithDebug(const std::string& name, const std::string& source, unsigned int type) {
	Log::setDelimiter("");
	Log::info("%s: ", name.c_str());
	
	unsigned int id = compileShader(source, type);

	if (id > 0) 
		Log::print("done\n", Log::Color::DEBUG);

	Log::setDelimiter("\n");

	return id;
}

unsigned int createShader(const std::string& name, const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader, const std::string& tesselationControlShader, const std::string& tesselationEvaluateShader) {
	unsigned int program = glCreateProgram();

	Log::subject s(name);

	Log::info("Compiling shader (%s)", name.c_str());

	unsigned int vs = compileShaderWithDebug("Vertex shader", vertexShader, GL_VERTEX_SHADER);
	unsigned int fs = compileShaderWithDebug("Fragment shader", fragmentShader, GL_FRAGMENT_SHADER);

	unsigned int gs;
	if (!geometryShader.empty())
		gs = compileShaderWithDebug("Geometry shader", geometryShader, GL_GEOMETRY_SHADER);

	unsigned int tcs;
	if (!tesselationControlShader.empty()) 
		tcs = compileShaderWithDebug("Tesselation control shader", tesselationControlShader, GL_TESS_CONTROL_SHADER);
	
	unsigned int tes;
	if (!tesselationEvaluateShader.empty())
		tes = compileShaderWithDebug("Tesselation evaluate", tesselationEvaluateShader, GL_TESS_EVALUATION_SHADER);

	glCall(glAttachShader(program, vs));
	glCall(glAttachShader(program, fs));
	if (!geometryShader.empty()) 
		glCall(glAttachShader(program, gs));
	if (!tesselationControlShader.empty())
		glCall(glAttachShader(program, tcs));
	if (!tesselationEvaluateShader.empty())
		glCall(glAttachShader(program, tes));

	glCall(glLinkProgram(program));
	glCall(glValidateProgram(program));

	glCall(glDeleteShader(vs));
	glCall(glDeleteShader(fs));
	if (!geometryShader.empty())
		glCall(glDeleteShader(gs));
	if (!tesselationControlShader.empty())
		glCall(glDeleteShader(tcs));
	if (!tesselationEvaluateShader.empty())
		glCall(glDeleteShader(tes));

	Log::info("Created shader with id (%d)", name.c_str(), (int) program);

	return program;
}

#pragma endregion

#pragma region parse

std::string parseFile(const std::string& path) {
	std::string line;
	std::ifstream fileStream(path);
	std::stringstream stringStream;

	Log::setDelimiter("");

	Log::info("Reading (%s) ... ", path.c_str());

	if (fileStream.fail()) 
		Log::print("Fail", Log::Color::ERROR);
	else 
		Log::print("Done", Log::Color::DEBUG);
	
	Log::setDelimiter("\n");

	while (getline(fileStream, line)) 
		stringStream << line << "\n";

	fileStream.close();

	return stringStream.str();
}

ShaderSource parseShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath, const std::string& tesselationControlPath, const std::string& tesselationEvaluatePath) {
	Log::subject s(name);

	std::string vertexFile = parseFile(vertexPath);
	std::string fragmentFile = parseFile(fragmentPath);
	std::string geometryFile = parseFile(geometryPath);
	std::string tesselationControlFile = parseFile(tesselationControlPath);
	std::string tesselationEvaluateFile = parseFile(tesselationEvaluatePath);

	return ShaderSource(name, vertexFile , fragmentFile , geometryFile, tesselationControlFile, tesselationEvaluateFile);
}

ShaderSource parseShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath) {
	Log::subject s(name);

	std::string vertexFile = parseFile(vertexPath);
	std::string fragmentFile = parseFile(fragmentPath);
	std::string geometryFile = parseFile(geometryPath);

	return ShaderSource(name, vertexFile , fragmentFile , geometryFile, "", "");
}

ShaderSource parseShader(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath) {
	Log::subject s(name);

	std::string vertexFile = parseFile(vertexPath);
	std::string fragmentFile = parseFile(fragmentPath);

	return ShaderSource(name, vertexFile , fragmentFile , "", "", "");
}

ShaderSource parseShader(const std::string& name, std::istream& shaderTextStream) {
	Log::subject s(name);

	Log::info("Reading (%s)", name.c_str());

	enum class ShaderType {
		NONE = -1,
		COMMON = 0,
		VERTEX = 1,
		FRAGMENT = 2,
		GEOMETRY = 3,
		TESSELATION_CONTROL = 4,
		TESSELATION_EVALUATE = 5
	};

	ShaderType type = ShaderType::NONE;

	std::string line;
	std::stringstream stringStream[6];

	int lineNumber = 0;
	while (getline(shaderTextStream, line)) {
		lineNumber++;
		if (line.find("[vertex]") != std::string::npos) {
			type = ShaderType::VERTEX;
		} else if (line.find("[fragment]") != std::string::npos) {
			type = ShaderType::FRAGMENT;
		} else if (line.find("[geometry]") != std::string::npos) {
			type = ShaderType::GEOMETRY;
		} else if (line.find("[tesselation control]") != std::string::npos) {
			type = ShaderType::TESSELATION_CONTROL;
		} else if (line.find("[tesselation evaluate]") != std::string::npos) {
			type = ShaderType::TESSELATION_EVALUATE;
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
	
	Log::setDelimiter("");

	if (!commonSource.empty()) {
		Log::info("Common file: ");
		Log::print("done\n", Log::Color::DEBUG);
	}

	if (!vertexSource.empty()) {
		Log::info("Vertex shader: ");
		Log::print("done\n", Log::Color::DEBUG);
		vertexSource = commonSource + vertexSource;
	}

	if (!fragmentSource.empty()) {
		Log::info("Fragment shader: ");
		Log::print("done\n", Log::Color::DEBUG);
		fragmentSource = commonSource + fragmentSource;
	}

	if (!geometrySource.empty()) {
		Log::info("Geometry shader: ");
		Log::print("done\n", Log::Color::DEBUG);
		geometrySource = commonSource + geometrySource;
	}

	if (!tesselationControlSource.empty()) {
		Log::info("Tesselation control shader: ");
		Log::print("done\n", Log::Color::DEBUG);
		tesselationControlSource = commonSource + tesselationControlSource;
	}

	if (!tesselationEvaluateSource.empty()) {
		Log::info("Tesselation evaluation shader: ");
		Log::print("done\n", Log::Color::DEBUG);
		tesselationEvaluateSource = commonSource + tesselationEvaluateSource;
	}

	Log::setDelimiter("\n");

	return ShaderSource(name, vertexSource, fragmentSource, geometrySource, tesselationControlSource, tesselationEvaluateSource);
}

void Shader::addUniform(const std::string& uniform) {
	if (uniforms.find(uniform) == uniforms.end())
		createUniform(uniform);
}

std::vector<std::string> getSuffixes(const ShaderStage& stage, const ShaderLocal& local) {
	std::vector<std::string> suffixes;

	auto strct = stage.info.structs.find(local.variableType);
	bool isStruct = strct != stage.info.structs.end();
	bool isArray = local.array;

	if (isArray) {
		if (isStruct) { // struct array
			for (int i = 0; i < local.amount; i++) {
				std::string variable = local.name + "[" + std::to_string(i) + "].";

				for (const ShaderLocal& local : strct->second.locals) {
					std::vector<std::string> localSuffixes = getSuffixes(stage, local);

					for (std::string localSuffix : localSuffixes) {
						suffixes.push_back(variable + localSuffix);
					}
				}	
			}
		} else { // normal array
			for (int i = 0; i < local.amount; i++) {
				suffixes.push_back(local.name + "[" + std::to_string(i) + "]");
			}
		}
	} else {
		if (isStruct) {
			std::string variable = local.name + ".";

			for (const ShaderLocal& local : strct->second.locals) {
				std::vector<std::string> localSuffixes = getSuffixes(stage, local);

				for (std::string localSuffix : localSuffixes) {
					suffixes.push_back(variable + localSuffix);
				}
			}
		} else {
			suffixes.push_back(local.name);
		}
	}

	return suffixes;
}

void Shader::addUniforms(const ShaderStage& stage) {
	for (const ShaderUniform& uniform : stage.info.uniforms) {
		std::vector<std::string> variables = getSuffixes(stage, uniform);

		for (const std::string& variable : variables)
			addUniform(variable);
	}
}

void Shader::addShaderStage(const ShaderStage& stage, const ShaderFlag& flag) {
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
}

ShaderStage parseShaderStage(const std::string& source) {
	if (!source.empty()) {
		ShaderInfo info = ShaderParser::parse(source);
		return ShaderStage(source, info);
	}
	
	return ShaderStage();
}

#pragma endregion

#pragma region constructors

ShaderStage::ShaderStage() {};
ShaderStage::ShaderStage(const std::string& source, const ShaderInfo& info) : source(source), info(info) {};

ShaderSource::ShaderSource() {};
ShaderSource::ShaderSource(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource, const std::string& geometrySource, const std::string& tesselationControlSource, const std::string& tesselationEvaluateSource) : name(name), vertexSource(vertexSource), fragmentSource(fragmentSource), geometrySource(geometrySource), tesselationControlSource(tesselationControlSource), tesselationEvaluateSource(tesselationEvaluateSource) {}

Shader::Shader() {};
Shader::Shader(const std::string& name, const std::string& vertexShader, const std::string& fragmentShader) : Shader(name, vertexShader, fragmentShader, "", "", "") {};
Shader::Shader(const std::string& name, const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader) : Shader(name, vertexShader, fragmentShader, geometryShader, "", "") {};
Shader::Shader(const ShaderSource& shaderSource) : Shader(shaderSource.name, shaderSource.vertexSource, shaderSource.fragmentSource, shaderSource.geometrySource, shaderSource.tesselationControlSource, shaderSource.tesselationEvaluateSource) {};

Shader::Shader(const std::string& name, const std::string& vertexSource, const std::string& fragmentSource, const std::string& geometrySource, const std::string& tesselationControlSource, const std::string& tesselationEvaluateSource) : name(name) {
	id = createShader(name, vertexSource, fragmentSource, geometrySource, tesselationControlSource, tesselationEvaluateSource);

	std::future<ShaderStage> futureVertexStage = std::async(std::launch::async, parseShaderStage, vertexSource);
	std::future<ShaderStage> futureFragmentStage = std::async(std::launch::async, parseShaderStage, fragmentSource);
	std::future<ShaderStage> futureGeometryStage = std::async(std::launch::async, parseShaderStage, geometrySource);
	std::future<ShaderStage> futureTesselationControlStage = std::async(std::launch::async, parseShaderStage, tesselationControlSource);
	std::future<ShaderStage> futureTesselationEvaluationStage = std::async(std::launch::async, parseShaderStage, tesselationEvaluateSource);

	addShaderStage(futureVertexStage.get(), VERTEX);
	addShaderStage(futureFragmentStage.get(), FRAGMENT);
	addShaderStage(futureGeometryStage.get(), GEOMETRY);
	addShaderStage(futureTesselationControlStage.get(), TESSELATION_CONTROL);
	addShaderStage(futureTesselationEvaluationStage.get(), TESSELATION_EVALUATE);
}

Shader::~Shader() {
	close();
}

Shader::Shader(Shader&& other) {
	id = other.id;
	other.id = 0;
}

Shader& Shader::operator=(Shader&& other) {
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
	glUseProgram(id);
}

void Shader::unbind() {
	glUseProgram(0);
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