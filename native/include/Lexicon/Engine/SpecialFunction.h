#pragma once

#include "Lexicon/Function.h"

#include "Lexicon/Engine/SpecialBase.h"
#include "Lexicon/Engine/GenericFunction.h"

//*src
#include "Lexicon/Engine/FunctionBase.h"

namespace LEX
{

	struct SpecialFunction : public Function, public SpecialBase
	{
		FunctionBase* _self = nullptr;

		//SpecialFunction(GenericBase* base, IFunction* func, ITemplatePart* spec) : SpecialBase{ base, spec }, _self{ func }
		//{
		//	report::info("NewBodyPart {}", (uintptr_t)this);
		//}

		SpecialFunction(GenericFunction* base, ITemplatePart* spec) : SpecialBase{ base, spec }, _self{ base }
		{
			report::info("NewBodyPart {}", (uintptr_t)this);
		}


		//TODO: The below part about itself seems to maybe be possible to template away. If there's merit I'll do it. 
		// For now, boilerplate. If it's not interface do it later
		FunctionBase* GetSelf()
		{
			return _self;
		}

		const FunctionBase* GetSelf() const
		{
			return _self;
		}

		ISpecializable* GetSpecializable() override { return GetGeneric(); }

		bool IsResolved() const override
		{
			return SpecialBase::IsResolved();
		}




		virtual std::string_view GetName() const { return GetSelf()->GetName(); }




		IFunction* CheckFunction(ITemplatePart* args) override
		{
			return dynamic_cast<IFunction*>(ObtainSpecial(args));
		}

		Function* GetFunction(ITemplateBody* args) override
		{
			auto special = ObtainSpecial(args)->ToResolved<Function>();

			if (!special) {
				//TODO: Scream maybe?
			}

			return special;
		}


		ITemplatePart* GetTemplatePart() override
		{
			//This exists so someone can get the part of a generic type, either before or after specializing.
			return this;
		}
		uint64_t GetProcedureData() const override
		{
			return 0;
		}

		RuntimeVariable Execute(api::vector<RuntimeVariable> args, Runtime* runtime, RuntimeVariable* def) override
		{
			if (IsResolved() == false)
				report::apply::error("Specialized function is not resolved");


			return _self->BasicExecute(this, this, args, runtime, def);
		}



	};

}