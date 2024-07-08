#pragma once

#include "Lexicon/QualifiedType.h"
#include "Lexicon/Engine/OverloadKey.h"

namespace LEX
{
	QualifiedType OverloadKey::GetTarget() const
	{
		return {};
	}
}