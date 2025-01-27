#pragma once

#include "Lexicon/Number.h"
#include "Lexicon/Interfaces/ProcedureHandler.h"


namespace LEX
{
	struct RegisterDump
	{
		int no = 0;

		RegisterDump& operator=(bool result)
		{
			logger::info("#{} = {}", ++no, result);
			return *this;
		}
	};


	inline bool IsNAN(Number a_this)
	{
		return std::isnan(a_this.Get<double>());
	}


	
	INITIALIZE("function_register")
	{
		RegisterDump dump;

		dump = ProcedureHandler::instance->RegisterCoreFunction(IsNAN, "IsNAN");
	}

}