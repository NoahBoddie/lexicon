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


		RuntimeVariable Execute(api::vector<RuntimeVariable> args, Runtime* caller, RuntimeVariable* def) override
		{
			if (args->size() != GetParamCount())
				report::apply::critical("Arg size not compatible with param size ({}/{})", args->size(), GetParamCount());

			if (!1)
				VisitParameters([&](ParameterInfo& param)
					{
						//Cancelling this for now. This should be in invoke
						return;
						int i = param.GetFieldIndex();

						AbstractTypePolicy* expected = param.GetType()->FetchTypePolicy(caller);
						if (!expected)
							report::apply::critical("unexpected?");

						RuntimeVariable check = args[i]->Convert(expected);

						if (check.IsVoid() == true)
							report::apply::error("cannot convert argument into parameter {}, {} vs {}", param.GetFieldName(), i, i);

						args[i] = check;
					});


			{
				RuntimeVariable result;

				Runtime runtime{ *GetRoutine(), args, caller };

				result = runtime.Run();

				return result;
			}
		}
		

	};

}