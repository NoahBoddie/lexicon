#pragma once

#include "Lexicon/Impl/InputStream.h"
#include "Lexicon/Impl/TokenStream.h"

namespace LEX::Impl
{

	RecordData TokenStream::next()
	{
		RecordData token = _current;

		//Don't let this fool you, current is a tool used by peek.
		_current = {};

		if (!token) {
			token = _ReadNext();
		}
		//Previous equals token, because token here is what will be deleted after finishing.
		_previous = token;

		return token;
	}

	RecordData TokenStream::peek()
	{
		if (!_current)
			_current = _ReadNext();

		return _current;
	}

	RecordData TokenStream::prev()
	{
		return _previous;
	}


	bool TokenStream::eof()
	{
		return !peek();
	}

	void TokenStream::croak(std::string msg)
	{
		input.croak(msg);
	}
		

		


	bool TokenStream::IsToken(std::string& token, TokenType type)
	{
		//Because the idea is the regex squares a lot of this away.
		//If the first character is _ or a letter it's an identifier
		//keywords are matched one to one
		//numbers are matched if the first letter is a digit, or if the first char is . and it's not a size of 1.
		//Note, the above is pretty loose but that' primarily because I don't want the parser to remove an error where 4.4.4 becomes "4.4", ".4"
		// it should be 4.4.4 and thus should shit the bed once it tries to parse that.

		constexpr char quote_char = '\"';
		constexpr char apost_char = '\'';

		switch (type)
		{
		case TokenType::Number:
			return std::isdigit(token.front()) || token.front() == '.' && std::isdigit(token.front()++);

		case TokenType::String:
			return token.front() == quote_char && token.back() == quote_char ||
				token.front() == apost_char && token.back() == apost_char;

		case TokenType::Object:
			//Could probably do this a bit better with strcmp
			return token.find(":{") == 0 && token.rfind("}") == token.size() - 1;

		case TokenType::Boolean:
			//should I make this string
			return token == "true" || token == "false";

		case TokenType::Identifier:
			return std::isalpha(token.front()) || token.front() == '_';

		case TokenType::Keyword:
			//Keywords have to be submitted to a thingy, I'm not about that right now.
			return _Search(token, "KEY", "KEY");

		case TokenType::Punctuation:
			//Checks for characters(, will handle :: later.
			return _Search(token, '[', ']', '{', '}', '(', ')', ',', ';', ':');//, '::');
			return false;

		case TokenType::Operator:
			//Checks for strings.
			return _Search(token, "*", "/", "-", "+", "=");

		case TokenType::Inline:
			//Inlined language utilization is the only thing that will ever have these built into itself
			return token.front() == '{' && token.back() == '}';

		default:
			RGL_LOG(critical, "unconfirmable token type {} detected. Terminating program.", (int)type);
			throw nullptr;
		}

	}

	bool TokenStream::SetIfToken(RecordData& token, TokenType type)
	{
		std::string& str = token.GetTag();

		bool result = IsToken(str, type);

		if (result)
			token.GetEnum<Token>().type = type;

		return result;
	}



	RecordData TokenStream::_ReadNext()
	{
		if (input.eof() == true)
			return {};



		//I don't need to use peek because peek is used to preserve the first character to when making a string.
		// but the string is already made, so peek is basically useless.
		RecordData data = input.next();

		if (!data)
			croak("Empty string requested for use");


		if (SetIfToken(data, TokenType::Boolean) == true)
			return data;

		if (SetIfToken(data, TokenType::Keyword) == true)
			return data;

		if (SetIfToken(data, TokenType::Object) == true)
			return data;

		if (SetIfToken(data, TokenType::String) == true)
			return data;

		if (SetIfToken(data, TokenType::Number) == true)
			return data;

		if (SetIfToken(data, TokenType::Identifier) == true)
			return data;

		if (SetIfToken(data, TokenType::Punctuation) == true)
			return data;

		if (SetIfToken(data, TokenType::Operator) == true)
			return data;

		if (SetIfToken(data, TokenType::Inline) == true)
			return data;

		croak(std::format("Tokenizing: Unidentified token '{}' detected", data.GetTag()));

		return {};
	}

	InputStream* TokenStream::GetInput()
	{
		return &input;
	}
}