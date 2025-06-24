#pragma once

#include "OverloadFlag.h"

#include "Lexicon/Engine/Overload.h"

namespace LEX
{
	struct Overload;
	struct OverloadParameter;
	struct ITypeInfo;
	struct QualifiedType;

	struct OverloadArgument
	{//The argument side of the overload.

		virtual OverloadBias Match(OverloadParameter*, ITypeInfo* scope, Overload& out, Overload* prev) = 0;

	};

}