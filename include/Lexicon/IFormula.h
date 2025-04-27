#pragma once

#include "Lexicon/ICallableUnit.h"


namespace LEX
{
	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(IFormula, ICallableUnit)
			{
				
			};
		}

		CURRENT_VERSION(IFormula, 1);
	}

	struct IMPL_VERSION(IFormula) {};
}