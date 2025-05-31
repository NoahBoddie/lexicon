#pragma once

#include "Lexicon/TypeAliases.h"
#include "TokenType.h"

namespace LEX
{
	struct Token
	{
		//Currently the size is 1 + 2 + 2, which means 3 bytes aren't being used.
		TokenType type = TokenType::Invalid;
		bool isConditional = false;//Used to tell if this was a conditional keyword or not, if it is it will check with the current parse module
		Column column;
		Line line;

		bool IsLiteral()
		{
			switch (type)
			{
			case TokenType::Boolean:
			case TokenType::Number:
			case TokenType::String:
			case TokenType::Object:
				return true;
			}

			return false;
		}

		bool IsType(TokenType t) const
		{
			return t == type;
		}

		std::string Print()
		{
			return std::format("Token: {} (col: {}/ line: {}", magic_enum::enum_name(type), column, line);
		}

		operator std::string() { return Print(); }

		Token() = default;
		
		Token(Column c, Line l) : column{ c }, line{ l } {}
	};
	//static_assert(sizeof(Token) == 0x8);

//Shorthand for getting a token from a token. Makes sure that it's a record data to prevent misuse.
#define TOKEN() GetEnumFromData<Token>()
}