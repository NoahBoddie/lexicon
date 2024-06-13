#pragma once

#include "Variable.h"

namespace LEX
{

	template <typename T>
	struct Unvariable
	{

	};



	//Should be named var converter

	template <stl::castable_from<Variable> Type>
	struct Unvariable<Type>
	{
		Type operator()(Variable* var)
		{
			if (var->CanCastTo<Type>() == false)
				report::apply::fatal("Current value of Variable is unable to be cast to '{}'.", typeid(Type).name());

			return static_cast<Type>(*var);
		}
	};

}