#include "Lexicon/Engine/ConcreteFunction.h"

#include "Lexicon/Engine/Runtime.h"

#include "Lexicon/ProcedureData.h"

namespace LEX
{
	RuntimeVariable ConcreteFunction::Execute(api::vector<RuntimeVariable> args, Runtime* runtime, RuntimeVariable* def)
	{
		if (IsValid() == false) {
			report::log("Function '{}' is not valid. Maybe say error later.", IssueType::Apply, def ? IssueLevel::Failure : IssueLevel::Error, GetName());
			
			//disable this warning.

			return *def;
		}


		Variable* target = nullptr;

		if (args->size() != GetParamCount())
			report::apply::critical("Arg size not compatible with param size ({}/{})", args->size(), parameters.size());



		VisitParameters([&](ParameterInfo& param)
		{

			//Cancelling this for now. It should be used in Invoke, rather than here.
			return;
			int i = param.GetFieldIndex();

			AbstractTypePolicy* expected = param.GetType()->FetchTypePolicy(runtime);


			if (!expected)
				report::apply::critical("unexpected?");

			RuntimeVariable check = args[i]->Convert(expected);

			if (check.IsVoid() == true)
				report::apply::critical("cannot convert argument into parameter {}, {} vs {}", param.GetFieldName(), i, i);

			args[i] = check;
		});



		RuntimeVariable result;

		if (Procedure prod = GetProcedure(); prod)
		{
			ProcedureData data;
			
			data.runtime = runtime;
			data.defOption = def;
			data.function = this;

			auto begin = args->begin();
			
			std::vector<Variable*> send_args;
			
			send_args.reserve(GetArgCount());
			
			if (IsMethod() == true)
			{
				target = args[0].Ptr();
				begin++;
			}
			
			std::transform(begin, args->end(), std::back_inserter(send_args), [&](auto& it) { return it.Ptr(); });

			report _{ IssueType::Runtime };
			
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
