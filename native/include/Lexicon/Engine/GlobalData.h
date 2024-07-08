#pragma once

#include "Lexicon/Variable.h"

namespace LEX
{
	struct ICallableUnit;


	struct GlobalData
	{
		std::string _name;

		ICallableUnit* _init = nullptr;//This is the default for a given global. Call to reset value. But, do note, this should be a formula, not a callable unit.
		Variable value{};//This is the current value of the global. Should be assigned a type and that type should not change.
	};

}