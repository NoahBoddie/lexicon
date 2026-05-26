#include "Lexicon/ObjectInfo.h"

#include "Lexicon/ObjectPolicy.hpp"

#include "Lexicon/Interfaces/IdentityManager.h"

namespace LEX
{


	uint32_t IObjectInfo::GetTypeID(const ObjectParams& data)
	{
		//auto id = GetObjectPolicy()
		
		auto id = IdentityManager::instance->GetIDFromIndex(GetObjectPolicy()->GetCategoryIndex());
		
		id += GetTypeOffset(data);

		return id;
	}


}