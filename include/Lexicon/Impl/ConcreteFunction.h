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

		//I'm not currently doing anything with these but they're here when I get to generics.
		std::vector<ITypePolicy*> GetGenericInput() override { return {}; }
		std::vector<DefaultGenericInput> GetDefaultGenericInput() override { return {}; }
		std::vector<DefaultParamInput> GetDefaultParamInput() override { return {}; }


	};

	using Function = ConcreteFunction;
}
