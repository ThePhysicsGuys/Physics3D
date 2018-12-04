#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "../../util/log.h"
#include "../debug.h"

#include <fstream>
#include <sstream>

void Shader::createUniform(std::string uniform) {
	int location = glGetUniformLocation(id, uniform.c_str());
	if (location < 0)
		Log::error("Could not find uniform (%s)", uniform.c_str());
	else
		Log::debug("Created uniform [%s] with id %d", uniform.c_str(), location);
	uniforms.insert(std::make_pair(uniform, location));
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

void Shader::setUniform(std::string uniform, Vec2 value) {
	glUniform2d(uniforms[uniform], value.x, value.y);
}

void Shader::setUniform(std::string uniform, Vec3 value) {
	glUniform3d(uniforms[uniform], value.x, value.y, value.z);
}

void Shader::setUniform(std::string uniform, Mat4f value) {
	glCall(glUniformMatrix4fv(uniforms[uniform], 1, GL_FALSE, value.m));
}

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

unsigned int createShader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader) {
	unsigned int program = glCreateProgram();

	Log::info("Compiling vertex shader");
	unsigned int vs = compileShader(vertexShader, GL_VERTEX_SHADER);
	Log::info("Done compiling vertex shader");

	Log::info("Compiling fragment shader");
	unsigned int fs = compileShader(fragmentShader, GL_FRAGMENT_SHADER);
	Log::info("Done compiling fragment shader");

	unsigned int gs;
	if (!geometryShader.empty()) {
		Log::info("Compiling geometry shader");
		gs = compileShader(geometryShader, GL_GEOMETRY_SHADER);
		Log::info("Done compiling geometry shader");
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
	

	Log::debug("Created shader with id %d", program);

	return program;
}

std::string parseFile(const std::string& path) {
	std::string line;
	std::ifstream fileStream(path);
	std::stringstream stringStream;

	if (fileStream.fail()) {
		Log::fatal("File could not be opened: %s", path.c_str());
	}

	while (getline(fileStream, line)) {
		stringStream << line << "\n";
	}

	fileStream.close();

	return stringStream.str();
}

ShaderSource parseShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath) {
	Log::info("Started parsing vertex shader: (%s), fragment shader: (%s) and geometry shader: (%s)", vertexPath.c_str(), fragmentPath.c_str(), geometryPath.c_str());
	std::string vertexFile = parseFile(vertexPath);
	std::string fragmentFile = parseFile(fragmentPath);
	std::string geometryFile = parseFile(geometryPath);
	Log::info("Parsed vertex shader: (%s), fragment shader: (%s) and geometry shader: (%s)", vertexPath.c_str(), fragmentPath.c_str(), geometryPath.c_str());
	return { vertexFile , fragmentFile , geometryPath };
}

ShaderSource parseShader(const std::string& vertexPath, const std::string& fragmentPath) {
	Log::info("Started parsing vertex shader: (%s), and fragment shader: (%s)", vertexPath.c_str(), fragmentPath.c_str());
	std::string vertexFile = parseFile(vertexPath);
	std::string fragmentFile = parseFile(fragmentPath);
	Log::info("Parsed vertex shader: (%s), and fragment shader: %s)", vertexPath.c_str(), fragmentPath.c_str());
	return { vertexFile , fragmentFile , ""};
}

ShaderSource parseShader(const std::string& path) {
	Log::info("Started parsing fragment and vertex shader: (%s)", path.c_str());
	enum class ShaderType {
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1,
		GEOMETRY = 2
	};

	ShaderType type = ShaderType::NONE;

	std::ifstream fileStream(path);
	std::string line;
	std::stringstream stringStream[3];

	if (fileStream.fail()) {
		Log::fatal("File could not be opened: %s", path.c_str());
	}

	int lineNumber = 0;
	while (getline(fileStream, line)) {
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
				Log::warn("(line %d): Code in (%s) before the first #shader instruction will be ignored", lineNumber, path.c_str());
				continue;
			}
			stringStream[(int)type] << line << "\n";
		}
	}

	fileStream.close();

	if (stringStream[(int)ShaderType::GEOMETRY].str().empty()) {
		Log::info("Parsed vertex and fragment shader: (%s)", path.c_str());
		return { stringStream[(int)ShaderType::VERTEX].str(), stringStream[(int)ShaderType::FRAGMENT].str(), "" };
	}
	else {
		Log::info("Parsed vertex, fragment and geometry: (%s)", path.c_str());
		return { stringStream[(int)ShaderType::VERTEX].str(), stringStream[(int)ShaderType::FRAGMENT].str(),  stringStream[(int)ShaderType::GEOMETRY].str() };
	}
}
	

unsigned int Shader::getId() {
	return id;
}

Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader) {
	id = createShader(vertexShader, fragmentShader, "");
}

Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader) {
	id = createShader(vertexShader, fragmentShader, geometryShader);
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

