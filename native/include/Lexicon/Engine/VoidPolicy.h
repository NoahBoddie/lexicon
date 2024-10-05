#pragma once 


#include "Lexicon/Engine/ConcretePolicy.h"

namespace LEX
{
	//TODO: Due to the existence of VoidPolicy I will need a "IsPolicyValid" function.
	// or something like that to say it's an instantiatible type.

	//Also additionally, this shit should be a singleton.
	class VoidPolicy : public ConcretePolicy
	{
	public:

		//I don't think I'd even want to do this.
		//using ConcretePolicy::ConcretePolicy;
		std::vector<ITypePolicy*> GetPostAffixedTypes() const override { return { IdentityManager::instance->GetInherentBase(InherentType::kVoidable) }; }


		virtual TypeRuleset GetRuleset() const
		{
			return TypeRuleset::NoGlobal | TypeRuleset::NoVariable | TypeRuleset::ReturnOpt;
		}

		//Should do nothing.
		VoidPolicy();

	};


	class VoidablePolicy : public ConcretePolicy
	{
	public:

		//I don't think I'd even want to do this.
		//using ConcretePolicy::ConcretePolicy;
		std::vector<ITypePolicy*> GetPostAffixedTypes() const override { return {}; }


		virtual TypeRuleset GetRuleset() const
		{
			return TypeRuleset::NoGlobal | TypeRuleset::ReturnOpt;
		}

		//Should do nothing.
		VoidablePolicy()
		{
			_id = (uint32_t)InherentType::kVoidable;

			_dataType = DataType::Interface;
		}
	};
}