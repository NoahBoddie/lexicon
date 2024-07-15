#include "Lexicon/ICallableUnit.h"

#include "Lexicon/RuntimeVariable.h"


namespace LEX
{

	RuntimeVariable ICallableUnit::Invoke(std::vector<RuntimeVariable>& args, Variable*)
	{
		return Execute(args, nullptr, nullptr);
	}



}