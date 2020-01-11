#include "core.h"

#include "shaderLexer.h"

std::vector<TokenType> ShaderLexer::types = {
	TokenType(TokenType::NONE, std::regex("(.*?)")),
	TokenType(TokenType::ID, std::regex("[A-Za-z_]\\w*")),
	TokenType(TokenType::ASSIGN, std::regex("=")),
	TokenType(TokenType::COMMA, std::regex(",")),
	TokenType(TokenType::DOT, std::regex("\\.")),
	TokenType(TokenType::OP, std::regex("\\+\\+?|--?|\\*\\*?|\\:|\\/\\/?|\\?|<=?|>=?")),
	TokenType(TokenType::NUMBER, std::regex("^[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?")),
	TokenType(TokenType::VERSION, std::regex("#version")),
	TokenType(TokenType::PREP, std::regex("#ifdef|#ifndef|#else|#endif")),
	TokenType(TokenType::STRING, std::regex("\"((?:\\\\.|[^\\\\\"])*)\"")),
	TokenType(TokenType::COMMENT, std::regex("\\/\\*((?:.|[^\\*\\/])*)\\*\\/|\\/[\\/]+.*")),
	TokenType(TokenType::LP, std::regex("\\(")),
	TokenType(TokenType::RP, std::regex("\\)")),
	TokenType(TokenType::LB, std::regex("\\[")),
	TokenType(TokenType::RB, std::regex("\\]")),
	TokenType(TokenType::LC, std::regex("\\{")),
	TokenType(TokenType::RC, std::regex("\\}")),
	TokenType(TokenType::EOL, std::regex("\\;")),
	TokenType(TokenType::IO, std::regex("in|out")),
	TokenType(TokenType::LAYOUT, std::regex("layout")),
	TokenType(TokenType::LOCATION, std::regex("location")),
	TokenType(TokenType::QUALIFIER, std::regex("const")),
	TokenType(TokenType::UNIFORM, std::regex("uniform")),
	TokenType(TokenType::TYPE, std::regex("mat2|mat3|mat4|float|int|vec2|vec3|vec4|struct|VS_OUT|sampler2D|void|sampler3D"))
};

TokenType ShaderLexer::getMatch(const std::string& input) {
	TokenType match = ShaderLexer::types[0];

	for (int i = 0; i < ShaderLexer::types.size(); i++) {
		if (std::regex_match(input, types[i].regex))
			match = types[i];
	}

	return match;
}

Token ShaderLexer::popToken(std::string& input, TokenType type, std::string value) {
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
		currentToken.append(std::string(1, input.at(i)));

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

TokenStack ShaderLexer::lex(std::string input) {
	TokenStack tokens;

	input = trim(input);

	while (input.length() != 0) {
		Token token = nextToken(input);

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
