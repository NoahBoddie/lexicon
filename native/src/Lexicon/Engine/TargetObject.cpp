#include "Lexicon/Engine/TargetObject.h"

#include "Lexicon/Engine/Solution.h"
#include "Lexicon/Engine/ISpecializable.h"

namespace LEX
{
	GenericBase* TargetObject::GetClient() const
	{
		return target->policy->GetSpecializable()->FetchBase();
	}


	size_t TargetObject::GetSize() const
	{
		return target->policy->GetTemplate().size();
	}


	ITypeInfo* TargetObject::GetPartArgument(size_t i) const
	{
		return target->policy->GetTemplate()[i];
		//return _types[i];
	}

	TypeInfo* TargetObject::GetBodyArgument(size_t i) const
	{
		if (GetState())
		{
			return target->policy->GetTemplate()[i]->FetchTypePolicy(nullptr);;
		}
		return nullptr;
	}

	bool TargetObject::IsValid() const
	{
		return this && target && target->policy;
	}

	bool TargetObject::IsResolved() const
	{
		return !target || target->policy->IsResolved();
	}

}