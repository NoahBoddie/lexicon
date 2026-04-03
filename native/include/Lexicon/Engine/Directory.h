#pragma once

#include "Lexicon/Interfaces/IDirectoryBase.h"

#include "Lexicon/Engine/Element.h"

namespace LEX
{

	struct Directory : public Element, public IDirectoryBase
	{
		DEFINE_COMPONENT_OFFSET(ComponentType::Directory);



		//virtual Directory* FindDirectory(SyntaxRecord& record, ITemplateInserter& inserter) { return nullptr; }
	};
}