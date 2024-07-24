#pragma once

#include "Lexicon/ITemplateInsertPart.h"

//*src
#include "Lexicon/ITypePolicy.h"

namespace LEX
{
	struct ITemplateBody;

	struct ITemplateInsertBody : public ITemplateInsertPart
	{

		void InsertType(ITypePolicy* part) override
		{
			auto back = part->GetTypePolicy((ITemplateBody*)nullptr);

			assert(back);

			return InsertType(back);
		}

		virtual void InsertType(AbstractTypePolicy* body) = 0;

		ITemplateInsertBody* TryPromoteInserter() override { return this; }
	};

}