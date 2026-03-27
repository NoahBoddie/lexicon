#pragma once

#include "Lexicon/Impl/Utility.h"
#include "Lexicon/TypeInfo.hpp"
namespace LEX
{
	TypeInfo* Utility::ResolveTypeInfo(ITypeInfo* type)
	{
		return type->FetchTypePolicy(nullptr);
	}
}