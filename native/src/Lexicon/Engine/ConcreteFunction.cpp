#include "Lexicon/Engine/ConcreteFunction.h"

#include "Lexicon/Engine/Runtime.h"

#include "Lexicon/ProcedureData.h"

namespace LEX
{
	RuntimeVariable ConcreteFunction::Execute(std::vector<RuntimeVariable>& args, Runtime* runtime, RuntimeVariable* def)
	{
		//TODO: Once arrays and the params keyword gets introduced, this will need to be implemented in other ways. Further more, could just bake this into the call.

		
		Variable* target = nullptr;

		if (args.size() != parameters.size())
			report::apply::critical("Arg size not compatible with param size ({}/{})", args.size(), parameters.size());

		size_t size = parameters.size();

		for (int i = 0; i < size; i++)
		{
			//Cancelling this for now.
			break;
			int j = i + IsMethod();

			AbstractTypePolicy* expected = parameters[i].GetType()->FetchTypePolicy(runtime);
			if (!expected)
				throw nullptr;
			RuntimeVariable check = args[j]->Convert(expected);

			if (check.IsVoid() == true)
				report::apply::critical("cannot convert argument into parameter {}, {} vs {}", parameters[i].GetFieldName(), i, j);

			args[i] = check;
		}



		RuntimeVariable result;

		if (Procedure prod = GetProcedure(); prod)
		{
			ProcedureData data;

			data.runtime = runtime;
			data.defOption = def;
			data.function = this;

			auto begin = args.begin();
			
			std::vector<Variable*> send_args;

			send_args.reserve(parameters.size());

			if (IsMethod() == true)
			{
				target = args[0].Ptr();
				begin++;
			}

			std::transform(begin, args.end(), std::back_inserter(send_args), [&](auto& it) { return it.Ptr(); });
			
			logger::critical("size check {} {}, {}", args.size(), parameters.size(), send_args.size());


			prod(result, target, send_args, data);
		}
		else
		{
			Runtime runtime{ *GetRoutine(), args };//The creation of runtime yields 2 numbers that should not exist.

			result = runtime.Run();

			//verify

			if (result.index() == 2)
				report::fault::break_warn("index is 2. Not currently handled. {}", result.index());
		
			return result;
		}

		return result;
	}



}
