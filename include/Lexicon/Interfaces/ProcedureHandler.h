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
	using Procedure = void(*)(RuntimeVariable&, Variable* target, api::vector<Variable*>, ProcedureData&);

	namespace Version
	{
		namespace _1
		{

			struct INTERFACE_VERSION(ProcedureHandler)
			{
				virtual bool CheckSignatureMatch(const LEX::ISignature& base, IFunction* func) = 0;
				virtual bool RegisterDispatch(Dispatcher* dispatch, IFunction* func) = 0;//This isn't supposed to be visible
				virtual bool RegisterFunction(Procedure procedure, IFunction* func) = 0;
			};
		}

		CURRENT_VERSION(ProcedureHandler, 1);
	}


	struct IMPL_SINGLETON(ProcedureHandler)
	{		
		bool CheckSignatureMatch(const LEX::ISignature& base, IFunction* func) override INTERFACE_FUNCTION;

		bool RegisterDispatch(Dispatcher* dispatch, IFunction* func) override INTERFACE_FUNCTION;

		bool RegisterFunction(Procedure procedure, IFunction* func) override INTERFACE_FUNCTION;

		/*
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

	INTERNAL:

		//I'd like to just program this into the project manager at some point.
		IFunction* GetCoreFunction(std::string_view path);


		//These allow for the registration of functions that exist within core files.
		template <typename R, typename... Args>
		bool RegisterCoreFunction(R(*prod)(Args...), std::string_view path)
		{
#ifdef LEX_SOURCE

			IFunction* func = GetCoreFunction(path);

			return RegisterFunction(prod, func);
#else
			report::apply::warn("Non-source binaries cannot register core functions.");
			return false;
#endif
		}

		bool RegisterCoreFunction(Procedure procedure, std::string_view path)
		{
#ifdef LEX_SOURCE

			IFunction* func = GetCoreFunction(path);

			return ProcedureHandler::instance->RegisterFunction(procedure, func);
#else
			report::apply::warn("Non-source binaries cannot register core functions.");
			return false;
#endif			
		}

		//*/


		//////////////////////////////////////////////

	private:
		template <detail::function_has_var_type R, typename... Args>
		bool RegisterFunctionImpl(R(*prod)(Args...), const ISignature& base, IFunction* func)
		{
			if (!func) {
				return false;
			}

			if (CheckSignatureMatch(base, func) == false) {
				return false;
			}

			auto result = BasicDispatcher<R, Args...>::Create(prod, func);

			return result;
		}

	INTERNAL:
		template <detail::function_has_var_type R, typename... Args>
		bool RegisterFunction(R(*prod)(Args...), IFunction* func)
		{

			ISignature base{};

			//bool processed = FillSignature<true, R, Args...>(sign);
			bool processed = base.Fill<SignatureEnum::Result, R, Args...>();

			if (!processed) {
				report::link::warn("Signature of function '{}' failed to be created.", typeid(decltype(prod)).name());
				return false;
			}

			return RegisterFunctionImpl(prod, base, func);
		}


	public:

		//Only internal stuff can use this version of the function.
		template <detail::function_has_var_type R, typename... Args>
		bool RegisterFunction(R(*prod)(Args...), std::string_view path)
		{
			ISignature base{};

			//bool processed = FillSignature<true, R, Args...>(sign);
			bool processed = base.Fill<SignatureEnum::Result, R, Args...>();

			if (!processed) {
				report::link::warn("Signature of function '{}' failed to be created.", typeid(decltype(prod)).name());
				return false;
			}


			IFunction* func = ProjectManager::instance->GetFunctionFromPath(path, base);

			return RegisterFunctionImpl(prod, base, func);
		}

		bool RegisterFunction(Procedure procedure, std::string_view path, const ISignature& sign)
		{
			IFunction* func = ProjectManager::instance->GetFunctionFromPath(path, sign);

			return ProcedureHandler::instance->RegisterFunction(procedure, func);
		}
		

	INTERNAL:

		//I'd like to just program this into the project manager at some point.
		IFunction* GetCoreFunction(std::string_view path, const ISignature& base);


		//These allow for the registration of functions that exist within core files.
		template <typename R, typename... Args>
		bool RegisterCoreFunction(R(*prod)(Args...), std::string_view path)
		{
#ifdef LEX_SOURCE

			ISignature base{};

			//bool processed = FillSignature<true, R, Args...>(sign);
			bool processed = base.Fill<SignatureEnum::Result, R, Args...>();

			if (!processed) {
				report::link::warn("Signature of core function '{}' failed to be created.", typeid(decltype(prod)).name());
				return false;
			}

			IFunction* func = GetCoreFunction(path, base);

			return RegisterFunction(prod, func);
#else
			report::apply::warn("Non-source binaries cannot register core functions.");
			return false;
#endif
		}

		bool RegisterCoreFunction(Procedure prod, std::string_view path, const ISignature& sign)
		{
#ifdef LEX_SOURCE
			IFunction* func = GetCoreFunction(path, sign);

			return RegisterFunction(prod, func);
#else
			report::apply::warn("Non-source binaries cannot register core functions.");
			return false;
#endif			
		}

	};
}