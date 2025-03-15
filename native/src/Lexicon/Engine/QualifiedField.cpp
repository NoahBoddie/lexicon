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

			Solution result{ QualifiedType{GetType()}, OperandType::Variable, var };
			
			result = GetQualifiers();

			return result;
		}

		Solution result{ QualifiedType{GetType()}, OperandType::Index, (size_t)GetFieldIndex() };

		result = GetQualifiers();


		return result;
	}
}