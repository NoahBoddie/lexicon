#pragma once

#include "Lexicon/Impl/ObjectID.h"

namespace LEX
{
	struct Prototype;

	struct Hierarchy
	{
		//Should both be examples of how this should be (slightly).
		//https://github.com/CharmedBaryon/CommonLibSSE-NG/blob/main/include/RE/O/ObjectTypeInfo.h#L16
		//https://github.com/CharmedBaryon/CommonLibSSE-NG/blob/main/include/RE/T/TypeInfo.h

		//A note, hierarchy can handle conversions between non-linked types. Think int to float and vice versa
		// I think I may allow for manual ones to be found, but if a conversion exists on file, it will not be used.
		//So to put it simply, it's specific to a file and works to similar rules to extension functions, no ambiguity
		// but overriding due to something being closer to the use is allowed. This is what the function operator 'as'
		// will attempt to sort out during compiling, otherwise you know how it is.
		//HMM, don't think I like this idea too terribly, I'll parse it away, but conversion between non-linked types
		// is a thing I wish to maintain at least on the C++ level.

		//Also note, Hierarchies have to be created MANUALLY. There's no automation I can make there. Just make a solid
		// maker function.


		//This object is VERY simple, as there's really not much of a reason to go crazy with how it works.
		Hierarchy* _extends;
		uint32_t _length;//It shouldn't ever get that fucking long anyhow, and if I do this I can fit the object type in there.
		ObjectID id;


		Prototype* AsPrototype()
		{
			//Use the struct index btw.
			if (id.value[0] == 0xFF){
				//I think this should work because it's the main inheritence?
				return reinterpret_cast<Prototype*>(this);
			}

			return nullptr;
		}


		bool DerivedFrom(const Hierarchy* from) const
		{
			if (!_length || from->_length >= _length)
				return false;

			//Here we check if one inherits the other by linear checking. We check the longer one because there's no way the shorter hierarchy derives
			// from one that is longer than itself.


			//STOP, this actually is incorrect, if focus is this, and we're trying to see if to derives from it,
			// (So focus is a ObjectReference, but To is Actor) it will be true that it derives from it, but not that 
			// it can turn focus into the To class.
			const Hierarchy* focus = this;
			
			while (focus = focus->_extends)
			{
				if (focus == from)
					return true;
			}

			return false;
		}

		bool IsRelated(const Hierarchy* other) const
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

		bool CanConvertTo(Hierarchy* to)
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
	
	//Unordered if I can. Gonna re order and tend to later.
	std::map<ObjectID, Hierarchy*> TEMP(hierarchyMap);

	std::map<std::string, ObjectID> typeNameMap;

	bool operator >> (ObjectID a_lhs, ObjectID a_rhs)
	{
		if (a_lhs.IsArray() != a_rhs.IsArray())
			return false;

		Hierarchy* l_type = temp_hierarchyMap[a_lhs];
		Hierarchy* r_type = temp_hierarchyMap[a_rhs];

		return l_type->CanConvertTo(r_type);
	}


	class HierarchyManager
	{

	};
}