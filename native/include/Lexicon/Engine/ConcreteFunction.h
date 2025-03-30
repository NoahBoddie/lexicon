#pragma once

#include "Lexicon/Engine/FunctionBase.h"
#include "Lexicon/AbstractFunction.h"

namespace LEX
{
	class RuntimeVariable;

	class ConcreteFunction : public FunctionBase, public AbstractFunction
	{
	public:
		ISpecializable* GetSpecializable() override { return nullptr; }

		//void LoadFromRecord(Record&)

	
		RuntimeVariable Execute(api::vector<RuntimeVariable> args, Runtime* runtime, RuntimeVariable* def) override;


	protected:
		void* Cast(std::string_view name) override
		{
			switch (Hash(name))
			{
			case Hash(TypeName<AbstractFunction>::value):
				return (AbstractFunction*)this;

			case Hash(TypeName<ConcreteFunction>::value):
				return this;
			}

			return __super::Cast(name);
		}

	};

	using Function = ConcreteFunction;
}
