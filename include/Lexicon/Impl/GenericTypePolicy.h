#pragma once

#include "TypePolicy.h"
#include "ISpecializable.h"
#include "SpecialTypePolicy.h"

namespace LEX
{

	//I'm just putting this here temporarily because it's gonna be interacted with the most, 
	// but all generics need to reimplement their Get functions to search for a specialization when
	// given args.
	//Or might not be needed.
	struct GenericTypePolicy : public TypePolicy, public ISpecializable 
	{
		//JUST type policies can't be declared within a class scope, so no Specializable parent.

		ISpecializable* GetParentSpecializable() { return nullptr; }

		//This needs to not derive from AbstractTypePolicy, NOR TypePolicyData. Because this might extend from an ITypePolicy instead of a real type policy.
		// This is because unspecialized generics can be a base class.
		SpecServer* MakeServer(IGenericArgument& args) override
		{
			//Not actually proper, this is just what it sends.
			return new SpecialTypePolicy();
		}
	};
}