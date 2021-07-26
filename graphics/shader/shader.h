#pragma once

#include "../bindable.h"
#include "../gui/color.h"
#include "propertiesParser.h"

namespace P3D::Graphics {

	class Shader : public Bindable {
	public:
		struct Stage {
			using ID = unsigned;
			static ID common;
			static ID properties;
			static ID vertex;
			static ID geometry;
			static ID fragment;
			static ID tesselationControl;
			static ID tesselationEvaluation;
			static ID compute;
			
			std::string_view view;
			
			explicit Stage(const std::string_view& view);
		};

		struct Source {
			std::string source;
			std::unordered_map<Stage::ID, std::pair<std::size_t, std::size_t>> views;
		};

		std::string name;
		std::string code;
		std::unordered_map<Stage::ID, Stage> stages;
		std::unordered_map<std::string, int> uniforms;
		std::vector<Property> properties;

		Shader();
		Shader(const std::string& name, const std::string& path, bool isPath);
		virtual ~Shader();

		Shader(const Shader&) = delete;
		Shader(Shader&&) noexcept = delete;
		Shader& operator=(const Shader&) = delete;
		Shader& operator=(Shader&& other) noexcept = delete;

		[[nodiscard]] static std::string getStageName(const Stage::ID& stage);
		[[nodiscard]] static GLID compile(const Source& source);
		[[nodiscard]] static GLID compile(const std::string& common, const std::string& source, const Stage::ID& stage);
		[[nodiscard]] static Source parse(std::istream& istream);
		
		void bind() override;
		void unbind() override;
		void close() override;
		
		[[nodiscard]] int getUniform(const std::string& uniform);
		void addUniform(const std::string& uniform);
		void createUniform(const std::string& uniform);
		void setUniform(const std::string& uniform, int value);
		void setUniform(const std::string& uniform, GLID value);
		void setUniform(const std::string& uniform, float value);
		void setUniform(const std::string& uniform, const Color& color);
		void setUniform(const std::string& uniform, const Vec2f& value);
		void setUniform(const std::string& uniform, const Vec3f& value);
		void setUniform(const std::string& uniform, const Vec4f& value);
		void setUniform(const std::string& uniform, const Mat2f& value);
		void setUniform(const std::string& uniform, const Mat3f& value);
		void setUniform(const std::string& uniform, const Mat4f& value);
		void setUniform(const std::string& uniform, const Position& value);
	};

	class CShader : public Shader {
	public:
		CShader(const std::string& name, const std::string& path, bool isPath);
		
		void dispatch(unsigned int x, unsigned int y, unsigned int z);
		void barrier(unsigned int flags);
	};
	
}
