#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "../../util/log.h"
#include "../debug.h"

#include <fstream>
#include <sstream>

void Shader::createUniform(std::string uniform) {
	bind();
	int location = glGetUniformLocation(id, uniform.c_str());
	if (location < 0)
		Log::error("Could not find uniform (%s) in shader (%s)", uniform.c_str(), name.c_str());
	else {
		Log::debug("Created uniform (%s) in shader (%s) with id (%d)", uniform.c_str(), name.c_str(), location);
		uniforms.insert(std::make_pair(uniform, location));
	}
}

void Shader::setUniform(std::string uniform, int value) {
	glUniform1i(uniforms[uniform], value);
}

void Shader::setUniform(std::string uniform, float value) {
	glUniform1f(uniforms[uniform], value);
}

void Shader::setUniform(std::string uniform, double value) {
	glUniform1d(uniforms[uniform], value);
}

void Shader::setUniform(std::string uniform, Vec2f value) {
	glUniform2d(uniforms[uniform], value.x, value.y);
}

void Shader::setUniform(std::string uniform, Vec3f value) {
	glUniform3f(uniforms[uniform], value.x, value.y, value.z);
}

void Shader::setUniform(std::string uniform, Mat4f value) {
	glUniformMatrix4fv(uniforms[uniform], 1, GL_FALSE, value.m);
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

		Log::error(message);
		
		glDeleteShader(id);
		return 0;
	}
	return id;
}

unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader, const std::string name) {
	unsigned int program = glCreateProgram();

	Log::info("Compiling vertex shader for shader (%s)", name.c_str());
	unsigned int vs = compileShader(vertexShader, GL_VERTEX_SHADER);
	Log::info("Done compiling vertex shader for shader (%s)", name.c_str());

	Log::info("Compiling fragment shader for shader (%s)", name.c_str());
	unsigned int fs = compileShader(fragmentShader, GL_FRAGMENT_SHADER);
	Log::info("Done compiling fragment shader for shader (%s)", name.c_str());

	unsigned int gs;
	if (!geometryShader.empty()) {
		Log::info("Compiling geometry shader for shader (%s)", name.c_str());
		gs = compileShader(geometryShader, GL_GEOMETRY_SHADER);
		Log::info("Done compiling geometry shader for shader (%s)", name.c_str());
	}

	glCall(glAttachShader(program, vs));
	glCall(glAttachShader(program, fs));
	if (!geometryShader.empty()) 
		glCall(glAttachShader(program, gs));

	glCall(glLinkProgram(program));
	glCall(glValidateProgram(program));

	glCall(glDeleteShader(vs));
	glCall(glDeleteShader(fs));
	if (!geometryShader.empty())
		glCall(glDeleteShader(gs));
	

	Log::debug("Created shader (%s) with id (%d)", name.c_str(), program);

	return program;
}

std::string parseFile(const std::string& path) {
	std::string line;
	std::ifstream fileStream(path);
	std::stringstream stringStream;

	if (fileStream.fail()) {
		Log::fatal("File could not be opened: (%s)", path.c_str());
	}

	while (getline(fileStream, line)) {
		stringStream << line << "\n";
	}

	fileStream.close();

	return stringStream.str();
}

ShaderSource parseShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath, const std::string name) {
	Log::info("Started parsing vertex shader: (%s), fragment shader: (%s) and geometry shader: (%s) for shader (%s)", vertexPath.c_str(), fragmentPath.c_str(), geometryPath.c_str(), name.c_str());
	std::string vertexFile = parseFile(vertexPath);
	std::string fragmentFile = parseFile(fragmentPath);
	std::string geometryFile = parseFile(geometryPath);
	Log::info("Parsed vertex shader: (%s), fragment shader: (%s) and geometry shader: (%s) for shader (%s)", vertexPath.c_str(), fragmentPath.c_str(), geometryPath.c_str(), name.c_str());
	return { vertexFile , fragmentFile , geometryPath, name };
}

ShaderSource parseShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string name) {
	Log::info("Started parsing vertex shader: (%s), and fragment shader: (%s) for shader (%s)", vertexPath.c_str(), fragmentPath.c_str(), name.c_str());
	std::string vertexFile = parseFile(vertexPath);
	std::string fragmentFile = parseFile(fragmentPath);
	Log::info("Parsed vertex shader: (%s), and fragment shader: (%s) for shader (%s)", vertexPath.c_str(), fragmentPath.c_str(), name.c_str());
	return { vertexFile , fragmentFile , "", name };
}

ShaderSource parseShader(std::istream& shaderTextStream, const std::string name) {
	Log::info("Started parsing vertex, geometry and fragment shader for shader (%s)", name.c_str());
	enum class ShaderType {
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1,
		GEOMETRY = 2
	};

	ShaderType type = ShaderType::NONE;

	std::string line;
	std::stringstream stringStream[3];

	int lineNumber = 0;
	while (getline(shaderTextStream, line)) {
		lineNumber++;
		if (line.find("#shader vertex") != std::string::npos) {
			type = ShaderType::VERTEX;
		}
		else if (line.find("#shader fragment") != std::string::npos) {
			type = ShaderType::FRAGMENT;
		}
		else if (line.find("#shader geometry") != std::string::npos) {
			type = ShaderType::GEOMETRY;
		}
		else {
			if (type == ShaderType::NONE) {
				Log::warn("(line %d): Code in (%s) before the first #shader instruction will be ignored", lineNumber, name.c_str());
				continue;
			}
			stringStream[(int)type] << line << "\n";
		}
	}

	if (stringStream[(int)ShaderType::GEOMETRY].str().empty()) {
		Log::info("Parsed vertex and fragment shader for shader (%s)", name.c_str());
		return { stringStream[(int)ShaderType::VERTEX].str(), stringStream[(int)ShaderType::FRAGMENT].str(), std::string(""), name };
	}
	else {
		Log::info("Parsed vertex, fragment and geometry shader for shader (%s)", name.c_str());
		return { stringStream[(int)ShaderType::VERTEX].str(), stringStream[(int)ShaderType::FRAGMENT].str(),  stringStream[(int)ShaderType::GEOMETRY].str(), name };
	}
}
	
unsigned int Shader::getId() {
	return id;
}

Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader, const std::string name) {
	id = createShader(vertexShader, fragmentShader, std::string(), name);
	this->name = name;
}

Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader, const std::string name) {
	id = createShader(vertexShader, fragmentShader, geometryShader, name);
	this->name = name;
}

void Shader::bind() {
	glUseProgram(id);
}

void Shader::unbind() {
	glUseProgram(0);
}

void Shader::close() {
	unbind();
	if (id != 0) {
		glDeleteProgram(id);
	}
}

