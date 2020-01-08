#pragma once

#include "shaderLexer.h"

enum class ShaderVariableType {
	NONE,
	VOID,
	INT,
	FLOAT,
	VEC2,
	VEC3,
	VEC4,
	MAT2,
	MAT3,
	MAT4,
	STRUCT,
	SAMPLER2D,
	SAMPLER3D
};

enum class ShaderIOType {
	NONE,
	IN,
	OUT
};

struct ShaderLayoutItem {
	int location;
	ShaderIOType ioType;
	ShaderVariableType variableType;
};

struct ShaderUniform {
	std::string name;
	ShaderVariableType variableType;
};

struct ShaderVSOUT : ShaderGlobal {
	std::vector<ShaderGlobal> locals;
};

struct ShaderGlobal {
	std::string name;
	bool array;
	ShaderIOType ioType;
	ShaderVariableType variableType;
};

struct ShaderLocal {
	std::string name;
	bool array;
	ShaderVariableType variableType;
};

struct ShaderStruct : ShaderLocal {
	std::vector<ShaderGlobal> locals;
};

typedef std::vector<ShaderLayoutItem> ShaderLayout;
typedef std::vector<ShaderUniform> ShaderUniforms;
typedef std::vector<ShaderGlobal> ShaderGlobals;
typedef std::vector<ShaderLocal> ShaderLocals;

struct ShaderInfo {
	ShaderLayout layout;
	ShaderUniforms uniforms;
	ShaderGlobals globals;
	ShaderLocals locals;
};

class Parser {
public:
	static ShaderInfo parse(std::vector<Token> tokens);
};