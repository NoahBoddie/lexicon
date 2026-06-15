#pragma once

#include "Lexicon/Engine/FunctionBase.h"
#include "Lexicon/Function.h"

namespace LEX
{
	class RuntimeVariable;

	class ConcreteFunction : public ConcreteFuncBase
	{
	public:
		DEFINE_COMPONENT_TYPE(ComponentType::ConcreteFunction)
		DECL_IMPL_FUNC_ELEMENT;

		ISpecializable* GetSpecializable() override { return nullptr; }

		//void LoadFromRecord(Record&)

	
		IFunction* CheckFunction(ITemplatePart* args) override
		{
			return this;
		}

		RuntimeVariable Execute(std::span<RuntimeVariable> args, Runtime* runtime, RuntimeVariable* def) override;
	
	private:
		const Component* GetComponent() const override final { return this; }
	};
}
