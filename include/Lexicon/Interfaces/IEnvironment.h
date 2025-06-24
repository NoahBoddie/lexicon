#pragma once

#include "Interface.h"

namespace LEX
{
	struct IElement;

	struct IFunction;

	struct Environment;

	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(IEnvironment)
			{
				//TODO: It's my belief that IEnvironment is going to be deprecated

				//Nevermind, I want to use this again.


				//All of these basically use the path parser in order to find out what element it's looking for.
				virtual ITypeInfo* FindTypePath(std::string_view path) = 0;
				virtual IFunction* FindFunctionPath(std::string_view path) = 0;
				//This exclusively finds members, so I'll likely adjust it for that purpose.
				// That being said, it will still come as a field to serve as information. Globals can provide that information directly.
				// correction, finding a field like this is something that should be present in ITypeInfo, not this.
				//virtual Field* FindFieldPath(std::string_view path) = 0;
				//virtual IGlobal* FindGlobalPath(std::string_view path) = 0;
			};
		}

		CURRENT_VERSION(IEnvironment, 1);
	}

	struct IMPL_VERSION(IEnvironment){};
}