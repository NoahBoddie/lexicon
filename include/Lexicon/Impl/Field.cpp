#include "Field.h"

#include "Solution.h"

namespace LEX
{



	Solution Field::AsSolution()
	{
		Solution result{ GetTypePolicy(), OperandType::Index, GetFieldIndex() };

		result.basicFlags = GetBasicFlags();
		result.runtimeFlags = GetRuntimeFlags();

		return result;
	}
}