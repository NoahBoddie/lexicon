#include "Lexicon/ObjectInfo.h"

#include "Lexicon/ObjectPolicy.h"

#include "Lexicon/Interfaces/IdentityManager.h"

namespace LEX
{


	uint32_t ObjectVTable::GetTypeID(ObjectData& data)
	{
		//auto id = GetObjectPolicy()
		
		auto id = IdentityManager::instance->GetIDFromIndex(GetObjectPolicy()->GetCategoryIndex());
		
		id += GetTypeOffset(data);

		return id;
	}


}