#pragma once

#include "ObjectPolicy.h"
#include "Object.h"

namespace LEX
{
	Object ObjectPolicy::CreateObject(uint32_t context_id)
	{
		auto index = GetPolicyID();

		ObjectData data = CreateData(context_id);

		ObjectDataType type = ObjectDataType::kNone;

		if (IsPooled(data) == true) {
			data = InitializePool(data, GetStorageType());
			type = ObjectDataType::kRef;
		}
		else {
			type = GetStorageType() ? ObjectDataType::kVal : ObjectDataType::kPtr;
		}


		return Object{ data, index, type };
	};
}