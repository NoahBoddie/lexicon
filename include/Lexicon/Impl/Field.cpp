#include "Field.h"

#include "Solution.h"
#include "QualifiedType.h"
namespace LEX
{
	QualifiedType Field::GetQualifiedType() const
	{
		QualifiedType result{};

		result.policy = GetType();

		result.flags = GetQualifiers();

		return result;
	}


	Solution Field::AsSolution()
	{
		Solution result{ GetType(), OperandType::Index, (size_t)GetFieldIndex() };

		result.flags = GetQualifiers();


		return result;
	}
}