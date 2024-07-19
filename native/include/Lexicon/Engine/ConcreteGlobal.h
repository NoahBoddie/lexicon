#pragma once

#include "Lexicon/Engine/InfoBase.h"
#include "Lexicon/Engine/GlobalBase.h"
#include "Lexicon/AbstractGlobal.h"

namespace LEX
{
	class ConcreteGlobal : public GlobalBase, public AbstractGlobal, public InfoBase
	{


	};
}