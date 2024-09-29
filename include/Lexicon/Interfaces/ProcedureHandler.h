#pragma once

#include "Lexicon/Dispatcher.h"
#include "Lexicon/SignatureBase.h"

#include "Lexicon/Interfaces/ProjectManager.h"

namespace LEX
{
	struct Variable;
	struct IFunction;
	struct IRuntime;
	struct Dispatcher;
	struct ProcedureData;
	struct RuntimeVariable;

	//This exists elsewhere, please move this some how.
	using Procedure = void(*)(RuntimeVariable&, Variable* target, api::container<std::vector<Variable*>>, ProcedureData&);

	namespace Version
	{
		namespace _1
		{

			struct INTERFACE_VERSION(ProcedureHandler)
			{
				virtual bool CheckSignatureMatch(LEX::ISignature base, IFunction* func) = 0;
				virtual bool RegisterDispatch(Dispatcher* dispatch, IFunction* func) = 0;
				virtual bool RegisterFunction(Procedure procedure, IFunction* func) = 0;
			};
		}

		CURRENT_VERSION(ProcedureHandler, 1);
	}


	struct IMPL_SINGLETON(ProcedureHandler)
	{		
		bool CheckSignatureMatch(LEX::ISignature base, IFunction* func) override INTERFACE_FUNCTION;

		bool RegisterDispatch(Dispatcher* dispatch, IFunction* func) override INTERFACE_FUNCTION;

		bool RegisterFunction(Procedure procedure, IFunction* func) override INTERFACE_FUNCTION;


	INTERNAL:

		//Only internal stuff can use this version of the function.
		template <detail::function_has_var_type R, typename... Args>
		bool RegisterFunction(R(*prod)(Args...), IFunction* func)
		{
			//Currently, the only way to properly handle the system is by making a unique lambda each time a new template is created. But needless to say,
			// cant do that.

			//I think something I can do is that optionally a 64 bit context code can be given when making a procedure. This allows me to just simply pull what's needed.
			// So I think that might be a proceedure thing idk. Something to make context of. But something fast. Definitely not something like what native function
			// would have been.

			//The context thing is a good idea, it can possibly sort out which overload is being set up, if someone is using it pure (which one shouldn't.
			// Long story short it's the address to use.

			SignatureBase base{};

			//bool processed = FillSignature<true, R, Args...>(sign);
			bool processed = base.Fill<SignatureEnum::Result, R, Args...>();

			if (!processed)
				throw nullptr;

			if (CheckSignatureMatch(base, func) == false) {
				return false;
			}

			//static auto dispatch = new BasicDispatcher(func, base);
			auto result = BasicDispatcher<R, Args...>::Create(prod, func);

			return result;
		}

	public:

		//thing to get function here.

		template <typename R, typename... Args>
		bool RegisterFunction(R(*prod)(Args...), std::string_view path)
		{
			IFunction* func = ProjectManager::instance->GetFunctionFromPath(path);

			return RegisterFunction(prod, func);
		}

		bool RegisterFunction(Procedure procedure, std::string_view path)
		{
			IFunction* func = ProjectManager::instance->GetFunctionFromPath(path);

			return ProcedureHandler::instance->RegisterFunction(procedure, func);
		}
	};
}