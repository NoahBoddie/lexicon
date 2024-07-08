#pragma once

#include "Interface.h"

namespace LEX
{
	namespace Version
	{
		namespace _1
		{

			struct INTERFACE_VERSION(IScript)
			{
				//Relationship information
			};

		}

		
		CURRENT_VERSION(IScript, 1);
	}

	using IScript = Version::Current::IScript;
}