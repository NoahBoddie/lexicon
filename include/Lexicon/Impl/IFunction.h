#pragma once

#include "Interface.h"

namespace LEX
{
	struct AbstractFunction;
	struct IGenericArgument;

	struct IFunction : public Interface
	{
		virtual bool IsAbstract() const { return false; }
		virtual AbstractFunction* GetFunction(IGenericArgument* args) = 0;
	};
}