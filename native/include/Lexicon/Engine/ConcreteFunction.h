#pragma once

#include "Lexicon/Engine/FunctionBase.h"
#include "Lexicon/AbstractFunction.h"

namespace LEX
{
	class RuntimeVariable;

	class ConcreteFunction : public FunctionBase, public AbstractFunction
	{
	public:

		//void LoadFromRecord(Record&)

	
		RuntimeVariable Execute(std::vector<RuntimeVariable>& args, Runtime* runtime, RuntimeVariable* def) override;


	};

	using Function = ConcreteFunction;
}
