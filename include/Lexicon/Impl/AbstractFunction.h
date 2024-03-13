#pragma once

#include "IFunction.h"
#include "ICallableUnit.h"

namespace LEX
{
	struct RoutineBase;
	struct IGenericArgument;
	struct AbstractTypePolicy;

	struct AbstractFunction : public virtual IFunction, public ICallableUnit
	{
		//virtual size_t GetParameterCount() = 0;

		//May make a GetAt by default but still allow overriding with virtual
		//virtual std::vector<AbstractTypePolicy*> GetParameters() = 0;

		virtual bool IsAbstract() const { return true; }

		virtual bool IsGeneric() const { return false; }
		
		

		//Change this when generic gets into the picture
		AbstractFunction* GetFunction(IGenericArgument* args) override
		{
			return this;
		}
	};


}