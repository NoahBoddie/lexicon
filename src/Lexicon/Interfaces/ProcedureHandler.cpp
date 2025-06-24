#include "Lexicon/Interfaces/ProcedureHandler.h"

#include "Lexicon/Engine/Signature.h"
//#include "Lexicon/Engine/FunctionBase.h"
#include "Lexicon/Engine/Script.h"

//#include "Lexicon/Function.h"
#include "Lexicon/Dispatcher.h"
#include "Lexicon/ProcedureData.h"

#include "Lexicon/Engine/ConcreteFunction.h"



namespace LEX
{

	inline static std::vector<std::unique_ptr<Dispatcher>> _dispatchList{};


	bool ProcedureHandler::CheckSignatureMatch(const ISignature& base, IFunction* func)
	{

		Signature sign{ base };

		OverloadFlag flag = OverloadFlag::AllAccess;

		bool result = sign.Match(dynamic_cast<ConcreteFunction*>(func));

		//Should this fail on any step it needs to report the error to itself.

		return result;
	}

	bool ProcedureHandler::RegisterDispatch(Dispatcher* dispatch, IFunction* func)
	{
		//This type can likely produce a name actually.

		//If this thing cannot take on a procedure is what this check is.
		if (false)
			return false;

		auto dispatchee = dynamic_cast<FunctionBase*>(func);

		if (!dispatchee)
			return false;

		_dispatchList.emplace_back(std::unique_ptr<Dispatcher>{ dispatch });

		auto reciever = [](RuntimeVariable& result, Variable* target, std::span<Variable*> args, ProcedureData& data) -> void {
			auto p_data = data.function->GetProcedureData();

			//check data here.

			Dispatcher* func = reinterpret_cast<Dispatcher*>(p_data);

			if (!func) {
				//Throw exception.
				return;
			}

			func->Dispatch(result, target, args, data);
		};

		dispatchee->SetProcedureData(reciever, reinterpret_cast<uint64_t>(dispatch));

		dispatch->registered = true;

		return true;
	}

	bool ProcedureHandler::RegisterFunction(Procedure procedure, IFunction* func)
	{
		auto function = dynamic_cast<FunctionBase*>(func);

		if (!function)
			return false;

		function->SetProcedureData(procedure, 0);

		return true;
	}


	IFunction* ProcedureHandler::GetCoreFunction(std::string_view path, const ISignature& base)
	{
		auto core = ProjectManager::instance->GetCore();


		Signature sign{ base };

		auto element = core->GetElementFromPath(path, ElementType::kFuncElement, &sign);
		
		return dynamic_cast<IFunction*>(element);
	}
}