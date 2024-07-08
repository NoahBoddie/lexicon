#include "Lexicon/ICallableUnit.h"

#include "Lexicon/RuntimeVariable.h"


namespace LEX
{
	RuntimeVariable ICallableUnit::Call(std::vector<Variable>& args)
	{
		std::vector<RuntimeVariable> t_args{ args.begin(), args.end() };
		return Invoke(t_args, nullptr);
	}

	RuntimeVariable ICallableUnit::Call(std::vector<RuntimeVariable>& args)
	{
		return Invoke(args, nullptr);
	}


}