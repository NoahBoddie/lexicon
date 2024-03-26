#pragma once

#include "ParamInput.h"
#include "ParameterInfo.h"



namespace LEX
{
		ParamInput::ParamInput(LocalInfo info) : 
			policy{info.GetTypePolicy() },
			flags{ info.GetQualifiers() }
		{}


}