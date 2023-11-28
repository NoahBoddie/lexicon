#pragma once

#include "Function.h"
#include "ISpecializable.h"
#include "SpecialFunction.h"
namespace LEX
{
	struct GenericFunction : public FunctionData, public ISpecializable, public IFunction
	{
		



		SpecServer* MakeServer(IGenericArgument& args) override
		{
			//Not actually proper, this is just what it sends.
			return new SpecialFunction();
		}

	};
}