#pragma once
#include "Lexicon/Engine/QualifiedField.h"
#include "Lexicon/Engine/Solution.h"
namespace LEX
{
	Solution QualifiedField::AsSolution()
	{
		if (GetFieldType() == FieldType::Variable)
		{
			Variable* var = dynamic_cast<Variable*>(_target);

			Solution result{ GetType(), OperandType::Variable, var };

			result.flags = GetQualifiers();

			return result;
		}

		Solution result{ GetType(), OperandType::Index, (size_t)GetFieldIndex() };

		result.flags = GetQualifiers();


		return result;
	}
}