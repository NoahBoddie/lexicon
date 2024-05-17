#pragma once


#include "OverloadInput.h"

#include "TargetObject.h"

namespace LEX
{

	Qualifier OverloadInput::GetQualifiers() const
	{
		return object ? object->target->flags : Qualifier::None;
	}


	ITypePolicy* OverloadInput::GetTarget()
	{
		return object && object->target ? object->target->policy : nullptr;
	}

}