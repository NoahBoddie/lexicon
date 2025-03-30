#pragma once

#include "IFunction.h"
#include "ICallableUnit.h"

namespace LEX
{
	struct RoutineBase;
	struct ITemplateBody;
	struct Type;

	struct AbstractFunction : public virtual IFunction, public ICallableUnit
	{
		//virtual size_t GetParameterCount() = 0;

		//May make a GetAt by default but still allow overriding with virtual
		//virtual std::vector<Type*> GetParameters() = 0;

		bool IsResolved() const override final { return true; }

		virtual bool IsGeneric() const { return false; }
		
		

		//Change this when generic gets into the picture
		AbstractFunction* GetFunction(ITemplateBody* args) override
		{
			return this;
		}
	};


}