#pragma once

#include "ITypePolicy.h"

namespace LEX
{
	class Variable;
	struct ITemplateBody;
	

	struct AbstractTypePolicy : public virtual ITypePolicy
	{	
		//This might come with hierarchy data automatically.


		//less of the idea of a type policy, and more an actual type policy. While this class cannot be instantiated, but the functionality of this
		// object is equal to that of a type policy, whatever it actually is.

		//TODO: ABSOLUTELY change the name "AbstractTypePolicy::IsAbstract". Perhaps "IsImplemented". Which this should never not be implemented.
		// Also, probably base that off something like the interface this comes from.
		bool IsResolved() const override final { return true; }

		AbstractTypePolicy* GetTypePolicy(ITemplateBody* args) override
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
		//REMOVE THIS PLEASE.
		void EmplaceDefault(Variable& var);
		void EmplaceDefault(Variable&& var) { return EmplaceDefault(var); }

		virtual AbstractTypePolicy* GetExtends() = 0;

		
	};

}