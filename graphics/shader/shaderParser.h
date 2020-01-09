#pragma once

#include <stack>
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

struct ShaderLocal {
	std::string name;
	ShaderVariableType variableType;
	bool array;

	ShaderLocal(const std::string& name, const ShaderVariableType& variableType, bool array) : name(name), variableType(variableType), array(array) {}
};
typedef std::vector<ShaderLocal> ShaderLocals;

struct ShaderGlobal {
	std::string name;
	ShaderIOType ioType;
	ShaderVariableType variableType;
	bool array;

	ShaderGlobal(const std::string& name, const ShaderIOType& ioType, const ShaderVariableType& variableType, bool array) : name(name), ioType(ioType), variableType(variableType), array(array) {};
};
typedef std::vector<ShaderGlobal> ShaderGlobals;

struct ShaderLayoutItem {
	int location;
	ShaderIOType ioType;
	ShaderVariableType variableType;
};
typedef std::vector<ShaderLayoutItem> ShaderLayout;

struct ShaderVSOUT : public ShaderGlobal {
	ShaderLocals locals;

	ShaderVSOUT(const std::string& name, const ShaderIOType& ioType, const ShaderVariableType& variableType, bool array, const ShaderLocals& locals) : ShaderGlobal(name, ioType, variableType, array), locals(locals) {}
};

struct ShaderStruct : public ShaderLocal {
	ShaderLocals locals;

	ShaderStruct(const std::string& name, const ShaderVariableType& variableType, bool array, const ShaderLocals& locals) : ShaderLocal(name, variableType, array), locals(locals) {}
};

typedef ShaderLocal ShaderUniform;
typedef std::vector<ShaderUniform> ShaderUniforms;

struct ShaderInfo {
	ShaderLayout layout;
	ShaderUniforms uniforms;
	ShaderGlobals globals;
	ShaderLocals locals;
};

class Parser {
public:
	static ShaderInfo parse(TokenStack& tokens);
};