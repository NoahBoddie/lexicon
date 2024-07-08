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

		bool IsAbstract() const override { return false; }

	
		RuntimeVariable Invoke(std::vector<RuntimeVariable>& args, RuntimeVariable*) override;


	};

	using Function = ConcreteFunction;
}
