#pragma once

#include "Lexicon/Impl/Token.h"
#include "Lexicon/Impl/Exception.h"
namespace LEX::Impl
{

	inline std::vector<char> GetPuncuations()
	{
		return {};
	}

	inline std::vector<std::string_view> GetOperators()
	{
		return {};
	}

	inline std::vector <std::string_view> GetKeywords()
	{
		return {};
	}


	//submit the streams to the next one.

	class InputStream
	{
		//-input stream takes a string.
		//-The idea is you include a regex to the input stream, and then using that it creates tokens. This will allow me to use the input stream for more than
		// 1 thing.
		//-The idea should be something like having a map to associate with different compiler settings, this way, I can collect the strings for regular expression
		// once for a given input type, and then never again.
		

		//*
		using Iterator = std::string::const_iterator;
	public:
		InputStream(std::string n, std::string p, std::string s, Line l = 1, Column c = 1);


		[[deprecated("deprecated due to better version")]]
		void _CalcColumnLine(const std::string& str, Column& a_column, Line& a_line)
		{
			auto test = std::count_if(str.begin(), str.end(), [&](auto it)
				{//will count the number of line breaks and when one is detected it will reset the column
					bool res = it == '\n' || it == '\r';
					if (res) {
						a_column = 1;

						if (max_value<Line> == a_line)
							croak(std::format("Max number of line reached, more than {} lines.", max_value<Line>));

						a_line += 1;
					}
					else {
						if (max_value<Column> == a_column)
							croak(std::format("Column at line {} exceeds max character limit of {}.", a_column, max_value<Column>));

						a_column += 1;
					}
					return res;
				});
		}


		void _CalcColumnLine(Iterator until, Column& a_column, Line& a_line);


		RecordData _ReadNext(Iterator& it);

	public:
		RecordData next();

		RecordData peek();

		bool eof();

		void croak(std::string msg);

		std::string project();
		
		std::string name();


		//Save point is part of a concept where something can save, begin doing processes and possibly bail out once something is figured own to have
		// been interpreted ambiguously or something idk. likely won't need
		//Iterator _savePoint;


		

		//screams when I make it const I don't care just want to move on.
		std::string stream;

	private:
		Column _column = 0;
		Line _line = 0;


		Iterator _current;
		//std::byte test[sizeof(Iterator)];
		Iterator _end;


		boost::regex _regex;


		//Peek is this is a storage value for what peek uses. This way I don't have to use a regex over and over again when peek is used. This should be cleared
		// when next is called.
		RecordData _peek;

		std::string _project;
		std::string _name;

	};

}