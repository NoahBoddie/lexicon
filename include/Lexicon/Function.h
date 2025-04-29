#pragma once

#include "Lexicon/IFunction.hpp"



#include "ICallableUnit.h"

namespace LEX
{
	struct RoutineBase;
	struct ITemplateBody;
	struct TypeInfo;

	struct Function : public IFunction, public ICallableUnit
	{
		//virtual size_t GetParameterCount() = 0;

		//May make a GetAt by default but still allow overriding with virtual
		//virtual std::vector<TypeInfo*> GetParameters() = 0;

		bool IsResolved() const override final { return true; }

		virtual bool IsGeneric() const { return false; }
		
		

		//Change this when generic gets into the picture
		Function* GetFunction(ITemplateBody* args) override
		{
			return this;
		}
	};


}