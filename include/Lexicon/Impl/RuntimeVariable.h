#pragma once

#include "Variable.h"


namespace LEX
{
	class RuntimeVariable : public ClassAlias<std::variant<Void, Variable, std::reference_wrapper<Variable>>>
	{
	public:

		ALIAS_HEADER;

		using VariableRef = std::reference_wrapper<Variable>;

		Variable& Ref()
		{
			switch (index())
			{
			default:
				RGL_LOG(critical, "RuntimeVariable is undefined.");
				throw nullptr;

			case 1:
				return std::get<Variable>(*this);

			case 2:
				return std::get<VariableRef>(*this);
			}
		}

		RuntimeVariable AsRef()
		{
			//This function turns this into a reference even if it's not one. IE, it will make a runtime that refers
			// to this. Long story short, ensures it's passed by ref not val. Use with caution.
			return std::ref(Ref());
		}

		Variable* operator->()
		{
			return IsVoid() ? nullptr : &Ref();
		}

		bool IsVoid() { return !index() || Ref().IsVoid(); }

		operator Variable()
		{
			return Ref();
		}
		//Here's how it should work. This needs a wrapper for Variable&. When this object is deleted it's given a callback to hold onto when deleted.
		// When deleted and is used in a place that doesn't allow deletion
	};
}