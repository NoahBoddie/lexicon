#pragma once

#include "SpecialServer.h"
#include "AbstractTypePolicy.h"


namespace LEX
{
	//Ive fucked up the naming of this, but this is the thing that goes in GenericTypePolicy. You can tell because it derives abstract type policy.
	// And while it's not a real type policy, it is a stand-in for one.
	struct SpecialTypePolicy : public AbstractTypePolicy, public SpecServer
	{

	};
}