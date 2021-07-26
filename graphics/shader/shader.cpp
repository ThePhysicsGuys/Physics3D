#include "core.h"
#include "shader.h"

#include <fstream>

#include <GL/glew.h>
#include <Physics3D/misc/toString.h>

#include "renderer.h"
#include "debug/guiDebug.h"
#include "util/stringUtil.h"
#include "util/systemVariables.h"

namespace P3D::Graphics {

	Shader::Stage::ID Shader::Stage::common = 1;
	Shader::Stage::ID Shader::Stage::properties = 2;
	Shader::Stage::ID Shader::Stage::vertex = GL_VERTEX_SHADER;
	Shader::Stage::ID Shader::Stage::fragment = GL_FRAGMENT_SHADER;
	Shader::Stage::ID Shader::Stage::geometry = GL_GEOMETRY_SHADER;
	Shader::Stage::ID Shader::Stage::tesselationControl = GL_TESS_CONTROL_SHADER;
	Shader::Stage::ID Shader::Stage::tesselationEvaluation = GL_TESS_EVALUATION_SHADER;
	Shader::Stage::ID Shader::Stage::compute = GL_COMPUTE_SHADER;

	Shader::Stage::Stage(const std::string_view& view) : view(view) {
		
	}

	Shader::Shader() = default;

	Shader::Shader(const std::string& name, const std::string& path, bool isPath) : name(name) {
		Source source;

		// Parsing
		if (isPath) {
			std::ifstream istream;
			istream.open(path);

			if (!istream.is_open()) {
				Log::error("Failed to read path [%s] to parse shader [%s]", path.c_str(), name.c_str());
				return;
			}

			source = parse(istream);
		} else {
			std::istringstream istream(path);
			source = parse(istream);
		}

		// Compilation
		this->id = compile(source);

		// Stages
		this->code = source.source;
		for (const auto& [stage, view] : source.views)
			this->stages.emplace(stage, Stage(std::string_view(this->code.data() + view.first, view.second)));

		// Properties
		auto iterator = this->stages.find(Stage::properties);
		if (iterator != this->stages.end()) {
			this->properties = PropertiesParser::parse(this->code.data(), iterator->second.view);
		}
	}

	Shader::~Shader() {
		//Todo fix error Log::info("Deleted shader [%s] with id [%u]", name.c_str(), id); 
		Shader::unbind();
		glDeleteProgram(id);
	}

	
	void Shader::bind() {
		Renderer::bindShader(id);
	}

	void Shader::unbind() {
		Renderer::bindShader(0);
	}

	void Shader::close() {
		// Todo remove
	}

	std::string Shader::getStageName(const Stage::ID& stage) {
		if (stage == Stage::common)
			return std::string("common");
		if (stage == Stage::properties)
			return std::string("properties");
		if (stage == Stage::vertex)
			return std::string("vertex");
		if (stage == Stage::fragment)
			return std::string("fragment");
		if (stage == Stage::geometry)
			return std::string("geometry");
		if (stage == Stage::tesselationControl)
			return std::string("tesselation control");
		if (stage == Stage::tesselationEvaluation)
			return std::string("tesselation evaluation");
		if (stage == Stage::compute)
			return std::string("compute");

		return std::string();
	}

	GLID Shader::compile(const Source& source) {
		// Only OpenGL > 330 is supported
		if (SystemVariables::get("OPENGL_SHADER_VERSION") < 330) {
			Log::error("Shader version [%d] not supported", SystemVariables::get("OPENGL_SHADER_VERSION"));
			return 0;
		}

		// Shader creation
		GLID program = glCreateProgram();
		if (program == 0) {
			Log::error("Compilation failed, could not create a new program");
			return 0;
		}

		// Get common source
		auto iterator = source.views.find(Stage::common);
		std::string common = iterator != source.views.end() ? source.source.substr(iterator->second.first, iterator->second.second) : std::string();

		// Compilation and attachment
		std::vector<GLID> compiledStages;
		for (const auto& [stage, view] : source.views) {
			if (stage == Stage::common || stage == Stage::properties)
				continue;

			std::string code = source.source.substr(view.first, view.second);
			GLID id = compile(common, code, stage);
			if (id == 0) {
				Log::error("Compilation failed");

				for (GLID compiledStage : compiledStages) {
					glDetachShader(program, compiledStage);
					glDeleteShader(compiledStage);
				}

				glDeleteProgram(program);

				return 0;
			}

			glAttachShader(program, id);
			compiledStages.push_back(id);
		}

		// Linking and validation
		glCall(glLinkProgram(program));
		glCall(glValidateProgram(program));

		// Deletion of stages
		for (GLID compiledStage : compiledStages)
			glDeleteShader(compiledStage);

		Log::info("Compilation of shader [%d] successful", program);

		return program;
	}

