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


			};

		}

		
		CURRENT_VERSION(IScript, 1);
	}

	struct IMPL_VERSION(IScript){};
}