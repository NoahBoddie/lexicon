#include "Lexicon/Impl/ParseHandler.h"

namespace LEX::Impl
{
	void ParseHandler::AddModule(ParseModule* mdl)
	{
		//Check if the module is within the list
		if (auto end = _moduleList.end(); std::find(_moduleList.begin(), end, mdl) != end)
			return;

		_sorted = false;
		_moduleList.push_back(mdl);
	}



	GENERIC_ARRAY(ParseModule*) ParseHandler::GetModuleList()
	{
		_CheckSort();

		return _moduleList;

		decltype(_moduleList) result;

		//std::copy_if(_moduleList.begin(), _moduleList.end(), std::back_inserter(result),
		//	[&](ParseModule* it) { return it->GetDepth() == depth; });

		return result;
	}

	ParseModule* ParseHandler::GetModuleByName(std::string_view name)
	{
		auto it = _moduleList.begin();
		auto end = _moduleList.end();

		it = std::find_if(it, end,
				[&](ParseModule* it) { return it->GetModuleName() == name; });

		if (it == end)
			return nullptr;

		return *it;
	}


	void ParseHandler::_CheckSort()
	{
		if (_sorted)
			return;

		std::sort(_moduleList.begin(), _moduleList.end(),
			[](ParseModule* a_lhs, ParseModule* a_rhs) {return a_lhs->GetPriority() > a_rhs->GetPriority(); });

		_sorted = true;
	}
}
