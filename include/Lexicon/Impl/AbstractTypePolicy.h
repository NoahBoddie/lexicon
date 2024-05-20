#pragma once

#include "ITypePolicy.h"

namespace LEX
{
	class Variable;
	struct IGenericArgument;
	

	struct AbstractTypePolicy : public virtual ITypePolicy
	{	
		//less of the idea of a type policy, and more an actual type policy. While this class cannot be instantiated, but the functionality of this
		// object is equal to that of a type policy, whatever it actually is.

		//TODO: ABSOLUTELY change the name "AbstractTypePolicy::IsAbstract". Perhaps "IsImplemented". Which this should never not be implemented.
		// Also, probably base that off something like the interface this comes from.
		virtual bool IsAbstract() const { return true; }

		AbstractTypePolicy* GetTypePolicy(IGenericArgument* args) override
		{
			return this;
		}

		ISpecializable* GetSpecializable() override
		{
			//I think I can't actually return a specialiable if it's implemented.
			return nullptr;
		}

		virtual Variable GetDefault() = 0;
		
	protected: 
		virtual void SetDefault(Variable&) = 0;
	public:
		//Make these work like an actual emplace function, where it basically does the construction elsewhere.
		// Move into function base? Largely non-basic structures depend on something else for this shit.
		void EmplaceDefault(Variable& var);
		void EmplaceDefault(Variable&& var) { return EmplaceDefault(var); }

		virtual AbstractTypePolicy* GetExtends() = 0;

		
	};

}