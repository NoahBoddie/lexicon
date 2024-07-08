#pragma once

#include "Lexicon/TypeAliases.h"
#include "TokenType.h"

namespace LEX::Impl
{
	struct Token
	{
		//Currently the size is 1 + 2 + 2, which means 3 bytes aren't being used.
		TokenType type = TokenType::Invalid;
		//Want to use some free space here.
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

		std::string Print()
		{
			return std::format("Token: {} (col: {}/ line: {}", TokenToString(type), column, line);
		}

		operator std::string() { return Print(); }

		Token() = default;
		
		Token(Column c, Line l) : column{ c }, line{ l } {}
	};
	//static_assert(sizeof(Token) == 0x8);

//Shorthand for getting a token from a token. Makes sure that it's a record data to prevent misuse.
#define TOKEN() GetEnumFromData<Token>()
}