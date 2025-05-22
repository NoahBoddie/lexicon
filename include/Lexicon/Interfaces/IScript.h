#pragma once

#include "Lexicon/Interfaces/IElement.h"

namespace LEX
{
	class Script;

	namespace Version
	{
		namespace _1
		{

			struct INTERFACE_VERSION(IScript)
			{
				//Relationship information

				virtual bool AppendContent(const std::string_view& content, std::span<std::string_view> options = {}) = 0;
			};

		}

		
		CURRENT_VERSION(IScript, 1);
	}

	struct IMPL_VERSION(IScript), public IElement
	{

	};
}