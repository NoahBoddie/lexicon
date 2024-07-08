#pragma once

#include "Lexicon/Engine/InfoBase.h"
#include "Lexicon/Engine/Solution.h"

namespace LEX
{


	Solution InfoBase::AsSolution()
	{
		Solution result{ GetType(), OperandType::Index, (size_t)GetFieldIndex() };

		result.flags = GetQualifiers();


		return result;
	}
}