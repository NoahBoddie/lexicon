#include "Lexicon/Engine/ParseHandler.h"

namespace LEX::Impl
{
	void ParseHandler::AddBuilder(ModuleBuilder build)
	{
		//Check if the module is within the list
		if (auto end = _buildList.end(); std::find(_buildList.begin(), end, build) != end)
			return;

		_sorted = false;
		_buildList.push_back(build);
	}



	std::vector<std::unique_ptr<ParseModule>> ParseHandler::BuildModules()
	{

		_CheckSort();

		std::vector<std::unique_ptr<ParseModule>> result{ _buildList.size() };

		std::transform(_buildList.begin(), _buildList.end(), result.begin(), [](ModuleBuilder builder) {return builder(); });

		return result;

	}



	void ParseHandler::_CheckSort()

	{
		if (_sorted)
			return;

		//I'm going to keep it a buck, I do not want to find some way to sort these entries at the same time, so I will ust be doing this.
		std::sort(_buildList.begin(), _buildList.end(),
			[](ModuleBuilder a_lhs, ModuleBuilder a_rhs) {return a_lhs()->GetPriority() > a_rhs()->GetPriority(); });

		_buildList.shrink_to_fit();

		_sorted = true;
	}

}
