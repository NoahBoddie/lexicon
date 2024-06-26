#pragma once

#include "ProcedureHandler.h"

#include "FunctionBase.h"
#include "AbstractFunction.h"
#include "Dispatcher.h"
namespace LEX
{

	inline static std::vector<std::unique_ptr<Dispatcher>> _dispatchList{};


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

		auto reciever = [](RuntimeVariable& result, Variable* target, std::vector<Variable*> args, ProcedureData& data) -> void {
			Dispatcher* funcOld = _dispatchList[0].get();

			auto p_data = data.srcFunc->GetProcedureData();

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