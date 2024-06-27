#include "Lexicon/Impl/InputStream.h"
#include "Lexicon/Impl/TokenStream.h"
#include "Lexicon/Impl/TokenHandler.h"

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

	void TokenStream::croak(std::string msg, Token* token)
	{
		input.croak(msg, token);
	}
		

	bool TokenStream::_Search(const std::string& token, std::vector<std::string> strings)
	{
		//move to source.
		for (int i = 0; i < strings.size(); i++)
		{
			if (token == strings[i])
				return true;
		}

		return false;
	}


	bool TokenStream::IsToken(RecordData& data, TokenType type)
	{
		//Because the idea is the regex squares a lot of this away.
		//If the first character is _ or a letter it's an identifier
		//keywords are matched one to one
		//numbers are matched if the first letter is a digit, or if the first char is . and it's not a size of 1.
		//Note, the above is pretty loose but that' primarily because I don't want the parser to remove an error where 4.4.4 becomes "4.4", ".4"
		// it should be 4.4.4 and thus should shit the bed once it tries to parse that.

		constexpr char quote_char = '\"';
		constexpr char apost_char = '\'';

		std::string& token = data.GetTag();

		//FOR ALL THINGS THAT SEARCH FRONT AND BACK, only the front should be exampled, however, the back should be a question for failure. Make that a different function
		// that should croak if need be. Such a thing should always return true.

		switch (type)
		{
		case TokenType::Number:
			return std::isdigit(token.front()) || token.front() == '.' && token.size() > 1 && std::isdigit(token[1]);

		case TokenType::String:
			//return token.front() == quote_char && token.back() == quote_char ||
			//	token.front() == apost_char && token.back() == apost_char;
			//We only check the front because we want to be descriptive of the failure involved, that being
			// " Back character of string isn't "/' "
			return token.front() == quote_char || token.front() == apost_char;


		case TokenType::Object:
			//Could probably do this a bit better with strcmp
			return token.find(":{") == 0 && token.rfind("}") == token.size() - 1;

		case TokenType::Boolean:
			//TODO: Maybe might not be forced into being a boolean, as I have other plans for the name.
			return token == "true" || token == "false" || token == "maybe";

		case TokenType::Identifier:
			return std::isalpha(token.front()) || token.front() == '_';

		case TokenType::Keyword:
			//Keywords have to be submitted to a thingy, I'm not about that right now.
			//return _Search(token, "KEY", "KEY");
			return _Search(token, TokenHandler::GetKeywords());

		case TokenType::Punctuation:
			//Checks for characters(, will handle :: later.
			//return _Search(token, "[", "]", "{", "}", "(", ")", ",", ";", ":", "::");
			return _Search(token, TokenHandler::GetPuncutation());
			

		case TokenType::Operator:
			//Checks for strings.
			//return _Search(token, "*", "/", "-", "+", "=");
			return _Search(token, TokenHandler::GetOperators());

		case TokenType::Format:
			//If the last token was a format
		{
			RecordData previous = prev();
			//The last tag has to be format, and the front and back of this token needs to be '{' and '}'
			return previous.GetTag() == "format" && token.front() == '{' && token.back() == '}';

		}


		case TokenType::Comment:
			if (auto size = token.size(); size >= 2){
				int res = std::strncmp(token.c_str(), "//", 2) == 0 ?
					1 : std::strncmp(token.c_str(), "/*", 2) == 0 ?
					2 : 0;
				//IE the second comment type
				const char& end = *(token.end() - 2);

				if (res == 2 && std::strncmp(&end, "*/", 2) != 0) {
					//Later, this is to be a generic function, able to print a generic name.
					croak("COMMENT unclosed at end of file.", &data.TOKEN());
				}

				return res;
			}
			else
				return false;

		default:
			report::parse::error("unconfirmable token type {} detected. Terminating program.", (int)type);
			throw nullptr;
		}

	}

	bool TokenStream::SetIfToken(RecordData& token, TokenType type)
	{
		bool result = IsToken(token, type);

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


		//Fuck it. Comments are all culled. ReadNext culls all things it things are comments.
		if (IsToken(data, TokenType::Comment) == true) {
			//Right here is a great place to have special comments that are culled to have special messages.
			//Clipping these would be nice btw.
			RGL_LOG(debug, "{}", data.GetTag());
			return _ReadNext();
		}

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

		if (SetIfToken(data, TokenType::Operator) == true) {
			return data;
		}
		//Might have to go below format.
		if (SetIfToken(data, TokenType::Identifier) == true)
			return data;



		if (SetIfToken(data, TokenType::Punctuation) == true)
			return data;


		if (SetIfToken(data, TokenType::Format) == true)
			return data;

		croak(std::format("Tokenizing: Unidentified token '{}' detected", data.GetTag()));

		return {};
	}

	InputStream* TokenStream::GetInput()
	{
		return &input;
	}
}


