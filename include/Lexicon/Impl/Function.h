#pragma once

#include "Element.h"
#include "AbstractFunction.h"

namespace LEX
{

	struct FunctionData : public Element
	{
		//weren't functions environments? A: They don't need to be anymore.
		//Data
	};

	struct Function : public AbstractFunction,  public FunctionData
	{
		bool IsAbstract() const override { return false; }
	};
}