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

		void LoadFromRecord(Record& target) override;

		void OnAttach() override;

		//This and generic don't care which it is, but concrete policy will, so it's virtual to report that possibility
		// The same may be done for parameter. We shall see.
		virtual void SetReturnType(ITypePolicy* policy);

	public:

		std::string GetName()
		{//would an empty check be better?
			if (_name == "")
				return "<empty>";
			
			return _name;
		}
	};


}
