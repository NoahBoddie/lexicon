#pragma once

#include "Lexicon/Interfaces/IElement.h"
#include "Lexicon/Interfaces/IDirectoryBase.h"

namespace LEX
{


	struct IDirectory : INTERFACE_DERIVES(IDirectoryBase, IElement)
	{
		DEFINE_COMPONENT_OFFSET(ComponentType::IDirectory)

	};
}