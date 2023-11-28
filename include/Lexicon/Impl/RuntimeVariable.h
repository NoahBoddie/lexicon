#pragma once

#include "Variable.h"

namespace LEX
{
	struct RuntimeVariable : public ClassAlias<std::variant<Void, Variable, std::reference_wrapper<Variable>>>
	{

		ALIAS_HEADER;

		using VariableRef = std::reference_wrapper<Variable>;

		Variable& Ref()
		{
			switch (index())
			{
			default:
				throw nullptr;

			case 1:
				return std::get<Variable>(*this);

			case 2:
				return std::get<VariableRef>(*this);
			}
		}

		Variable* operator->()
		{
			return IsVoid() ? nullptr : &Ref();
		}

		bool IsVoid() { return !index() || Ref().IsVoid(); }

		//Here's how it should work. This needs a wrapper for Variable&. When this object is deleted it's given a callback to hold onto when deleted.
		// When deleted and is used in a place that doesn't allow deletion
	};
}