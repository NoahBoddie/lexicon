#pragma once

#include "Overload.h"
#include "OverloadKey.h"

#include "QualifiedType.h"

namespace LEX
{
	//TODO: Move these to their own space, or just merge them with OverloadClause. Don't really care which.
	ENUM(WriteType)
	{
		//Mutable,	//Mutable isn't stored, instead it is a modifier that determents how the below are seen.
		Modable,	//With mutable, even through the lens of const,
			Const,
			Readonly,	//Only used on functions
	};


	inline WriteType GetWriteType(Qualifier q)
	{
		//This should just put things in a way I can use easier and filter out the shit I don't need.

		switch (q)
		{
		//case Qualifier::Modable:
		//case Qualifier::Mutable:
		case Qualifier::Const:
			return WriteType::Const;

		default:
			return WriteType::Modable;
		}
	}


	inline int CompareConstness(Qualifier lhs, Qualifier rhs)
	{
		WriteType left = GetWriteType(lhs);
		WriteType right = GetWriteType(rhs);


		return right - left;
	}

	inline size_t Alter(size_t i)
	{
		//A temporary function that says I want to shift the bits before adding it. This lends less importance to some changes than others, and prevents more resolvable ambiguities.
		return i;
	}



	struct OverloadClause : public OverloadKey
	{//The parameter side of the overload.


		virtual Overload Match(OverloadClause*, ITypePolicy* scope, Overload* prev, OverloadFlag& flag)
		{
			flag |= OverloadFlag::Ambiguous;
			return {};
		}



		//TrueName: CanMatch
		virtual bool PreEvaluate(size_t suggested, size_t optional, OverloadFlag) = 0;

		//Fuck it, these return non-booleans and use something else to denote their failures.

		//TrueNames MatchSuggestedEntry and MatchDefaultEntry
		virtual OverloadEntry EvaluateEntry2(QualifiedType, ITypePolicy* scope, size_t offset, size_t index, OverloadFlag& flags) = 0;
		virtual OverloadEntry EvaluateDefault2(QualifiedType, ITypePolicy* scope, std::string name, OverloadFlag& flags) = 0;
		//^ I'm thinking of having an extra parameter to show if I mean generic args or not.


		//This should only be called on the winnning one.
		//This should actually accept the Overload comparison object, as that can represent optional parameter pack values as well.

		//True name: ResolveEntries
		virtual std::vector<OverloadEntry> GetRemainingEvals(Overload& entries, ITypePolicy* scope, OverloadFlag& flags) = 0;
		//After this is used, it should be resized.


		//This is missing the ability to process default stuff.

		//Basically it needs the information to be able to edit
		// OverloadEntry can possibly be instructions as overload stuff. The idea being that it also outputs where a given portion of data is supposed to go.
		// To do this, I could use solutions as to not corrupt what is given.
		//But having it like this also allows for more freedom in terms of loading into shit.


		//Also, the place where OverloadInputs get loaded shouldn't try to move register data around. Instead, it should have vectors of instruction lists.
		// Then, I should free each individually.

		//virtual Void EvaluateDefault(std::vector)
	};

}