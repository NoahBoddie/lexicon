#pragma once

#include "Overload.h"
#include "OverloadKey.h"

#include "Lexicon/QualifiedType.h"

namespace LEX
{

	inline int CompareConstness(Qualifier lhs, Qualifier rhs)
	{
		Constness left = lhs.GetConstNormalized();
		Constness right = rhs.GetConstNormalized();

		return right - left;
	}

	inline size_t Alter(size_t i)
	{
		//A temporary function that says I want to shift the bits before adding it. This lends less importance to some changes than others, and prevents more resolvable ambiguities.
		return i;
	}



	struct OverloadClause : public OverloadKey
	{//The parameter side of the overload.


		virtual Overload Match(OverloadClause*, AbstractType* scope, Overload* prev, OverloadFlag& flag)
		{
			//This should use a similar type of match that signature uses.
			flag |= OverloadFlag::Ambiguous;
			return {};
		}



		//TrueName: CanMatch
		//QualifiedType should only be used if it is available.
		virtual bool CanMatch(QualifiedType, size_t suggested, size_t optional, OverloadFlag) = 0;

		//Fuck it, these return non-booleans and use something else to denote their failures.

		//TrueNames MatchSuggestedEntry and MatchDefaultEntry
		virtual OverloadEntry MatchSuggestedEntry(QualifiedType, AbstractType* scope, size_t offset, size_t index, OverloadFlag& flags) = 0;
		virtual OverloadEntry MatchDefaultEntry(QualifiedType, AbstractType* scope, std::string name, OverloadFlag& flags) = 0;
		//^ I'm thinking of having an extra parameter to show if I mean generic args or not.


		//This should only be called on the winnning one.
		//This should actually accept the Overload comparison object, as that can represent optional parameter pack values as well.

		//True name: ResolveEntries
		virtual std::vector<OverloadEntry> ResolveEntries(Overload& entries, AbstractType* scope, OverloadFlag& flags) = 0;
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