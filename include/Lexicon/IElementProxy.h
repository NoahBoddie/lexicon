#pragma once

#include "Lexicon/Interfaces/Interface.h"

namespace LEX
{
	struct IElement;

	struct IElementProxy : public Interface
	{
		virtual IElement* base() = 0;
		virtual const IElement* base() const = 0;

	};
}