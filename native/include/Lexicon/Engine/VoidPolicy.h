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

		std::string_view GetName() const
		{
			return "void";
		}


		virtual TypeRuleset GetRuleset() const
		{
			return TypeRuleset::NoGlobal | TypeRuleset::NoVariable | TypeRuleset::ReturnOpt;
		}

		//Should do nothing.
		VoidPolicy() : ConcreteType{ InherentType::kVoid } 
		{
			_dataType = DataType::Invalid;
		}

	};


	class VoidablePolicy : public ConcreteType
	{
	public:

		//I don't think I'd even want to do this.
		//using ConcreteType::ConcreteType;
		std::vector<ITypeInfo*> GetPostAffixedTypes() const override { return {}; }

		std::string_view GetName() const
		{
			return "voidable";
		}


		virtual TypeRuleset GetRuleset() const
		{
			return TypeRuleset::NoGlobal | TypeRuleset::ReturnOpt;
		}

		VoidablePolicy() : ConcreteType{ InherentType::kVoidable } 
		{
			_dataType = DataType::Interface;
		}
	};
}