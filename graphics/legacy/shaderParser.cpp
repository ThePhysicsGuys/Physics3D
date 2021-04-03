#include "core.h"

#include "shaderParser.h"

namespace P3D::Graphics {

std::map<std::string, ShaderVariableType> variableTypeMap = {
	{ "void", ShaderVariableType::VOID },
	{ "int", ShaderVariableType::INT },
	{ "float", ShaderVariableType::FLOAT },
	{ "mat2", ShaderVariableType::MAT2 },
	{ "mat3", ShaderVariableType::MAT3 },
	{ "mat4", ShaderVariableType::MAT4 },
	{ "vec2", ShaderVariableType::VEC2 },
	{ "vec3", ShaderVariableType::VEC3 },
	{ "vec4", ShaderVariableType::VEC4 },
	{ "sampler2D", ShaderVariableType::SAMPLER2D },
	{ "sampler3D", ShaderVariableType::SAMPLER3D },
	{ "struct", ShaderVariableType::STRUCT },
	{ "VS_OUT", ShaderVariableType::VS_OUT },
};

ShaderLocal parseLocal(TokenStack& tokens, const ShaderDefines& defines);

ShaderVariableType ShaderParser::parseVariableType(const std::string& value) {
	auto iterator = variableTypeMap.find(value);
	if (iterator != variableTypeMap.end())
		return iterator->second;

	return ShaderVariableType::NONE;
}

ShaderIOType parseIOType(const Token& token) {
	std::string value = token.value;

	if (value == "in")
		return ShaderIOType::IN;
	if (value == "out")
		return ShaderIOType::OUT;

	return ShaderIOType::NONE;
}

TokenStack nextScope(TokenStack& tokens, const TokenType::Type& ltype, const TokenType::Type& rtype) {
	int depth = -1;
	TokenStack scope;
	while (tokens.available()) {
		Token token = tokens.pop();

		if (token.type == ltype) {
			if (++depth == 0)
				continue;
		} else if (token.type == rtype) {
			if (depth == 0)
				break;
			else if (depth > 0)
				depth--;
		}

		scope.add(token);
	}

	scope.flip();
	return scope;
}

bool testFunction(TokenStack& tokens) {
	if (ShaderParser::parseVariableType(tokens.peek().value) == ShaderVariableType::VOID)
		return true;

	if (tokens.peek(1).type == TokenType::ID)
		if (tokens.peek(2).type == TokenType::LP)
			return true;

	return false;
}

int parseArray(TokenStack& tokens, const ShaderDefines& defines) {
	if (tokens.peek().type == TokenType::LB) {
		tokens.discard();
		Token next = tokens.pop();
		switch (next.type) {
			case TokenType::NUMBER: {
				return std::stoi(next.value);
				break;
			} case TokenType::ID: {
				if (defines.find(next.value) != defines.end())
					return int (defines.at(next.value));
			} case TokenType::RB: {
				return 0;
			}
		}
	}

	return -1;
}

ShaderLocals parseStructContent(TokenStack& scope, const ShaderStructs& structs, const ShaderDefines& defines) {
	ShaderLocals locals;

	while (scope.available()) {
		const Token& token = scope.peek();
		switch (token.type) {
			case TokenType::TYPE: {
				ShaderLocal local = parseLocal(scope, defines);
				locals.push_back(local);
				break;
			} case TokenType::ID: {
				if (structs.find(token.value) != structs.end()) {
					ShaderLocal local = parseLocal(scope, defines);
					locals.push_back(local);
					continue;
				}
			} default: {
				scope.discard();
			}
		}
	}

	return locals;
}

ShaderStruct parseStruct(TokenStack& tokens, const ShaderStructs& structs, const ShaderDefines& defines) {
	tokens.discard(); // pop STRUCT

	std::string name = tokens.pop().value;
	TokenStack scope = nextScope(tokens, TokenType::LC, TokenType::RC);
	ShaderLocals locals = parseStructContent(scope, structs, defines);

	return ShaderStruct(name, locals);
}

ShaderUniform parseUniform(TokenStack& tokens, const ShaderDefines& defines) {
	tokens.discard(); // pop UNIFORM

	std::string variableType = tokens.pop().value;
	std::string name = tokens.pop().value;
	int amount = parseArray(tokens, defines);
	tokens.until(TokenType::EOL);

	return ShaderUniform(name, variableType, amount != -1, amount);
}

ShaderVSOUT parseVSOUT(TokenStack& tokens, const std::string& ioType, const ShaderDefines& defines) {
	TokenStack scope = nextScope(tokens, TokenType::LC, TokenType::RC);
	ShaderLocals locals = parseStructContent(scope, ShaderStructs(), defines);
	std::string name = tokens.pop().value;
	int amount = parseArray(tokens, defines);

	return ShaderVSOUT(name, ioType, "VS_OUT", amount != -1, amount, locals);
}

ShaderLocal parseLocal(TokenStack& tokens, const ShaderDefines& defines) {
	std::string variableType = tokens.pop().value;
	std::string name = tokens.pop().value;
	int amount = parseArray(tokens, defines);

	return ShaderLocal(name, variableType, amount != -1, amount);
}

ShaderGlobal parseGlobal(TokenStack& tokens, const ShaderDefines& defines) {
	std::string ioType = tokens.pop().value;
	std::string variableType = tokens.pop().value;

	switch (ShaderParser::parseVariableType(variableType)) {
		case ShaderVariableType::VS_OUT: {
			ShaderVSOUT vsout = parseVSOUT(tokens, ioType, defines);
			return vsout;
			break;
		} default: {
			std::string name = tokens.pop().value;
			int amount = parseArray(tokens, defines);

			return ShaderGlobal(name, ioType, variableType, amount != -1, amount);
			break; 
		}
	}
}

ShaderLayoutItem parseLayoutItem(TokenStack& tokens) {
	tokens.discard();

	TokenStack scope = nextScope(tokens, TokenType::LP, TokenType::RP);

	ShaderLayoutAttributes attributes;
	while (scope.available()) {
		TokenStack setting = scope.until(TokenType::COMMA);
		std::string attribute = setting.pop().value;
		std::string value = (setting.peek().type == TokenType::ASSIGN) ? setting.pop().value : "";
		
		attributes.push_back(ShaderLayoutAttribute(attribute, value));
	}

	std::string ioType = tokens.pop().value;
	std::string variableType = "";
	std::string name = "";
	if (tokens.peek().type == TokenType::TYPE) {
		variableType = tokens.pop().value;
		name = tokens.pop().value;
	}

	return ShaderLayoutItem(attributes, ioType, variableType, name);
}

void parseDefine(TokenStack& tokens, ShaderDefines& defines) {
	std::string name = tokens.pop().value;
	float value = 1;

	if (tokens.peek().type == TokenType::NUMBER) {
		try {
			value = std::stof(tokens.pop().value);
		} catch (...) {
			return;
		}

		defines[name] = value;
	}
}

void parseUnDef(TokenStack& tokens, ShaderDefines& defines) {
	std::string name = tokens.pop().value;
	auto iterator = defines.find(name);
	if (iterator != defines.end())
		defines.erase(iterator);
}

bool parseIf(TokenStack& tokens, ShaderDefines& defines) {
	Token token = tokens.pop();

	switch (token.type) {
		case TokenType::NUMBER: {
			return std::stof(token.value);
			break;
		} case TokenType::ID: {
			if (defines.find(token.value) != defines.end())
				return defines.at(token.value);
			break;
		}
	}

	return false;
}

bool parseIfDef(TokenStack& tokens, ShaderDefines& defines) {
	Token token = tokens.pop();

	if (defines.find(token.value) != defines.end())
		return true;

	return false;
}

bool parseIfNDef(TokenStack& tokens, ShaderDefines& defines) {
	return !parseIfDef(tokens, defines);
}

void parseIfBody() {

}

ShaderVersion parseVersion(TokenStack& tokens) {
	ShaderVersion version;
	
	tokens.discard();

	if (tokens.peek().type == TokenType::NUMBER) {
		version.version = std::stoi(tokens.pop().value);
		version.version = std::clamp(version.version, 300, 450); // Todo check correctness

		if (tokens.peek().type == TokenType::ID && tokens.peek().value == "core") {
			version.core = true;
			tokens.pop();
		}
	}

	return version;
}

void parsePreprocessor(TokenStack& tokens, ShaderDefines& defines) {
	std::string preprocessor = tokens.pop().value;

	if (preprocessor == "#define") {
		parseDefine(tokens, defines);
	} else if (preprocessor == "#if") {
		bool result = parseIf(tokens, defines);
	} else if (preprocessor == "#ifdef") {
		bool result = parseIfDef(tokens, defines);
	} else if (preprocessor == "#ifndef") {
		bool result = parseIfNDef(tokens, defines);
	} else if (preprocessor == "#undef") {
		parseUnDef(tokens, defines);
	}
}

ShaderInfo ShaderParser::parse(const std::string& code) {
#ifdef NDEBUG // RELEASE
	TokenStack tokens = ShaderLexer::lex(code);
#else
	TokenStack tokens = ShaderLexer::lexDebug(code);
#endif // NDEBUG
	return parseTokens(tokens);
}

ShaderInfo ShaderParser::parseTokens(TokenStack& tokens) {
	ShaderVersion version;
	ShaderLayout layout;
	ShaderUniforms uniforms;
	ShaderGlobals globals;
	ShaderLocals locals;
	ShaderDefines defines;
	ShaderStructs structs;

	while (tokens.available()) {
		const Token& token = tokens.peek();
		switch (token.type) {
			case TokenType::UNIFORM: {
				ShaderUniform uniform = parseUniform(tokens, defines);
				uniforms.push_back(uniform);
				break; 
			} case TokenType::IO: {
				ShaderGlobal global = parseGlobal(tokens, defines);

				globals.push_back(global);
				break;
			} case TokenType::TYPE: {
				if (testFunction(tokens)) {
					nextScope(tokens, TokenType::LC, TokenType::RC);
					continue;
				}

				ShaderLocal local = parseLocal(tokens, defines);
				locals.push_back(local);
				break;
			}  case TokenType::LAYOUT: {
				ShaderLayoutItem layoutItem = parseLayoutItem(tokens);
				layout.push_back(layoutItem);
				break;
			} case TokenType::VERSION: {
				version = parseVersion(tokens);
				break;
			} case TokenType::PREP: {
				parsePreprocessor(tokens, defines);
				break;
			} case TokenType::STRUCT: {
				ShaderStruct strct = parseStruct(tokens, structs, defines);
				structs[strct.name] = strct;
				break;
			} case TokenType::ID: {
				if (structs.find(token.value) != structs.end()) {
					ShaderLocal local = parseLocal(tokens, defines);
					locals.push_back(local);
					continue;
				}
			} default: {
				tokens.discard();
			}
		}
	}

	return ShaderInfo(version, layout, uniforms, globals, locals, defines, structs);
}

};