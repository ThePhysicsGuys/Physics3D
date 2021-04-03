#pragma once

#include <string_view>
#include <unordered_map>
#include <vector>

#include "lexer.h"

namespace P3D::Graphics {

class Parser {
public:
	
	enum class Type : char {
		None,
		Void,
		Bool,
		Int,
		Float,
		Vec2,
		Vec3,
		Vec4,
		Mat2,
		Mat3,
		Mat4,
		Struct,
		VSOut,
		Sampler2D,
		Sampler3D
	};

	enum class IO {
		None,
		In,
		Out
	};

	struct Local {
		Lexer::Range type;
		Lexer::Range name;
		std::size_t amount;
	};
	typedef std::vector<Local> Locals;

	struct Global {
		IO io;
		Lexer::Range type;
		Lexer::Range name;
		std::size_t amount;
	};
	typedef std::vector<Global> Globals;

	struct Layout {
		struct Attribute {
			Lexer::Range attribute;
			Lexer::Range value;
		};
		
		IO io;
		Lexer::Range type;
		Lexer::Range name;
		std::vector<Attribute> attributes;
	};
	typedef std::vector<Layout> Layouts;

	struct VSOut {
		IO io;
		Lexer::Range type;
		Lexer::Range name;
		std::size_t amount;
		Locals locals;
	};
	typedef std::vector<VSOut> VSOuts;

	struct Version {
		std::size_t version = 0;
		bool core = false;
	};

	struct Struct {
		Lexer::Range name;
		Locals locals;
	};
	
	typedef std::unordered_map<std::string, Struct> Structs;
	typedef std::unordered_map<std::string, int> Defines;

	struct Parse {
		Version version;
		Layouts layouts;
		Locals uniforms;
		Globals globals;
		VSOuts vsOuts;
		Locals locals;
		Defines defines;
		Structs structs;
	};

	static Type parseType(std::string_view type);
	static IO parseIO(std::string_view io);
	
	static Parse parse(const char* code);
	
};
	
}
