#pragma once

#include "Element.h"
#include "IFunction.h"
#include "FunctionData.h"

namespace LEX
{

	class FunctionBase : public virtual IFunction, public SecondaryElement, public FunctionData
	{
		//This is a pivot for for functions, more important than anywhere else, this set up excludes formulas
		// from being able to be stored in a function, or having the same linking



	public:
		std::string GetName()
		{
			RGL_LOG(critical, "this shit not supposed to be used fam");
			throw nullptr;
		}
	};


}
