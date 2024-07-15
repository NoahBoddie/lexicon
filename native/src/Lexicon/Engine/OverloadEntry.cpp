#pragma once

#include "Lexicon/Engine/OverloadEntry.h"

#include "Lexicon/Engine/PolicyBase.h"

namespace LEX
{
	OverloadCode OverloadCode::FinalizeOld(HierarchyData* type, HierarchyData* other)
	{
		//auto other_data = other->GetInheritData(type);

		if (other->GetInheritData(type->GetHierarchyType()) == nullptr) {
			return type->CreateCode(nullptr);
		}
		//Even if it's virtually inherited, that's ok.

		return other->CreateCode(type->GetHierarchyType());
	}
}