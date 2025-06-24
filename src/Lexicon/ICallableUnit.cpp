#include "Lexicon/ICallableUnit.h"

#include "Lexicon/RuntimeVariable.h"


namespace LEX
{

	RuntimeVariable ICallableUnit::Invoke(std::span<RuntimeVariable> args, RuntimeVariable* def)
	{
		return Execute(std::move(args), nullptr, def);
	}



}