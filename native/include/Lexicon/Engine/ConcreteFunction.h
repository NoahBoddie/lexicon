#pragma once

#include "Lexicon/Engine/FunctionBase.h"
#include "Lexicon/Function.h"

namespace LEX
{
	class RuntimeVariable;

	class ConcreteFunction : public ConcreteFuncBase
	{
	public:
		ISpecializable* GetSpecializable() override { return nullptr; }

		//void LoadFromRecord(Record&)

	
		IFunction* CheckFunction(ITemplatePart* args) override
		{
			return this;
		}

		RuntimeVariable Execute(std::span<RuntimeVariable> args, Runtime* runtime, RuntimeVariable* def) override;



	protected:
		void* Cast(std::string_view name) override
		{
			switch (Hash(name))
			{
			case Hash(TypeName<Function>::value):
				return static_cast<Function*>(this);

			case Hash(TypeName<ConcreteFunction>::value):
				return this;
			}

			return __super::Cast(name);
		}

	};
}
