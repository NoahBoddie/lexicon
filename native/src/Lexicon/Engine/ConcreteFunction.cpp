#include "Lexicon/Engine/ConcreteFunction.h"

#include "Lexicon/Engine/Runtime.h"

#include "Lexicon/ProcedureData.h"

namespace LEX
{
	RuntimeVariable ConcreteFunction::Execute(std::span<RuntimeVariable> args, Runtime* caller, RuntimeVariable* def)
	{
		return BasicExecute(this, nullptr, args, caller, def);
	}



}
