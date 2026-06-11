#pragma once

#include "Lexicon/Dispatcher.h"
#include "Lexicon/SignatureBase.h"
#include "Lexicon/IFunction.h"

#include "Lexicon/Interfaces/ProjectManager.h"
#include "Lexicon/Interfaces/DirectoryManager.h"

namespace LEX
{
	struct Variable;
	struct IFunction;
	struct IRuntime;
	struct Dispatcher;
	struct ProcedureData;
	class RuntimeVariable;

	//This exists elsewhere, please move this some how.
	using Procedure = void(*)(RuntimeVariable&, Variable* target, std::span<Variable*>, ProcedureData&);

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
	INTERNAL:
		bool CheckSignatureMatch(const LEX::ISignature& base, IFunction* func) override INTERFACE_FUNCTION;

		bool RegisterDispatch(Dispatcher* dispatch, IFunction* func) override INTERFACE_FUNCTION;

		bool RegisterFunction(Procedure procedure, IFunction* func) override INTERFACE_FUNCTION;

	private:
		template <detail::function_has_var_type R, typename... Args>
		bool RegisterFunctionImpl(R(*prod)(Args...), const ISignature& base, IFunction* func)
		{
			if (!func) {
				return false;
			}

#ifndef LEX_SOURCE
			auto script = func->GetScript();

			assert_if (script && script->IsCoreScript() == true) {
				report::link::failure("Non-source binaries cannot register core functions.");
				return false;
			}
#endif
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



		bool RegisterDirectFunction(Procedure procedure, IFunction* func)
		{
			if (!func) {
				return false;
			}

#ifndef LEX_SOURCE
			auto script = func->GetScript();

			assert_if(script && script->IsCoreScript() == true) {
				report::link::failure("Non-source binaries cannot register core functions.");
				return false;
			}
#endif

			return RegisterFunction(procedure, func);
		}


		template <is_not<StaticTargetTag> T, typename... Args>
		bool RegisterConstructor(void(*prod)(T, Args...), std::string_view path)
		{
			ISignature base{};

			//bool processed = FillSignature<true, R, Args...>(sign);
			bool processed = base.Fill<SignatureEnum::Result, T, T, Args...>();

			if (!processed) {
				report::link::warn("Signature of constructor '{}' failed to be created.", typeid(decltype(prod)).name());
				return false;
			}


			IFunction* func = DirectoryManager::instance->GetConstructorFromPath(nullptr, path, &base);

			return RegisterFunctionImpl(prod, base, func);
		}


		bool RegisterFunction(Procedure procedure, std::string_view path, const ISignature& sign)
		{
			IFunction* func = ProjectManager::instance->GetFunctionFromPath(path, sign);

			return ProcedureHandler::instance->RegisterFunction(procedure, func);
		}


	};
}