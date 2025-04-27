#pragma once

#include "Variable.h"

namespace LEX
{

	template <typename T>
	struct Unvariable : public detail::not_implemented
	{

	};



	//Should be named var converter

	template <stl::castable_from<Variable> TypeInfo>
	struct Unvariable<TypeInfo>
	{
		TypeInfo operator()(Variable* var)
		{
			if (var->CanCastTo<TypeInfo>() == false)
				report::apply::critical("Current value of Variable is unable to be cast to '{}'.", typeid(TypeInfo).name());

			return static_cast<TypeInfo>(*var);
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