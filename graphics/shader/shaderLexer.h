#pragma once

#include "../util/stringUtil.h"
#include <regex>

struct TokenType {

	enum Type : char {
		NONE,
		ID,
		OP,
		COMMA,
		ASSIGN,
		NUMBER,
		VERSION,
		STRING,
		COMMENT,
		LP,
		RP,
		LB,
		RB,
		LC,
		RC,
		EOL,
		IO,
		LAYOUT,
		LOCATION,
		QUALIFIER,
		UNIFORM,
		TYPE,
		EOC
	};

public:
	Type type;
	std::regex regex;

	TokenType(Type type, std::regex regex) : type(type), regex(regex) {}

	operator Type() const { return type; }
	bool operator==(Type other) const { return type == other; }
	bool operator==(TokenType other) const { return type == other.type; }
	bool operator!=(Type other) const { return type != other; }
	bool operator!=(TokenType other) const { return type != other.type; }
};

struct Token {
	TokenType type;
	std::string value;

	Token(const TokenType& type, const std::string& value) : type(type), value(value) {}
};


struct TokenStack : public std::vector<Token> {
private:
	std::vector<Token>::iterator iterator;

public:
	void initIterator();

	Token peek(size_t offset = 0);
	Token pop();
	void popUntil(TokenType type);

	bool available(size_t offset = 0);
};

class Lexer {
	friend TokenStack;
private:
	static std::vector<TokenType> types;

	static Token nextToken(std::string& input);
	static TokenType getMatch(const std::string& input);
	static Token popToken(std::string& input, TokenType type, std::string value);

public:
	static TokenStack lex(std::string input);
};
