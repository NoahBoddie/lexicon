#include "Field.h"

#include "Solution.h"

namespace LEX
{



	Solution Field::AsSolution()
	{
		Solution result{ GetTypePolicy(), OperandType::Index, GetFieldIndex() };

		result.flags = GetQualifiers();


		return result;
	}
}