#pragma once

#include "Variable.h"

namespace LEX
{
	struct IVariable : public Variable
	{
		//Class basically does nothing, serves only as the polymorphic base
		virtual ~IVariable() = default;

		IVariable()
		{
			SetPolymorphic(true);
		}
	};

}