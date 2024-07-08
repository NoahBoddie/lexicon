#include "Lexicon/Engine/InputStream.h"
#include "Lexicon/Engine/TokenHandler.h"

namespace LEX::Impl
{
	using Iterator = std::string::const_iterator;


	void InputStream::_CalcColumnLine(Iterator until, Column& a_column, Line& a_line)
	{
		//The lines and columns calculated are off. Need to fix it somehow.

		//Better to just use a while actually.
		auto test = std::count_if(_current, until, [&](auto it)
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


	RecordData InputStream::_ReadNext(Iterator& it)
	{
		if (_current == _end)
			return {};

		auto end = stream.cend();

		boost::smatch what;

		//bool is_end = eof();
		//boost::regex_constants::

		if (boost::regex_search(it, end, what, _regex) == true) {
			//So the problem with this is it's not accounting for what it had to cull from _current to get to the new it.
			// this is most seen on the first run, where it will always have a first token on the starting line values.
			
			bool is_iterating = &it == &_current;

			if (auto new_it = what[0].first; is_iterating && new_it != it) {
				_CalcColumnLine(new_it, _column, _line);
				it = new_it;
			}

			auto result = RecordData{ std::string(what[0]), Token { _column, _line} };


			if (what[0].length() == 0)
				croak(std::format("empty string found in parse results {}", std::string(it, end)));


			if (result.GetTag()[0] == '\n')
				croak("The unforseen hath occured.");

			if (auto new_it = what[0].second; is_iterating) {
				_CalcColumnLine(new_it, _column, _line);
				it = new_it;
			}

			return result;
		}

		it = end;

		return {};
	}

	RecordData InputStream::next()
	{
		//Using this while eof should likely result in a parsing error.

		RecordData token = _peek;

		_peek = {};//Should make a clear func for this.

		if (!token) {
			token = _ReadNext(_current);
		}
		else {
			//_CalcColumnLine(token.GetTag(), column, line);
		}

		return token;
	}

	RecordData InputStream::peek() {
		//So it's something 
		if (!_peek) {
			//auto it = _current;
			//_peek = _ReadNext(it);
			_peek = _ReadNext(_current);
		}

		return _peek;
	}
	bool InputStream::eof() {
		//Adding on this this with peek
		//return *_current == '\0' && !_peek;
		//return _current == _end && !_peek;
		//RGL_LOG(debug, "cur {} && peek {}", _current == _end, !_peek);
		//return _current == _end && !_peek;
		return  _current == _end || !peek();
	}

	void InputStream::croak(std::string msg, Token* token) {
		//don't care so much about this.
		//TODO:Include name and project in croak please
		if (_column && _line)
			throw ParsingError(std::format("{}(at line {} : column {})", msg, token ? token->line : _line, token ? token->column : _column));
		else
			throw ParsingError(std::format("{}", msg));

	}

	std::string InputStream::project()
	{
		return _project;
	}

	std::string InputStream::name()
	{
		return _name;
	}


	

	InputStream::InputStream(std::string n, std::string p, std::string s, Line l, Column c)
	{
		auto _1 = c;
		auto _2 = l;
		
		//Will croak without outputting line and column at this point.

	
		std::string kinda_basic = TokenHandler::GetRegex();
		
		_regex = boost::regex{ kinda_basic };
		_project = p;
		_name = n;
		stream = s;
		_current = stream.cbegin();
		_end = stream.cend();
		_CalcColumnLine(_end, _1, _2);
		_line = l;
		_column = c;
	}
}