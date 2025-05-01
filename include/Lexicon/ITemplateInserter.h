#pragma once

//*src
#include "Lexicon/ITemplatePart.h"

namespace LEX
{
	struct ITemplateInserter
	{

		virtual void InsertType(ITypeInfo* type) = 0;

		void InsertTemplate(ITemplatePart* part)
		{
			auto size = part->GetSize();

			for (size_t i = 0; i < size; i++)
			{
				ITypeInfo* type = part->GetPartArgument(i);
				assert(type);
				InsertType(type);
			}
		}
	};

}