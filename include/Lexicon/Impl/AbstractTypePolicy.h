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
		void EmplaceDefault(Variable& var);
		void EmplaceDefault(Variable&& var) { return EmplaceDefault(var); }

		virtual AbstractTypePolicy* GetExtends() = 0;


		bool DerivedFrom(const AbstractTypePolicy* from) const
		{
			//This should be using all it's abstracts functions. But I haven't got them yet.

			return false;

			/*
			if (!_length || from->_length >= _length)
				return false;

			//Here we check if one inherits the other by linear checking. We check the longer one because there's no way the shorter hierarchy derives
			// from one that is longer than itself.


			//STOP, this actually is incorrect, if focus is this, and we're trying to see if to derives from it,
			// (So focus is a ObjectReference, but To is Actor) it will be true that it derives from it, but not that 
			// it can turn focus into the To class.
			const ConcretePolicy* focus = this;

			while (focus = focus->_extends)
			{
				if (focus == from)
					return true;
			}

			return false;
			//*/
		}


		
		virtual bool CanConvertTo(AbstractTypePolicy* to) const
		{
			//Should use derived from in combination with conversion functions and such.
			return false;

			/*
			if (id.value[0] != to->id.value[0])
				return false;

			uint32_t to_val = to->id;

			//If it's expecting the base type with no other strings attached, so since they're in
			// the same category, free pass.
			if (to->id.GetSpecifier() == 0)
				return true;

			//Break the above into it's own sort of function

			return to->DerivedFrom(this);
			//*/



		}
	};

}