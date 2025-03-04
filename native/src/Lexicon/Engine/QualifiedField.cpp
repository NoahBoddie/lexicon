#include "Lexicon/Engine/QualifiedField.h"
#include "Lexicon/Engine/Solution.h"
#include "Lexicon/Engine/ConcreteGlobal.h"
namespace LEX
{
	Solution QualifiedField::AsSolution()
	{
		if (GetFieldType() == FieldType::Variable)
		{
			ConcreteGlobal* varOld = dynamic_cast<ConcreteGlobal*>(_target);

			Variable* var = dynamic_cast<Variable*>(_target);

			Solution result{ GetType(), OperandType::Variable, var };

			result.qualifiers = GetQualifiers();

			return result;
		}

		Solution result{ GetType(), OperandType::Index, (size_t)GetFieldIndex() };

		result.qualifiers = GetQualifiers();


		return result;
	}
}