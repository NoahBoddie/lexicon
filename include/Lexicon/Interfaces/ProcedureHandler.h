#pragma once

#include "InterfaceSingleton.h"

namespace LEX
{
	struct Variable;
	struct IFunction;
	struct Dispatcher;
	struct ProcedureData;
	struct RuntimeVariable;

	//This exists elsewhere, please move this some how.
	using Procedure = void(*)(RuntimeVariable&, Variable* target, std::vector<Variable*>, ProcedureData&);

	namespace Version
	{
		namespace _1
		{

			struct INTERFACE_VERSION(ProcedureHandler)
			{
				virtual bool RegisterDispatch(Dispatcher* dispatch, IFunction* func) = 0;
				virtual bool RegisterFunction(Procedure procedure, IFunction * func) = 0;
			};
		}

		CURRENT_VERSION(ProcedureHandler, 1);
	}


	struct IMPL_SINGLETON(ProcedureHandler)
	{		
		bool RegisterDispatch(Dispatcher* dispatch, IFunction* func) override INTERFACE_FUNCTION;

		bool RegisterFunction(Procedure procedure, IFunction* func) override INTERFACE_FUNCTION;
	};


}