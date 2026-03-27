#pragma once

#include "Lexicon/Engine/IDirectoryImpl.h"

#include "Lexicon/Engine/Element.h"

namespace LEX
{

	struct Directory : public Element, public IDirectoryBase
	{
		DEFINE_COMPONENT_OFFSET(ComponentType::Directory);

	};
}