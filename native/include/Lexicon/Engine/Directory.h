#pragma once

#include "Lexicon/Interfaces/IDirectoryBase.h"

#include "Lexicon/Engine/Element.h"

namespace LEX
{

	struct Directory : public Element, public IDirectoryBase
	{
		DEFINE_COMPONENT_OFFSET(ComponentType::Directory)
	
	public:

		IDirectory* FindDirectory(SyntaxRecord& record) { return nullptr; };
		//std::vector<IDirectory*> GetAssociates(RelateType) = 0;

		virtual Directory* FindDirectory(SyntaxRecord& record, ITemplateInserter* inserter) = 0;


		virtual std::vector<Directory*> GetAssociates(RelateType) { return {}; }

	protected:

		Directory* GetDirectoryImpl() override
		{
			return this;
		}


	};
}