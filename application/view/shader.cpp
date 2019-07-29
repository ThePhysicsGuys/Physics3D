#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "../util/log.h"
#include "../debug.h"

#include <fstream>
#include <sstream>

void Shader::createUniform(const std::string& uniform) {
	bind();
	Log::setSubject(name);
	int location = glGetUniformLocation(id, uniform.c_str());
	if (location < 0)
		Log::error("Could not find uniform (%s) in shader (%s)", uniform.c_str(), name.c_str());
	else {
		Log::debug("Created uniform (%s) in shader (%s) with id (%d)", uniform.c_str(), name.c_str(), location);
	}
	uniforms.insert(std::make_pair(uniform, location));
	Log::resetSubject();
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

void Shader::setUniform(const std::string& uniform, const Vec4f& value) const {
	glUniform4f(uniforms.at(uniform), value.x, value.y, value.z, value.w);
}

void Shader::setUniform(const std::string& uniform, const Mat4f& value) const {
	glUniformMatrix4fv(uniforms.at(uniform), 1, GL_FALSE, value.m);
}

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
	if (id > 0) {
		Log::print("done\n", Log::Color::DEBUG);
	}

	Log::setDelimiter("\n");

	return id;
}

unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader, const std::string& tesselationControlShader, const std::string& tesselationEvaluateShader, const std::string& name) {
	unsigned int program = glCreateProgram();

	Log::setSubject(name);

	Log::info("Compiling shader (%s)", name.c_str());

	unsigned int vs = compileShaderWithDebug("Vertex shader", vertexShader, GL_VERTEX_SHADER);
	
	unsigned int fs = compileShaderWithDebug("Fragment shader", fragmentShader, GL_FRAGMENT_SHADER);

	unsigned int gs;
	if (!geometryShader.empty())
		gs = compileShaderWithDebug("Geometry shader",geometryShader, GL_GEOMETRY_SHADER);

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

	Log::info("Created shader with id (%d)", name.c_str(), program);

	Log::resetSubject();

	return program;
}

std::string parseFile(const std::string& path) {
	std::string line;
	std::ifstream fileStream(path);
	std::stringstream stringStream;

	Log::setDelimiter("");

	Log::info("Reading (%s) ... ", path.c_str());

	if (fileStream.fail()) {
		Log::print("Fail", Log::Color::ERROR);
	} else {
		Log::print("Done", Log::Color::DEBUG);
	}
	
	Log::setDelimiter("\n");

	while (getline(fileStream, line)) {
		stringStream << line << "\n";
	}

	fileStream.close();

	return stringStream.str();
}

ShaderSource parseShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath, const std::string& tesselationControlPath, const std::string& tesselationEvaluatePath, const std::string& name) {
	Log::setSubject(name);
	std::string vertexFile = parseFile(vertexPath);
	std::string fragmentFile = parseFile(fragmentPath);
	std::string geometryFile = parseFile(geometryPath);
	std::string tesselationControlFile = parseFile(tesselationControlPath);
	std::string tesselationEvaluateFile = parseFile(tesselationEvaluatePath);
	Log::resetSubject();
	return { vertexFile , fragmentFile , geometryFile, tesselationControlFile, tesselationEvaluateFile, name };
}

ShaderSource parseShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath, const std::string& name) {
	Log::setSubject(name);
	std::string vertexFile = parseFile(vertexPath);
	std::string fragmentFile = parseFile(fragmentPath);
	std::string geometryFile = parseFile(geometryPath);
	Log::resetSubject();
	return { vertexFile , fragmentFile , geometryFile, "", "", name };
}

ShaderSource parseShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& name) {
	Log::setSubject(name);
	std::string vertexFile = parseFile(vertexPath);
	std::string fragmentFile = parseFile(fragmentPath);
	Log::resetSubject();
	return { vertexFile , fragmentFile , "", "", "", name };
}

ShaderSource parseShader(std::istream& shaderTextStream, const std::string& name) {
	Log::setSubject(name);

	Log::info("Reading (%s)", name.c_str());

	enum class ShaderType {
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1,
		GEOMETRY = 2,
		TESSELATION_CONTROL = 3,
		TESSELATION_EVALUATE = 4
	};

	ShaderType type = ShaderType::NONE;

	std::string line;
	std::stringstream stringStream[5];

	int lineNumber = 0;
	while (getline(shaderTextStream, line)) {
		lineNumber++;
		if (line.find("#shader vertex") != std::string::npos) {
			type = ShaderType::VERTEX;
		} else if (line.find("#shader fragment") != std::string::npos) {
			type = ShaderType::FRAGMENT;
		} else if (line.find("#shader geometry") != std::string::npos) {
			type = ShaderType::GEOMETRY;} 
		else if (line.find("#shader tesselation control") != std::string::npos) {
			type = ShaderType::TESSELATION_CONTROL;} 
		else if (line.find("#shader tesselation evaluate") != std::string::npos) {
			type = ShaderType::TESSELATION_EVALUATE;
		} else {
			if (type == ShaderType::NONE) {
				Log::warn("(line %d): code before the first #shader instruction will be ignored", lineNumber);
				continue;
			}
			stringStream[(int)type] << line << "\n";
		}
	}

	std::string vertexFile = stringStream[(int)ShaderType::VERTEX].str();
	std::string fragmentFile = stringStream[(int)ShaderType::FRAGMENT].str();
	std::string geometryFile = stringStream[(int)ShaderType::GEOMETRY].str();
	std::string tesselationControlFile = stringStream[(int)ShaderType::TESSELATION_CONTROL].str();
	std::string tesselationEvaluateFile = stringStream[(int)ShaderType::TESSELATION_EVALUATE].str();
	
	Log::setDelimiter("");

	if (!vertexFile.empty()) {
		Log::info("Vertex shader: ");
		Log::print("done\n", Log::Color::DEBUG);
	}

	if (!fragmentFile.empty()) {
		Log::info("Fragment shader: ");
		Log::print("done\n", Log::Color::DEBUG);
	}

	if (!geometryFile.empty()) {
		Log::info("Geometry shader: ");
		Log::print("done\n", Log::Color::DEBUG);
	}

	if (!tesselationControlFile.empty()) {
		Log::info("Tesselation control shader: ");
		Log::print("done\n", Log::Color::DEBUG);
	}

	if (!tesselationEvaluateFile.empty()) {
		Log::info("Tesselation evaluation shader: ");
		Log::print("done\n", Log::Color::DEBUG);
	}

	Log::setDelimiter("\n");
	Log::resetSubject();

	return { vertexFile , fragmentFile , geometryFile, tesselationControlFile, tesselationEvaluateFile, name };
}

Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& name) {
	id = createShader(vertexShader, fragmentShader, "", "", "", name);
	this->name = name;
}

Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader, const std::string& name) {
	id = createShader(vertexShader, fragmentShader, geometryShader, "", "", name);
	this->name = name;
}

Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader, const std::string& tesselationControlSource, const std::string& tesselationEvaluateSource, const std::string& name) {
	id = createShader(vertexShader, fragmentShader, geometryShader, tesselationControlSource, tesselationEvaluateSource, name);
	this->name = name;
}

void Shader::bind() {
	glUseProgram(id);
}

void Shader::unbind() {
	glUseProgram(0);
}

void Shader::close() {
	if (id != 0) {
		Log::setSubject(name);
		unbind();
		Log::info("Closing shader");

		glDeleteProgram(id);
		id = 0;

		Log::info("Closed shader");
		Log::resetSubject();
	}
}

