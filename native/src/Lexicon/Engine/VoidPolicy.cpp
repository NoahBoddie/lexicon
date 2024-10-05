#pragma once

#include "Lexicon/Engine/VoidPolicy.h"

namespace LEX
{
	VoidPolicy::VoidPolicy()
	{
		//This should be attempting to claim 0 for itself.

		//_id = (uint32_t)InherentType::kVoid;

		_dataType = DataType::Invalid;
	}
}