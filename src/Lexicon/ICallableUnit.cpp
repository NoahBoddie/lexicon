#include "Lexicon/ICallableUnit.h"

#include "Lexicon/RuntimeVariable.h"


namespace LEX
{

	RuntimeVariable ICallableUnit::Invoke(api::vector<RuntimeVariable> args, Variable*)
	{
		return Execute(std::move(args), nullptr, nullptr);
	}



}