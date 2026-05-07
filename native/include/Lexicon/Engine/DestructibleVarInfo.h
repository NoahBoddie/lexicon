#pragma once

#include "Lexicon/Interfaces/VarInfo.h"

namespace LEX
{
	struct DestructibleVarInfo : public VarInfo
	{
		virtual void Destroy() {}
	};
}