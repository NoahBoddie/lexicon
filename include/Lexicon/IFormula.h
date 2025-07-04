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
				virtual std::string_view GetName() const = 0;
				virtual void SetName(const std::string_view& name) = 0;

			};
		}

		CURRENT_VERSION(IFormula, 1);
	}

	struct IMPL_VERSION(IFormula) {};
}