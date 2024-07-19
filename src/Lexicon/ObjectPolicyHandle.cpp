#pragma once

#include "Lexicon/ObjectPolicyHandle.h"


#include "Lexicon/Interfaces/ObjectPolicyManager.h"

namespace LEX
{
	

	OBJECT_POLICY* ObjectPolicyHandle::get() const
	{
		if (_index == invalid_index) {
			report::error("Invalid index given: {}", _index);
			return nullptr;
		}

		return ObjectPolicyManager::instance->GetObjectPolicy(_index);
	}
}