	GLID Shader::compile(const std::string& common, const std::string& source, const Stage::ID& stage) {
		GLID id = glCreateShader(stage);

		if (id == 0) {
			Log::error("Failed to create a %s stage", getStageName(stage).c_str());
			return 0;
		}

		const char* src[] = { common.c_str(), source.c_str() };
		const int sizes[] = { static_cast<const int>(common.length()), static_cast<const int>(source.length()) };
		glShaderSource(id, 2, src, sizes);
		glCompileShader(id);

		int compiled;
		glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
		if (compiled == GL_FALSE) {
			int length; glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
			char* message = static_cast<char*>(alloca(length * sizeof(char)));
			glGetShaderInfoLog(id, length, &length, message);

			Log::error("Compilation of %s stage failed", getStageName(stage).c_str());
			Log::error(message);

			glDeleteShader(id);
			return 0;
		}

		return id;
	}

	Shader::Source Shader::parse(std::istream& istream) {
		Source source;
		std::stringstream code;
		
		constexpr Stage::ID none = 0;
		Stage::ID old = none;
		Stage::ID current = none;

		std::size_t start = 0;
		std::size_t length = 0;
		
		std::string line;
		std::size_t lineNumber = 0;
		while (getline(istream, line)) {
			code << line << '\n';
			lineNumber++;
			
			std::string trimmedLine = Util::ltrim(line);
			if (trimmedLine.rfind("[vertex]", 0) != std::string::npos) {
				current = Stage::vertex;
			} else if (trimmedLine.rfind("[fragment]", 0) != std::string::npos) {
				current = Stage::fragment;
			} else if (trimmedLine.rfind("[geometry]", 0) != std::string::npos) {
				current = Stage::geometry;
			} else if (trimmedLine.rfind("[tesselation control]", 0) != std::string::npos) {
				current = Stage::tesselationControl;
			} else if (trimmedLine.rfind("[tesselation evaluate]", 0) != std::string::npos) {
				current = Stage::tesselationEvaluation;
			} else if (trimmedLine.rfind("[compute]", 0) != std::string::npos) {
				current = Stage::compute;
			} else if (trimmedLine.rfind("[common]", 0) != std::string::npos) {
				current = Stage::common;
			} else if (trimmedLine.rfind("[properties]", 0) != std::string::npos) {
				current = Stage::properties;
			}
			
			if (current == none)
				Log::warn("Code at line [%d] does not belong to a shader stage", lineNumber);

			if (current != old) {
				if (old != none) {
					std::pair<std::size_t, std::size_t> view(start, length);
					auto [iterator, inserted] = source.views.emplace(old, view);
					if (!inserted)
						Log::warn("Duplicate shader stages encountered, new stage is discarded");
				}

				start += length + line.length() + 1;
				length = 0;
				old = current;
				
				continue;
			}
			
			length += line.length() + 1;
		}

		// Last stage
		std::pair<std::size_t, std::size_t> view(start, length);
		auto [iterator, inserted] = source.views.emplace(old, view);
		if (!inserted)
			Log::warn("Duplicate shader stages encountered, new stage is discarded");

		source.source = code.str();

		return source;
	}

	int Shader::getUniform(const std::string& uniform) {
		if (id == 0) {
			Log::warn("Requesting a uniform [%s] from an empty shader [%s]", uniform.c_str(), name.c_str()); \
				return -1;
		}
		
		auto iterator = uniforms.find(uniform);
		if (iterator != uniforms.end())
			return iterator->second;

		int location = glGetUniformLocation(id, uniform.c_str());
		if (location != -1) {
			Log::debug("Created uniform [%s] with id [%d] in shader [%s] ", uniform.c_str(), location, name.c_str());
			uniforms.emplace(uniform, location);
			return location;
		}
		
		Log::warn("Uniform [%s] not found in shader [%s]", uniform.c_str(), name.c_str());
		uniforms.emplace(uniform, location);
		return -1;
	}

