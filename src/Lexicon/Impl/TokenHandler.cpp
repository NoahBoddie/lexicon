#include "Lexicon/Impl/TokenHandler.h"

namespace LEX::Impl
{
	void TokenHandler::_CheckCompiled()
	{
		if (_compRegex.empty() == false)
			return;

		auto sort_code = [](auto a, auto b) { return a.size() > b.size(); };
		//Can maybe use std::greater or someting instead.
		std::sort(_opCodes.begin(), _opCodes.end(), sort_code);
		std::sort(_puncCodes.begin(), _puncCodes.end(), sort_code);
		//this should be done only if we've confirmed that it's not a letter, because if it is that might
		// mean something completely different.

		for (auto& code : _opCodes){
			
			if (code.size() == 0)
				continue;
			
			std::string submit;

			if (std::isalpha(code[0]) != 0){
				submit = code;
			}
			else{

				submit.reserve(code.size());

				for (auto chr : code)
				{
					submit.push_back('\\');
					submit.push_back(chr);
				}
			}

			
			_compRegex += std::format(R"(({})|)", submit);
		}

		for (auto& code : _puncCodes){
			std::string submit;

			if (std::isalpha(code[0]) != 0) {
				submit = code;
			}
			else {

				submit.reserve(code.size());

				for (auto chr : code)
				{
					submit.push_back('\\');
					submit.push_back(chr);
				}
			}

			_compRegex += std::format(R"(({})|)", submit);
		}

		//std::ostringstream imploded;
		//std::copy(_opCodes.begin(), _opCodes.end(),
		//	std::ostream_iterator<std::string>(imploded, delim));
		//std::copy(_puncCodes.begin(), _puncCodes.end(),
		//	std::ostream_iterator<std::string>(imploded, delim));

		//_compRegex = imploded.str();
		_compRegex += remains;//Should end with "|"
	}

	void TokenHandler::_CheckConstant()
	{
		if (_constRegex.empty() == false)
			return;

		_constRegex.clear();
		_constRegex += format;
		_constRegex += "|";
		_constRegex += comments;
		_constRegex += "|";
		_constRegex += identifiers;
		_constRegex += "|";
		_constRegex += digits;
		_constRegex += "|";
		_constRegex += quotes;
		_constRegex += "|";
		_constRegex += objects;
		_constRegex += "|";
		//I think this looks stupid. Just making sure this is known.
	}

	std::vector<std::string> TokenHandler::GetKeywords() { return _keyCodes; }
	std::vector<std::string> TokenHandler::GetOperators() { return _opCodes; }
	std::vector<std::string> TokenHandler::GetPuncutation() { return _puncCodes; }

	std::string TokenHandler::GetRegex()
	{
		_CheckConstant();
		_CheckCompiled();

		return _constRegex + _compRegex;
	}
}