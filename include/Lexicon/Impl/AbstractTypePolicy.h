#pragma once

#include "ITypePolicy.h"

namespace LEX
{
	struct IGenericArgument;

	struct AbstractTypePolicy : public ITypePolicy
	{
		//less of the idea of a type policy, and more an actual type policy. While this class cannot be instantiated, but the functionality of this
		// object is equal to that of a type policy, whatever it actually is.

		virtual bool IsAbstract() const { return true; }

		AbstractTypePolicy* GetTypePolicy(IGenericArgument* args) override
		{
			return this;
		}


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
			const TypePolicy* focus = this;

			while (focus = focus->_extends)
			{
				if (focus == from)
					return true;
			}

			return false;
			//*/
		}

		bool IsRelated(const AbstractTypePolicy* other) const
		{
			return false;


			/*

			//If the length of one is 0, this is the core object of the hierarchy. If the other isn't too, this thing is invalid.
			// Additionally, if both have the same length, neither could possibly inherit from the other.
			if (!_length || other->_length == _length)
				return false;

			//Here we check if one inherits the other by linear checking. We check the longer one because there's no way the shorter hierarchy derives
			// from one that is longer than itself.


			//STOP, this actually is incorrect, if focus is this, and we're trying to see if to derives from it,
			// (So focus is a ObjectReference, but To is Actor) it will be true that it derives from it, but not that 
			// it can turn focus into the To class.
			return other->_length > _length ? DerivedFrom(other) : other->DerivedFrom(this);

			//*/
		}
	};

}