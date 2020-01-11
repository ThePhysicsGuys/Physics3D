#include "core.h"

#include "shaderParser.h"

ShaderLocal parseLocal(TokenStack& tokens);
ShaderGlobal parseGlobal(TokenStack& tokens);

ShaderVariableType parseVariableType(const Token& token) {
	std::string value = token.value;

	if (value == "void")
		return ShaderVariableType::VOID;
	if (value == "int")
		return ShaderVariableType::INT;
	if (value == "float")
		return ShaderVariableType::FLOAT;
	if (value == "mat2")
		return ShaderVariableType::MAT2;
	if (value == "mat3")
		return ShaderVariableType::MAT3;
	if (value == "mat4")
		return ShaderVariableType::MAT4;
	if (value == "vec2")
		return ShaderVariableType::VEC2;
	if (value == "vec3")
		return ShaderVariableType::VEC3;
	if (value == "vec4")
		return ShaderVariableType::VEC4;
	if (value == "sampler2D")
		return ShaderVariableType::SAMPLER2D;
	if (value == "sampler3D")
		return ShaderVariableType::SAMPLER3D;
	if (value == "struct")
		return ShaderVariableType::STRUCT;
	if (value == "VS_OUT")
		return ShaderVariableType::VS_OUT;

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

		scope.push_back(token);
	}

	scope.initIterator();
	return scope;
}

bool testFunction(TokenStack& tokens) {
	if (parseVariableType(tokens.peek()) == ShaderVariableType::VOID)
		return true;

	if (tokens.peek(1).type == TokenType::ID)
		if (tokens.peek(2).type == TokenType::LP)
			return true;

	return false;
}

bool testArray(TokenStack& tokens) {
	if (tokens.peek().type == TokenType::LB)
		return true;

	return false;
}

ShaderLocals parseStructContent(TokenStack& scope) {
	ShaderLocals locals;

	while (scope.available()) {
		const Token& token = scope.peek();
		switch (token.type) {
			case TokenType::TYPE: {
				ShaderLocal local = parseLocal(scope);
				locals.push_back(local);
				break;
			} default: {
				scope.pop();
			}
		}
	}

	return locals;
}

ShaderStruct parseStruct(TokenStack& tokens) {
	tokens.pop(); // pop STRUCT

	std::string name = tokens.pop().value;
	TokenStack scope = nextScope(tokens, TokenType::LC, TokenType::RC);
	ShaderLocals locals = parseStructContent(scope);

	// TODO check if array possible
	return ShaderStruct(name, ShaderVariableType::STRUCT, false, locals);
}

ShaderUniform parseUniform(TokenStack& tokens) {
	tokens.pop(); // pop UNIFORM

	ShaderVariableType variableType = parseVariableType(tokens.pop());
	std::string name = tokens.pop().value;
	bool array = testArray(tokens);
	tokens.until(TokenType::EOL);

	return ShaderUniform(name, variableType, array);
}

ShaderVSOUT parseVSOUT(TokenStack& tokens, const ShaderIOType& ioType) {
	TokenStack scope = nextScope(tokens, TokenType::LC, TokenType::RC);
	ShaderLocals locals = parseStructContent(scope);
	std::string name = tokens.pop().value;

	bool array = testArray(tokens);

	return ShaderVSOUT(name, ioType, ShaderVariableType::VS_OUT, array, locals);
}

ShaderLocal parseLocal(TokenStack& tokens) {
	if (parseVariableType(tokens.peek()) == ShaderVariableType::STRUCT)
		return parseStruct(tokens);

	ShaderVariableType variableType = parseVariableType(tokens.pop());
	std::string name = tokens.pop().value;
	bool array = testArray(tokens);

	return ShaderLocal(name, variableType, array);
}

ShaderGlobal parseGlobal(TokenStack& tokens) {
	ShaderIOType ioType = parseIOType(tokens.pop());
	ShaderVariableType variableType = parseVariableType(tokens.pop());

	switch (variableType) {
		case ShaderVariableType::VS_OUT: {
			ShaderVSOUT vsout = parseVSOUT(tokens, ioType);
			return vsout;
			break;
		} default: {
			std::string name = tokens.pop().value;
			bool array = testArray(tokens);

			return ShaderGlobal(name, ioType, variableType, array);
			break; }
	}
}

ShaderLayoutItem parseLayoutItem(TokenStack& tokens) {
	tokens.pop();

	TokenStack scope = nextScope(tokens, TokenType::LP, TokenType::RP);

	ShaderLayoutAttributes attributes;
	while (scope.available()) {
		TokenStack setting = scope.until(TokenType::COMMA);
		std::string attribute = setting.pop().value;
		std::string value = (setting.peek().type == TokenType::ASSIGN) ? setting.pop().value : "";
		
		attributes.push_back(ShaderLayoutAttribute(attribute, value));
	}

	ShaderIOType ioType = parseIOType(tokens.pop());
	
	ShaderVariableType variableType = ShaderVariableType::NONE;
	std::string name = "";
	if (tokens.peek().type == TokenType::TYPE) {
		variableType = parseVariableType(tokens.pop());
		name = tokens.pop().value;
	}

	return ShaderLayoutItem(attributes, ioType, variableType, name);
}

ShaderInfo ShaderParser::parse(TokenStack& tokens) {
	tokens.initIterator();

	ShaderLayout layout;
	ShaderUniforms uniforms;
	ShaderGlobals globals;
	ShaderLocals locals;

	while (tokens.available()) {
		const Token& token = tokens.peek();
		switch (token.type) {
			case TokenType::UNIFORM: {
				ShaderUniform uniform = parseUniform(tokens);
				uniforms.push_back(uniform);
				break; 
			} case TokenType::IO: {
				ShaderGlobal global = parseGlobal(tokens);

				globals.push_back(global);
				break;
			} case TokenType::LAYOUT: {
				ShaderLayoutItem layoutItem = parseLayoutItem(tokens);
				layout.push_back(layoutItem);
				break;
			} case TokenType::TYPE: {
				if (testFunction(tokens)) {
					nextScope(tokens, TokenType::LC, TokenType::RC);
					continue;
				}

				ShaderLocal local = parseLocal(tokens);
				locals.push_back(local);
				break;
			} default: {
				tokens.pop();
			}
		}
	}

	return ShaderInfo(layout, uniforms, globals, locals);
}
