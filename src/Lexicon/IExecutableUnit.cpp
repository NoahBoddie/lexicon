#include "Lexicon/IExecutableUnit.h"

#include "Lexicon/RuntimeVariable.h"


namespace LEX
{

	RuntimeVariable IExecutableUnit::Invoke(std::span<RuntimeVariable> args, RuntimeVariable* def)
	{
		return Execute(std::move(args), nullptr, def);
	}



}