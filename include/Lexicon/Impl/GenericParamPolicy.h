#pragma once

#include "TypePolicy.h"
#include "ITypePolicy.h"
#include "IGenericArgument.h"

namespace LEX
{
	struct AbstractTypePolicy;


	struct GenericParamPolicy : public ITypePolicy, public TypePolicyData
	{
		//This could change names. Instead, it could likely be GenericParamConstraint or something. because that's what its type represents, a constraint.


		//The idea is that this tells one which type this belonged to. unsure of this honestly.
		ISpecializable* _target = nullptr;

		//The idea of this is it's a type that exists only within a different type.

		//This will have to not derive from TypePolicy though, just ITypePolicy. For the same reasons that other things aren't allowed to ya know?
		//Or so I thought, but I

		uint32_t GetGenericIndex() override
		{
			return _id.GetTrivalCode();
		}

		AbstractTypePolicy* GetTypePolicy(IGenericArgument* args) override
		{
			auto index = GetGenericIndex();

			return args->GetArgAtIndex(index);
		}

		static GenericParamPolicy Create(uint32_t index)
		{
			//Create to it's owner instead, that will tell us where the parameter goes.
			return {};
		}
	};


}
