#pragma once

#include "Lexicon/Dispatcher.h"

#include "Lexicon/Interfaces/ProcedureHandler.h"
#include "Lexicon/Function.h"

namespace LEX
{
	bool Dispatcher::TryRegister(Dispatcher* dispatch, IFunction* func)
	{
		return ProcedureHandler::instance->RegisterDispatch(dispatch, func);
	}


	
}