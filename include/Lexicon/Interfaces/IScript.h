#pragma once

#include "Interface.h"

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



				virtual Script* Promote() = 0;
				virtual const Script* Promote() const = 0;


			};

		}

		
		CURRENT_VERSION(IScript, 1);
	}

	struct IMPL_VERSION(IScript)
	{

		Script* TryPromote() { return this ? Promote() : nullptr; }
		const Script* TryPromote() const { return this ? Promote() : nullptr; }

	};
}