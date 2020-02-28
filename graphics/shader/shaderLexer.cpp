#include "core.h"

#include "shaderLexer.h"

std::vector<TokenType> ShaderLexer::types = {
	TokenType(TokenType::NONE, std::regex("(.*?)")),
	TokenType(TokenType::COMMA, ','),
	TokenType(TokenType::EOL, ';'),
	TokenType(TokenType::RP, ')'),
	TokenType(TokenType::LP, '('),
	TokenType(TokenType::ASSIGN, '='),
	TokenType(TokenType::LC, '{'),
	TokenType(TokenType::RC, '}'),
	TokenType(TokenType::LB, '['),
	TokenType(TokenType::RB, ']'),
	TokenType(TokenType::DOT, '.'),
	TokenType(TokenType::IO, std::regex("in|out")),
	TokenType(TokenType::LAYOUT, std::regex("layout")),
	TokenType(TokenType::UNIFORM, std::regex("uniform")),
	TokenType(TokenType::QUALIFIER, std::regex("const")),
	TokenType(TokenType::STRUCT, std::regex("struct")),
	TokenType(TokenType::VERSION, std::regex("#version")),
	TokenType(TokenType::PREP, std::regex("#ifdef|#ifndef|#else|#endif|#define|#if|#undef")),
	TokenType(TokenType::TYPE, std::regex("mat2|mat3|mat4|float|int|vec2|vec3|vec4|VS_OUT|sampler2D|void|sampler3D")),
	TokenType(TokenType::ID, std::regex("[A-Za-z_]\\w*")),
	TokenType(TokenType::OP, std::regex("\\+\\+?|--?|\\*\\*?|\\:|\\/\\/?|\\?|<=?|>=?")),
	TokenType(TokenType::NUMBER, std::regex("^[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?")),
	TokenType(TokenType::STRING, std::regex("\"((?:\\\\.|[^\\\\\"])*)\"")),
	TokenType(TokenType::SINGLECOMMENT, std::regex("\\/[\\/]+.*")),
	TokenType(TokenType::MULTICOMMENT, std::regex("\\/\\*((?:.|[^\\*\\/])*)\\*\\/"))
};

std::string lexSingleComment(const std::string& input) {
	int index = 0;
	while (input.length() != index && input.at(index) != '\n')
		index++;

	return input.substr(0, index);
}

TokenType ShaderLexer::getMatch(const std::string& input) {
	TokenType match = ShaderLexer::types[0];

	for (int i = 1; i < ShaderLexer::types.size(); i++) {
		if (types[i].accepting) {
			if (input.length() == 1 && input.at(0) == types[i].character) 
				return types[i];
			else 
				continue;
		} else {
			if (std::regex_match(input, types[i].regex))
				return types[i];
		}
	}

	return match;
}

Token ShaderLexer::popToken(std::string& input, const TokenType& type, std::string value) {
	// Throw exception if the token is not recognized
	if (type == TokenType::NONE)
		Log::error("Type not recognized for value %s", value.c_str());

	// removes the token from the input
	input.erase(0, value.length());

	// removes all leading whitespaces from the input
	input = ltrim(input);

	// remove quotes from the value if the type is a string, regex or comment
	switch (type) {
		case TokenType::SINGLECOMMENT:
			value = value.substr(2, value.length());
			break;
		case TokenType::MULTICOMMENT:
			value = value.substr(1, value.length() - 1);
		case TokenType::STRING:
			value = value.substr(1, value.length() - 1);
			break;
	}

	return Token(type, value);
}

Token ShaderLexer::nextToken(std::string& input) {
	std::string currentToken;

	TokenType lastMatch = ShaderLexer::types[0];
	for (int i = 0; i < input.length(); i++) {
		currentToken.push_back(input.at(i));

		// search for matches
		TokenType match = getMatch(currentToken);

		// No current match
		if (match == TokenType::NONE) {
			// Earlier match found
			if (lastMatch != TokenType::NONE) {
				// Remove last char
				currentToken.erase(currentToken.end() - 1);
				// break the loop
				break;
			}
			// else continue to search for a match
		} else if (match == TokenType::SINGLECOMMENT) {
			return popToken(input, match, lexSingleComment(input));
		} else {
			// update last match
			lastMatch = match;
		}
	}

	return popToken(input, lastMatch, currentToken);
}

TokenStack ShaderLexer::lexDebug(const std::string& input) {	
	std::string i = R"(
		
		
		
	)";
	std::function<std::vector<size_t>(const std::string&)> collect = [input] (const std::string& key) {
		std::vector<size_t> list;
		size_t lastIndex = 0;
		while (true) {
			size_t index = input.find(key, lastIndex);
			
			if (index == input.npos) {
				break;
			} else {
				list.push_back(index);
				lastIndex = index + key.size();
			}
		}
		return list;
	};

	std::vector<size_t> defines = collect("#define");
	std::vector<size_t> uniforms = collect("uniform");


	TokenStack tokens;
	return TokenStack();
	
	

}

TokenStack ShaderLexer::lex(const std::string& input) {
	TokenStack tokens;

	std::string code = trim(input);

	while (code.length() != 0) {
		Token token = nextToken(code);

		if (token.type == TokenType::SINGLECOMMENT || token.type == TokenType::MULTICOMMENT)
			continue;

		tokens.add(token);
	}

	tokens.flip();

	return tokens;
}

Token TokenStack::peek(size_t offset) {
	if (read)
		if (available(offset))
			return *(iterator + offset);
	
	return Token(ShaderLexer::types[0], "");
}

Token TokenStack::pop() {
	if (read) 
		if (available())
			return *iterator++;
	
	return Token(ShaderLexer::types[0], "");
}

void TokenStack::discard() {
	if (read)
		if (available())
			iterator++;
}

void TokenStack::add(const Token& token) {
	if (!read)
		stack.push_back(token);
}

void TokenStack::flip() {
	read = !read;

	if (read)
		iterator = stack.begin();
}

TokenStack TokenStack::until(const TokenType::Type& type, bool popType) {
	TokenStack content;
	
	if (read) {
		while (available()) {
			if (iterator->type == type)
				break;

			content.add(*iterator++);
		}

		if (popType)
			discard();
	}
	
	content.flip();

	return content;
}

inline bool TokenStack::available(size_t offset) {
	if (read)
		return iterator + offset < stack.end();

	return false;
}
