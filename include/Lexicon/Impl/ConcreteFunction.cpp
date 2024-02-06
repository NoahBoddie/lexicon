#include "ConcreteFunction.h"

#include "Runtime.h"

namespace LEX
{
	RuntimeVariable ConcreteFunction::Invoke(std::vector<RuntimeVariable>& args, RuntimeVariable*)
	{

		Runtime runtime{ *GetRoutine(), args};//The creation of runtime yields 2 numbers that should not exist.

		RuntimeVariable result = runtime.Run();

		//verify

		return result;
	}

}
