#pragma once
#include "Lexicon/Engine/QualifiedField.h"
#include "Lexicon/Engine/Solution.h"
namespace LEX
{
	Solution QualifiedField::AsSolution()
	{
		Solution result{ GetType(), OperandType::Index, (size_t)GetFieldIndex() };

		result.flags = GetQualifiers();


		return result;
	}
}