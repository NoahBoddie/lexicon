#pragma once

#include "Lexicon/Impl/InputStream.h"

namespace LEX::Impl
{

	class InputStream;

	class TokenStream
	{
	public:
		TokenStream(InputStream& i) : input{ i }{}

		TokenStream(InputStream&& i) : input{ i }{}

	private:

		//Searches a list of chars and sees if they're within the token string
		template<std::convertible_to<char>... CharType>
		bool _Search(const std::string& token, CharType... chars)
		{
			std::array<char, sizeof...(CharType)> characters{ chars... };

			for (int i = 0; i < characters.size(); i++)
			{
				if (token.find(characters[i]) != std::string::npos)
					return true;
			}

			return false;
		}

		//Searches a list of strings and sees if they're equal to the token string
		bool _Search(const std::string& token, std::vector<std::string> strings);

		//Searches a list of strings and sees if they're equal to the token string
		template<std::convertible_to<std::string>... StringType>
		bool _Search(const std::string& token, StringType... strs)
		{
			std::array<std::string, sizeof...(StringType)> strings{ std::string{strs}... };

			for (int i = 0; i < strings.size(); i++)
			{
				if (token == strings[i])
					return true;
			}

			return false;
		}


		RecordData _ReadNext();

	public:




		RecordData next();

		RecordData peek();

		RecordData prev();


		bool eof();

		void croak(std::string msg, Token* token = nullptr);

		//Both of these will likely need to be 

		


		bool IsToken(RecordData& token, TokenType type);

		bool SetIfToken(RecordData& token, TokenType type);



		InputStream* GetInput();

	private:
		RecordData _current;
		RecordData _previous;

	public:
		InputStream& input;
	};

}