#pragma once

#include "SpecialServer.h"
#include "AbstractFunction.h"


namespace LEX
{
	struct SpecialFunction : public AbstractFunction, public SpecServer
	{
		//A notice for special function specifically. Special functions should have their own
		// type signature that isn't based on the generic one (as this use of the function isn't
		// seen as generic, it's specialized, IE, looks normal).
	};
}