#pragma once

#include "QualifiedType.h"
#include "OverloadKey.h"

namespace LEX
{
	QualifiedType OverloadKey::GetTarget() const
	{
		return {};
	}
}