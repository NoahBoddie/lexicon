#pragma once

#include "Variable.h"

namespace LEX
{

	template <typename T>
	struct Unvariable : public detail::not_implemented
	{

	};



	//Should be named var converter

	template <stl::castable_from<Variable> Type>
	struct Unvariable<Type>
	{
		Type operator()(Variable* var)
		{
			if (var->CanCastTo<Type>() == false)
				report::apply::critical("Current value of Variable is unable to be cast to '{}'.", typeid(Type).name());

			return static_cast<Type>(*var);
		}
	};

	template <>
	struct Unvariable<Variable>
	{
		Variable& operator()(Variable* var)
		{
			return *var;
		}
	};

}