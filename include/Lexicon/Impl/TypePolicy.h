#pragma once


#include "TypeID.h"
#include "AbstractTypePolicy.h"

namespace LEX
{
	class TypePolicy;
	class TypeInfo;

	enum struct TypeEnum
	{
		Invalid,
		Primitive,
		Intrinsic,
		External,
		Custom,
		Data,
		Bound, 
		Signature,
	};

	struct TypePolicyData
	{
		AbstractTypePolicy* _extends;//This might need to be an ITypePolicy, in the case of deriving from another generic class.
		uint32_t _length;//It shouldn't ever get that fucking long anyhow, and if I do this I can fit the object type in there.
		TypeID _id;


		AbstractTypePolicy* extends()
		{
			return _extends;
		}
	};

	class TypePolicy : public AbstractTypePolicy, public TypePolicyData
	{
		virtual bool IsAbstract() const { return false; }

		virtual TypeEnum GetTypeEnum() = 0;

		//Technically, should be in abstract, but abstract actually is supposed to handle this so shrug.
		//virtual TypeInfo* AsTypeInfo() { return nullptr; }

		//When loaded and not trival, added to a map. Should have a function called SetTypeID and that should handle
		// it. Should have a class called TypeIDHandler handle it.
	};


	struct TypePolicy_Real
	{
		//Note, you will still need to be able to find one of these by ID alone.


		//Refer to hierarchy for documentation.

		//This object is VERY simple, as there's really not much of a reason to go crazy with how it works.
		TypePolicy* _extends;
		uint32_t _length;//It shouldn't ever get that fucking long anyhow, and if I do this I can fit the object type in there.
		TypeID id;

		std::optional<Variable> default_value;//Only defined types will need something like this, or linked objects will, otherwise there's a constructor to do it.

		TypeInfo* AsPrototype()
		{
			//Use the struct index btw.
			if (id.value[0] == 0xFF) {
				//I think this should work because it's the main inheritence?
				return reinterpret_cast<TypeInfo*>(this);
			}

			return nullptr;
		}


		bool DerivedFrom(const TypePolicy* from) const
		{
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
		}

		bool IsRelated(const TypePolicy* other) const
		{

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
		}

		bool CanConvertTo(TypePolicy* to)
		{
			//I think I want this to fail so if need be I can flush it out.
			//if (!to)
			//	return false;

			//This is kept in some kind of map in association with the type map.
			// How we find out if one is like the other, we first get the codes. Do their index bytes match? 
			// If not they are incompatible types. (I can use this concept for literals by having it be the byte under it that does this job).
			// Before we check for failure, an immediate pass is if the GoingTo Hierachy is the base (so XX000000). Should make an easy function to do that.
			// Then we check their lengths. If they are both the same then there's no way one could inherit from the other.
			// If it does, we check if any of the extends of the longer one are equal to the hierarchy here.
			//And that's how conversion is measured.

			//Note, the last part MAY not exactly be accurate.

			if (id.value[0] != to->id.value[0])
				return false;

			uint32_t to_val = to->id;

			//If it's expecting the base type with no other strings attached, so since they're in
			// the same category, free pass.
			if (to->id.GetSpecifier() == 0)
				return true;

			//Break the above into it's own sort of function

			return to->DerivedFrom(this);




		}
	};


	//Very temp name and temp idea.
	struct PolicyEnvironment : public Environment, public TypePolicy
	{
		//The idea of this is to be a pivot for environments that are also hierarchies, structs, classes, and enums (even if enums can't derive from anything)
		Record* _syntaxNode;

		TypePolicy* AsPolicy()//override
		{
			return this;
		}

		std::string GetName() override
		{
			//Should this throw an exception isntead?
			return "";
		}
	};
}