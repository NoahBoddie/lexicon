#pragma once

#include "Lexicon/Engine/QualifiedType.h"
#include "Lexicon/Engine/OverloadKey.h"

namespace LEX
{
	QualifiedType OverloadKey::GetTarget() const
	{
		return {};
	}
}