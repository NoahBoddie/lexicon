#pragma once

#include "Lexicon/Interfaces/IElement.h"
#include "Lexicon/Interfaces/IDirectoryBase.h"

namespace LEX
{
	struct SyntaxRecord;
	enum struct RelateType;

	struct IDirectory : INTERFACE_DERIVES(IDirectoryBase, IElement)
	{
		DEFINE_COMPONENT_OFFSET(ComponentType::IDirectory, ComponentType::Directory)
	public:


#ifdef LEX_SOURCE

		IDirectory* FindDirectory(SyntaxRecord& record) override;
		//std::vector<IDirectory*> GetAssociates(RelateType) override;

#endif
	};
}

