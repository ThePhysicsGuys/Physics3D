#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Shader.h"
#include "../../util/Log.h"

#include <fstream>
#include <string>
#include <sstream>

unsigned int compileShader(const std::string& source, unsigned int type) {
	unsigned int id = glCreateShader(type);
	
	const char* src = source.c_str();
	
	glShaderSource(id, 1, &src, nullptr);
	
	glCompileShader(id);

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

unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader) {
	unsigned int program = glCreateProgram();

	Log::info("Compiling vertex shader");
	unsigned int vs = compileShader(vertexShader, GL_VERTEX_SHADER);
	Log::info("Done compiling vertex shader");

	Log::info("Compiling fragment shader");
	unsigned int fs = compileShader(fragmentShader, GL_FRAGMENT_SHADER);
	Log::info("Done compiling fragment shader");

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

ShaderSource parseShader(const std::string& vertexPath, const std::string& fragmentPath) {
	std::string line;
	std::ifstream vertexFileStream(vertexPath);
	std::ifstream fragmentFileStream(fragmentPath);
	std::stringstream vertexStringStream;
	std::stringstream fragmentStringStream;

	if (vertexFileStream.fail()) {
		Log::fatal("File could not be opened: %s", vertexPath.c_str());
	}

	if (fragmentFileStream.fail()) {
		Log::fatal("File could not be opened: %s", fragmentPath.c_str());
	}

	while (getline(vertexFileStream, line)) {
		vertexStringStream << line << "\n";
	}

	while (getline(fragmentFileStream, line)) {
		fragmentStringStream << line << "\n";
	}

	vertexFileStream.close();
	fragmentFileStream.close();

	return { vertexStringStream.str(), fragmentStringStream.str() };
}

ShaderSource parseShader(const std::string& path) {
	enum class ShaderType {
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1
	};

	ShaderType type = ShaderType::NONE;

	std::ifstream fileStream(path);
	std::string line;
	std::stringstream stringStream[2];
	
	if (fileStream.fail()) {
		Log::fatal("File could not be opened: %s", path.c_str());
	}

	while (getline(fileStream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos) {
				type = ShaderType::VERTEX;
			} else if (line.find("fragment") != std::string::npos) {
				type = ShaderType::FRAGMENT;
			}
		} else {
			if (type == ShaderType::NONE) {
				Log::warn("Code in %s before the first #shader instruction will be ignored", path.c_str());
				continue;
			}
			stringStream[(int) type] << line << "\n";
		}
	}

	fileStream.close();

	return { stringStream[(int)ShaderType::VERTEX].str(), stringStream[(int)ShaderType::FRAGMENT].str() };
}

unsigned int id;

unsigned int Shader::getId() {
	return id;
}

Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader) {
	id = createShader(vertexShader, fragmentShader);
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

