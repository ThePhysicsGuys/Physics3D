#pragma once

#include "../util/stringUtil.h"
#include <regex>

namespace Graphics {

struct TokenType {

	enum Type : char {
		NONE,
		ID,
		OP,
		COMMA,
		DOT,
		ASSIGN,
		NUMBER,
		VERSION,
		PREP,
		STRING,
		SINGLECOMMENT,
		MULTICOMMENT,
		LP,
		RP,
		LB,
		RB,
		LC,
		RC,
		EOL,
		IO,
		STRUCT,
		LAYOUT,
		LOCATION,
		QUALIFIER,
		UNIFORM,
		TYPE,
	};

public:
	Type type;
	std::regex regex;
	char character;
	bool accepting;

	TokenType(Type type, std::regex regex) : type(type), regex(regex), character(), accepting(false) {}
	TokenType(Type type, char character) : type(type), character(character), accepting(true) {}

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

struct TokenStack {
private:
	bool read = false;

	std::vector<Token> stack;
	std::vector<Token>::iterator iterator;
public:
	void add(const Token& token);
	void addAll(const TokenStack& tokens);

	void flip();

	TokenStack until(const TokenType::Type& type, bool popType = true);
	Token peek(size_t offset = 0) const;
	Token pop();
	void discard();
	bool available(size_t offset = 0) const;
	size_t size() const;
};

class ShaderLexer {
	friend TokenStack;
private:
	static std::vector<TokenType> types;

	static Token nextToken(std::string& input);
	static TokenType getMatch(const std::string& input);
	static Token popToken(std::string& input, const TokenType& type, std::string value);

public:
	static TokenStack lex(const std::string& input);
	static TokenStack lexDebug(const std::string& input);
};

};