#pragma once

#include "FunctionBase.h"
#include "AbstractFunction.h"

namespace LEX
{
	class RuntimeVariable;

	class ConcreteFunction : public FunctionBase, public AbstractFunction
	{
	public:

		//void LoadFromRecord(Record&)

		bool IsAbstract() const override { return false; }

	
		RuntimeVariable Invoke(std::vector<RuntimeVariable>& args, RuntimeVariable*) override;

	};

	using Function = ConcreteFunction;
}
