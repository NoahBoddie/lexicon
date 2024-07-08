#pragma once

#include "ISpecial.h"

namespace LEX
{
	struct AbstractFunction;
	struct IGenericArgument;

	struct IFunction : public ISpecial
	{

		virtual bool IsConversion() const { return false; }
		virtual bool IsMethod() const { return false; }
		virtual bool IsAbstract() const { return false; }
		virtual std::string_view GetName() const = 0;
		virtual AbstractFunction* GetFunction(IGenericArgument* args) = 0;
	INTERNAL:
		//This shouldn't be internal should it? P
		virtual uint64_t GetProcedureData() const = 0;
	};
}