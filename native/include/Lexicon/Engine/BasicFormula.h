#pragma once

#include "Lexicon/IFormula.h"

#include "Lexicon/Engine/FunctionData.h"


//*src
#include "Lexicon/Engine/Runtime.h"

namespace LEX
{

	class BasicFormula : public IFormula, public BasicCallableData
	{
		//This version is obscured for the user. It should help inline functions and such into the code, or that can be given to others to run.
		//Formula rules. 
		// They don't have default parameters, 
		// they don't have procedures. 
		// They don't have names. Rather they have handles and hashes.
		// They don't have targets (SCRATCH, they now have targets)

		//Think this should probably store the string that it came from perhaps. Could be useful.


		RuntimeVariable Execute(api::vector<RuntimeVariable> args, Runtime* runtime, RuntimeVariable* def) override
		{
			if (args->size() != parameters.size())
				report::apply::critical("Arg size not compatible with param size ({}/{})", args->size(), parameters.size());

			size_t size = parameters.size();

			if (!1)
			for (int i = 0; i < size; i++) {
				//Cancelling this for now.
				break;
				int j = i;

				AbstractTypePolicy* expected = parameters[i].GetType()->FetchTypePolicy(runtime);
				if (!expected)
					throw nullptr;
				RuntimeVariable check = args[j]->Convert(expected);

				if (check.IsVoid() == true)
					report::apply::critical("cannot convert argument into parameter {}, {} vs {}", parameters[i].GetFieldName(), i, j);

				args[i] = check;
			}

			{
				RuntimeVariable result;

				Runtime _runtime{ *GetRoutine(), args };

				result = _runtime.Run();

				return result;
			}
		}
		

	};

}