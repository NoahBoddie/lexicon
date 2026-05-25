#pragma once

#include "Lexicon/ObjectPolicy.h"
#include "Lexicon/TypeInfo.h"
#include "Lexicon/Object.h"

namespace LEX
{
	Object IObjectPolicy::CreateObject(TypeInfo* a_type)
	{
		auto index = GetPolicyID();

		ObjectData data = CreateData(a_type);

		ObjectDataType type = ObjectDataType::kNone;

		//auto offset = GetTypeOffset(data);
		//if (offset < (uint16_t)-1) {
		//	offset = 0;
		//}

		if (IsPooled(a_type) == true) {
			data = InitializePool(data, GetStorage() == ObjectStorage::Pointer);
			type = ObjectDataType::kRef;
		}
		else {
			type = GetStorage() == ObjectStorage::Value ? ObjectDataType::kVal : ObjectDataType::kPtr;
		}


		//return Object{ data, index, type, (uint16_t)offset };
		return Object{ data, index, type };
	}

	Object IObjectPolicy::CreateDefault(TypeInfo* a_type)
	{
		auto index = GetPolicyID();

		if (IsPooled(a_type) == true) {
			return Object{ ObjectData{}, index, ObjectDataType::kNone };
		}

		return CreateObject(a_type);
	}


	ITypeInfo* IObjectPolicy::GetTypeInfo(TypeOffset offset)
	{
		auto id = GetTypeIDFromOffset(offset);
		return IdentityManager::instance->GetTypeByID(id);

	}
}