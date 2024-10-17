#pragma once

#include "Lexicon/IElementProxy.h"

namespace LEX
{
	class Script;

	namespace Version
	{
		namespace _1
		{

			struct INTERFACE_VERSION_DERIVES(IScript, IElementProxy)
			{
				//Relationship information

				virtual bool AppendContent(std::string_view content, api::vector<std::string_view> options = {}) = 0;
			};

		}

		
		CURRENT_VERSION(IScript, 1);
	}

	struct IMPL_VERSION(IScript){};
}