#pragma once

#include "Overload.h"
#include "OverloadKey.h"

#include "Lexicon/Engine/QualifiedType.h"

namespace LEX
{

	inline int CompareConstness(Qualifier lhs, Qualifier rhs)
	{
		Constness left = lhs.GetConstness();
		Constness right = rhs.GetConstness();

		return right - left;
	}

	inline size_t Alter(size_t i)
	{
		//A temporary function that says I want to shift the bits before adding it. This lends less importance to some changes than others, and prevents more resolvable ambiguities.
		return i;
	}




	struct OverloadParameter : public OverloadArgument
	{//The parameter side of the overload.


		virtual OverloadBias Match(OverloadParameter*, ITypeInfo* scope, Overload& out, Overload* prev)
		{
			return OverloadBias::kFailure;
		}

		virtual bool CanMatch(const QualifiedType& target, size_t callArgs, size_t tempArgs, OverloadFlag) = 0;

		virtual bool MatchImpliedEntry(OverloadEntry& out, const QualifiedType& type, ITypeInfo* scope, Overload& overload, size_t index, size_t offset, OverloadFlag& flags) = 0;
		virtual bool MatchStatedEntry(OverloadEntry& out, const QualifiedType&, ITypeInfo* scope, Overload& overload, std::string_view name, OverloadFlag& flags) = 0;


		virtual void QualifyOverload(Overload& overload) {}


		virtual bool ResolveOverload(Overload& entries, OverloadFlag& flags) = 0;
	};

}