	void Shader::addUniform(const std::string& uniform) {
		if (uniforms.find(uniform) == uniforms.end())
			createUniform(uniform);
	}

	void Shader::createUniform(const std::string& uniform) {
		Log::subject s(name);
		
		if (id == 0) {
			Log::warn("Creating a uniform [%s] in an empty shader [%s]", uniform.c_str(), name.c_str());
			return;
		}
		
		bind();
		int location = glGetUniformLocation(id, uniform.c_str());
		
		if (location == -1) {
			Log::warn("Uniform [%s] not found in shader [%s]", uniform.c_str(), name.c_str());
			return;
		}
		
		uniforms.emplace(uniform, location);
		Log::debug("Created uniform [%s] with id [%d] in shader [%s] ", uniform.c_str(), location, name.c_str());
	}

//Log::warn("Setting an unknown uniform [%s] in a shader [%s]", uniform.c_str(), name.c_str()); 
#define UNIFORM_CHECK(uniform) \
	this->getUniform(uniform); \
	if (id == 0) { \
		Log::warn("Setting a uniform [%s] in an empty shader [%s]", uniform.c_str(), name.c_str()); \
		return; \
	} \
	if (location == -1) \
		return;
	
	void Shader::setUniform(const std::string& uniform, GLID value) {
		int location = UNIFORM_CHECK(uniform);

		glUniform1i(location, value);
	}

	void Shader::setUniform(const std::string& uniform, int value) {
		int location = UNIFORM_CHECK(uniform);
		
		glUniform1i(location, value);
	}

	void Shader::setUniform(const std::string& uniform, float value) {
		int location = UNIFORM_CHECK(uniform);
		
		glUniform1f(location, value);
	}

	void Shader::setUniform(const std::string& uniform, const Color& value) {
		int location = UNIFORM_CHECK(uniform);

		glUniform4f(location, value.r, value.g, value.b, value.a);
	}

	void Shader::setUniform(const std::string& uniform, const Vec2f& value) {
		int location = UNIFORM_CHECK(uniform);

		glUniform2f(location, value.x, value.y);
	}

	void Shader::setUniform(const std::string& uniform, const Vec3f& value) {
		int location = UNIFORM_CHECK(uniform);

		glUniform3f(location, value.x, value.y, value.z);
	}

	void Shader::setUniform(const std::string& uniform, const Position& value) {
		int location = UNIFORM_CHECK(uniform);

		glUniform3f(location, static_cast<float>(value.x), static_cast<float>(value.y), static_cast<float>(value.z));
	}

	void Shader::setUniform(const std::string& uniform, const Vec4f& value) {
		int location = UNIFORM_CHECK(uniform);
		
		glUniform4f(uniforms.at(uniform), value.x, value.y, value.z, value.w);
	}

	void Shader::setUniform(const std::string& uniform, const Mat2f& value) {
		int location = UNIFORM_CHECK(uniform);
		
		float buf[4];
		value.toColMajorData(buf);
		glUniformMatrix2fv(location, 1, GL_FALSE, buf);
	}

	void Shader::setUniform(const std::string& uniform, const Mat3f& value) {
		int location = UNIFORM_CHECK(uniform);

		float buf[9];
		value.toColMajorData(buf);
		glUniformMatrix3fv(location, 1, GL_FALSE, buf);
	}

	void Shader::setUniform(const std::string& uniform, const Mat4f& value) {
		int location = UNIFORM_CHECK(uniform);
		
		float buf[16];
		value.toColMajorData(buf);
		glUniformMatrix4fv(location, 1, GL_FALSE, buf);
	}

	CShader::CShader(const std::string& name, const std::string& path, bool isPath) : Shader(name, path, isPath) {

	}

	void CShader::dispatch(unsigned int x, unsigned int y, unsigned int z) {
		glDispatchCompute(x, y, z);
	}

	void CShader::barrier(unsigned int flags) {
		glMemoryBarrier(flags);
	}
	
}
