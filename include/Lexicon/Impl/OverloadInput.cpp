#pragma once


#include "OverloadInput.h"

#include "TargetObject.h"

namespace LEX
{

	BasicQualifier OverloadInput::GetBasicFlags() const
	{
		return object ? object->target->basicFlags : BasicQualifier::None;
	}

	RuntimeQualifier OverloadInput::GetRuntimeFlags() const
	{
		return object ? object->target->runtimeFlags : RuntimeQualifier::None;
	}

	ITypePolicy* OverloadInput::GetTarget()
	{
		return object->target->policy;
	}

}