#include "core.h"
#include "propertiesParser.h"

#include "lexer.h"

namespace P3D::Graphics {

	std::optional<Property::Type> parseType(const Lexer& lexer, const Lexer::Token& token) {
		std::string_view type = token.view(lexer.code);

		static std::unordered_map<std::string_view, Property::Type> map { {
			{ "bool", Property::Type::Bool },
			{ "int", Property::Type::Int },
			{ "float", Property::Type::Float },
			{ "vec2", Property::Type::Vec2 },
			{ "vec3", Property::Type::Vec3 },
			{ "vec4", Property::Type::Vec4 },
			{ "mat2", Property::Type::Mat2 },
			{ "mat3", Property::Type::Mat3 },
			{ "mat4", Property::Type::Mat4 },
			{ "col3", Property::Type::Col3 },
			{ "col4", Property::Type::Col4 },
		} };
		
		auto iterator = map.find(type);
		if (iterator != map.end())
			return std::make_optional(iterator->second);

		return std::nullopt;
	}

	std::size_t arity(const Property::Type& type) {
		static std::unordered_map<Property::Type, std::size_t> map { {
			{ Property::Type::Bool, 1 },
			{ Property::Type::Int, 1 },
			{ Property::Type::Float, 1 },
			{ Property::Type::Vec2, 2 },
			{ Property::Type::Vec3, 3 },
			{ Property::Type::Vec4, 4 },
			{ Property::Type::Mat2, 2 },
			{ Property::Type::Mat3, 3 },
			{ Property::Type::Mat4, 4 },
			{ Property::Type::Col3, 3 },
			{ Property::Type::Col4, 4 },
		} };

		return map.at(type);
	}

	std::optional<Property::Default> parseDefault(Lexer& lexer, const Lexer::Token& token) {
		Property::Default value;

		if (token.type == Lexer::Token::Boolean)
			value.value = token.view(lexer.code) == "true" ? 1.0f : 0.0f;
		else if (token.type == Lexer::Token::Number) {
			float number = std::stof(token.string(lexer.code));
			value.value = number;
		} else
			return std::nullopt;

		if (lexer.peek().type != Lexer::Token::LeftParenthesis)
			return value;

		Lexer::Token parenthesis = lexer.next();

		Lexer::Token start = lexer.next();
		if (start.type != Lexer::Token::Number)
			return std::nullopt;

		if (lexer.next().type != Lexer::Token::Colon)
			return std::nullopt;

		Lexer::Token stop = lexer.next();
		if (start.type != Lexer::Token::Number)
			return std::nullopt;

		value.range = { std::stof(start.string(lexer.code)), std::stof(stop.string(lexer.code)) };

		parenthesis = lexer.next();
		if (parenthesis.type != Lexer::Token::RightParenthesis)
			return std::nullopt;

		return std::make_optional(value);
	}

	std::optional<Property> parseProperty(Lexer& lexer, const Lexer::Token& token) {
		Property property;
		
		if (token.type != Lexer::Token::Datatype)
			return std::nullopt;

		std::optional<Property::Type> type = parseType(lexer, token);
		if (!type.has_value())
			return std::nullopt;
		
		property.type = type.value();

		Lexer::Token name = lexer.next();
		if (name.type != Lexer::Token::Identifier)
			return std::nullopt;
		
		property.name = name.view(lexer.code);

		Lexer::Token next = lexer.next();
		if (next.type == Lexer::Token::LeftParenthesis) {
			Lexer::Token uniform = lexer.next();

			if (uniform.type != Lexer::Token::Identifier)
				return std::nullopt;

			property.uniform = uniform.view(lexer.code);

			next = lexer.next();
			if (next.type != Lexer::Token::RightParenthesis)
				return std::nullopt;

			next = lexer.next();
		} else {
			property.uniform = property.name;
		}

		if (next.type == Lexer::Token::Semicolon) {
			for (std::size_t i = 0; i < arity(property.type); i++) {
				Property::Default value { 0.0, std::nullopt };
				property.defaults.push_back(value);
			}

			return std::make_optional(property);
		}
		
		if (next.type != Lexer::Token::Equals)
			return std::nullopt;

		for (std::size_t i = 0; i < arity(property.type); i++) {
			if (i != 0) {
				next = lexer.next();
				if (next.type != Lexer::Token::Comma)
					return std::nullopt;
			}
			
			std::optional<Property::Default> value = parseDefault(lexer, lexer.next());
			if (!value.has_value())
				return std::nullopt;
			
			property.defaults.push_back(value.value());
		}

		next = lexer.next();
		if (next.type == Lexer::Token::Semicolon)
			return std::make_optional(property);

		return std::nullopt;
	}

	std::vector<Property> PropertiesParser::parse(const char* source, std::string_view view) {
		std::vector<Property> properties;

		std::string code(view);
		std::size_t source_offset = std::distance(source, view.data());

		Lexer lexer(code.data());
		Lexer::Token current = lexer.next();
		while (current.type != Lexer::Token::End) {
			std::optional<Property> property = parseProperty(lexer, current);
			if (property.has_value()) {
				std::size_t property_offset = source_offset + std::distance(code.c_str(), property->name.data());
				property->name = std::string_view(source + property_offset, property->name.size());

				std::size_t uniform_offset = source_offset + std::distance(code.c_str(), property->uniform.data());
				property->uniform = std::string_view(source + uniform_offset, property->uniform.size());

				properties.push_back(property.value());
			}

			current = lexer.next();
		}

		return properties;
	}
}
