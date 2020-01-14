#include "core.h"

#include "shaderLexer.h"

std::vector<TokenType> ShaderLexer::types = {
	TokenType(TokenType::NONE, std::regex("(.*?)"), false),
	TokenType(TokenType::COMMA, std::regex(","), true),
	TokenType(TokenType::EOL, std::regex("\\;"), true),
	TokenType(TokenType::RP, std::regex("\\)"), true),
	TokenType(TokenType::LP, std::regex("\\("), true),
	TokenType(TokenType::ASSIGN, std::regex("="), true),
	TokenType(TokenType::LC, std::regex("\\{"), true),
	TokenType(TokenType::RC, std::regex("\\}"), true),
	TokenType(TokenType::LB, std::regex("\\["), true),
	TokenType(TokenType::RB, std::regex("\\]"), true),
	TokenType(TokenType::DOT, std::regex("\\."), true),
	TokenType(TokenType::VERSION, std::regex("#version"), false),
	TokenType(TokenType::PREP, std::regex("#ifdef|#ifndef|#else|#endif"), false),
	TokenType(TokenType::ID, std::regex("[A-Za-z_]\\w*"), false),
	TokenType(TokenType::OP, std::regex("\\+\\+?|--?|\\*\\*?|\\:|\\/\\/?|\\?|<=?|>=?"), false),
	TokenType(TokenType::NUMBER, std::regex("^[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?"), false),
	TokenType(TokenType::STRING, std::regex("\"((?:\\\\.|[^\\\\\"])*)\""), false),
	TokenType(TokenType::COMMENT, std::regex("\\/\\*((?:.|[^\\*\\/])*)\\*\\/|\\/[\\/]+.*"), false),
	TokenType(TokenType::TYPE, std::regex("mat2|mat3|mat4|float|int|vec2|vec3|vec4|struct|VS_OUT|sampler2D|void|sampler3D"), false),
	TokenType(TokenType::IO, std::regex("in|out"), false),
	TokenType(TokenType::UNIFORM, std::regex("uniform"), false),
	TokenType(TokenType::LAYOUT, std::regex("layout"), false),
	TokenType(TokenType::LOCATION, std::regex("location"), false),
	TokenType(TokenType::QUALIFIER, std::regex("const"), false)
};

TokenType ShaderLexer::getMatch(const std::string& input) {
	TokenType match = ShaderLexer::types[0];

	for (int i = 0; i < ShaderLexer::types.size(); i++) {
		if (std::regex_match(input, types[i].regex)) {
			match = types[i];

			if (types[i].accepting)
				break;
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
		case TokenType::COMMENT:
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
		} else {
			// update last match
			lastMatch = match;
		}
	}

	return popToken(input, lastMatch, currentToken);
}

TokenStack ShaderLexer::lex(const std::string& input) {
	TokenStack tokens;

	std::string code = trim(input);

	while (code.length() != 0) {
		Token token = nextToken(code);

		if (token.type == TokenType::COMMENT)
			continue;

		tokens.push_back(token);
	}

	return tokens;
}

void TokenStack::initIterator() {
	iterator = begin();
}

Token TokenStack::peek(size_t offset) {
	if (iterator + offset < end())
		return *(iterator + offset);
	
	return Token(ShaderLexer::types[0], "");
}

Token TokenStack::pop() {
	if (available())
		return *iterator++;

	return Token(ShaderLexer::types[0], "");
}

TokenStack TokenStack::until(const TokenType::Type& type, bool popType) {
	TokenStack content;
	while (iterator < end()) {
		if (iterator->type == type)
			break;
		content.push_back(*iterator++);
	}

	if (popType)
		pop();

	content.initIterator();

	return content;
}

bool TokenStack::available(size_t offset) {
	return iterator + offset < end();
}
