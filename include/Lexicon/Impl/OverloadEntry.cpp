#pragma once

#include "OverloadEntry.h"

#include "PolicyBase.h"

namespace LEX
{
	OverloadCode OverloadCode::FinalizeOld(PolicyBase* type, PolicyBase* other)
	{
		//auto other_data = other->GetInheritData(type);

		if (other->GetInheritData(type) == nullptr) {
			return type->CreateCode(nullptr);
		}
		//Even if it's virtually inherited, that's ok.

		return other->CreateCode(type);
	}
}