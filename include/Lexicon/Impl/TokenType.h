#pragma once

namespace LEX::Impl
{
	//enum TokenType : __int8
	ENUM(TokenType, uint8_t)
	{
		Invalid,
		Number,			//Number literal
		//Integer,		//Integer literal//I wish to seperate into 2 categories integers and literals, so I know which to make.
		String,			//String literal
		Object,			//Object literal
		Boolean,		//Boolean literal
		Punctuation,
		Identifier,
		Keyword,
		Operator,
		Comment,		//Not actually used, just a used in searching. Later, there can perhaps be a tooltip system where it can preserve comments.
		Format,			//Used to designate the inline language statement(s) of the script. One type per script.
	};


	//Of the above, I can make some valid starting values, and then some valid continue values.
	//I'll be leaving this role to regex, the token stream will handle the assignment.

	//Unsure if I wish to inline this.
	inline std::string_view TokenToString(TokenType t, bool cap = true)
	{
		switch (t)
		{
		case TokenType::Keyword:
			return cap ? "Keyword" : "keyword";
		case TokenType::Number:
			return cap ? "Number" : "number";
		case TokenType::String:
			return cap ? "String" : "string";
		case TokenType::Object:
			return cap ? "Object" : "object";
		case TokenType::Boolean:
			return cap ? "Boolean" : "boolean";
		case TokenType::Punctuation:
			return cap ? "Punctuation" : "punctuation";
		case TokenType::Identifier:
			return cap ? "Identifier" : "identifier";
		case TokenType::Operator:
			return cap ? "Operator" : "operator";
		}
		return "Invalid";
	}
}