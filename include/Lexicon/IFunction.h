#pragma once

#include "ISpecial.h"

namespace LEX
{
	struct AbstractFunction;
	struct ITemplateBody;

	struct IFunction : public ISpecial
	{

		virtual bool IsConversion() const { return false; }
		virtual bool IsMethod() const { return false; }
		bool IsResolved() const override { return false; }
		virtual std::string_view GetName() const = 0;
		virtual AbstractFunction* GetFunction(ITemplateBody* args) = 0;
	INTERNAL:
		//This shouldn't be internal should it? P
		virtual uint64_t GetProcedureData() const = 0;
	};
}