#pragma once

#include "ParamInput.h"
#include "ParameterInfo.h"



namespace LEX
{
		ParamInput::ParamInput(LocalInfo info) : 
			policy{info.GetType() },
			flags{ info.GetQualifiers() }
		{}


}