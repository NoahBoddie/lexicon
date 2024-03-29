#include "Field.h"

#include "Solution.h"
#include "QualifiedType.h"
namespace LEX
{
	QualifiedType Field::GetQualifiedType() const
	{
		QualifiedType result{};

		result.policy = GetTypePolicy();

		result.flags = GetQualifiers();

		return result;
	}


	Solution Field::AsSolution()
	{
		Solution result{ GetTypePolicy(), OperandType::Index, GetFieldIndex() };

		result.flags = GetQualifiers();


		return result;
	}
}