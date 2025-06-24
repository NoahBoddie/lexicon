#pragma once 


#include "Lexicon/Engine/ConcreteType.h"

namespace LEX
{
	//TODO: Due to the existence of VoidPolicy I will need a "IsPolicyValid" function.
	// or something like that to say it's an instantiatible type.

	//Also additionally, this shit should be a singleton.
	class VoidPolicy : public ConcreteType
	{
	public:

		//I don't think I'd even want to do this.
		//using ConcreteType::ConcreteType;
		std::vector<ITypeInfo*> GetPostAffixedTypes() const override { return { IdentityManager::instance->GetInherentType(InherentType::kVoidable) }; }


		virtual TypeRuleset GetRuleset() const
		{
			return TypeRuleset::NoGlobal | TypeRuleset::NoVariable | TypeRuleset::ReturnOpt;
		}

		//Should do nothing.
		VoidPolicy();

	};


	class VoidablePolicy : public ConcreteType
	{
	public:

		//I don't think I'd even want to do this.
		//using ConcreteType::ConcreteType;
		std::vector<ITypeInfo*> GetPostAffixedTypes() const override { return {}; }


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