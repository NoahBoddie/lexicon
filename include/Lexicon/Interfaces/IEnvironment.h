#pragma once

#include "Interface.h"

namespace LEX
{
	struct IElement;

	struct IFunction;
	struct ITypePolicy;

	struct Environment;

	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(IEnvironment)
			{
				//TODO: It's my belief that IEnvironment is going to be deprecated

				//All of these basically use the path parser in order to find out what element it's looking for.
				virtual ITypePolicy* FindTypePath(std::string_view path) = 0;
				virtual IFunction* FindFunctionPath(std::string_view path) = 0;
				//This exclusively finds members, so I'll likely adjust it for that purpose.
				// That being said, it will still come as a field to serve as information. Globals can provide that information directly.
				// correction, finding a field like this is something that should be present in ITypePolicy, not this.
				//virtual Field* FindFieldPath(std::string_view path) = 0;
				//virtual IGlobal* FindGlobalPath(std::string_view path) = 0;
			

				virtual Environment* Promote() = 0;
				virtual const Environment* Promote() const = 0;


				Environment* TryPromote() { return this ? Promote() : nullptr; }
				const Environment* TryPromote() const { return this ? Promote() : nullptr; }

			};
		}

		CURRENT_VERSION(IEnvironment, 1);
	}

	struct IMPL_VERSION(IEnvironment)
	{
	
	};
}