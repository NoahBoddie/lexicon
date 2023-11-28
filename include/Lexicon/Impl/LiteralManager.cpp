#pragma once


#include "LiteralManager.h"


namespace LEX
{

	std::vector<Variable>& LiteralManager::_LiteralList()
	{
		static std::vector<Variable> _list{};
		return _list;
	}
}