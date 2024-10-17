#include "Lexicon/Interfaces/ProcedureHandler.h"

#include "Lexicon/Engine/Signature.h"
#include "Lexicon/Engine/FunctionBase.h"

#include "Lexicon/AbstractFunction.h"
#include "Lexicon/Dispatcher.h"
#include "Lexicon/ProcedureData.h"
namespace LEX
{

	inline static std::vector<std::unique_ptr<Dispatcher>> _dispatchList{};


	bool ProcedureHandler::CheckSignatureMatch(ISignature base, IFunction* func)
	{

		Signature sign{ base };

		OverloadFlag flag = OverloadFlag::AllAccess;

		auto overload = sign.Match(dynamic_cast<FunctionBase*>(func), nullptr, nullptr, flag);

		bool result;

		if (flag & OverloadFlag::Failure) {
			logger::info("FAILED TO MATCH");
			result = false;
		}
		else {
			logger::info("SUCCESS TO MATCH");
			result = true;
		}
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

		auto reciever = [](RuntimeVariable& result, Variable* target, api::vector<Variable*> args, ProcedureData& data) -> void {
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
}