#pragma once

#include "Lexicon/Variable.h"
#include "Lexicon/ProxyGuide.h"
namespace LEX
{

	template <typename T>
	struct Unvariable : public detail::not_implemented
	{

	};

	template <typename T> requires (requires(ProxyGuide<T> guide, Variable* arg) { { guide.Unvariable(arg) } -> std::convertible_to<T>; })
	struct Unvariable<T>
	{
		decltype(auto) operator()(Variable* var)
		{
			ProxyGuide<T> guide{};

			decltype(auto) result = guide.Unvariable(var);

			return result;
		}
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