#pragma once

#include "Lexicon/ComponentType.h"
#include "Lexicon/Interfaces/Interface.h"
#include "Lexicon/Interfaces/ComponentDetails.h"
#include "Lexicon/Interfaces/IDirectoryBase.h"

namespace LEX
{
	struct IElement;

	struct IFunction;

	struct Environment;

	namespace Version
	{
		namespace _1
		{
			struct M_INTERFACE_VERSION(IEnvironmentBase)
			{
				
				//virtual ITypeInfo* FindTypePath(std::string_view path) = 0;
				//virtual IFunction* FindFunctionPath(std::string_view path) = 0;

				//All of these basically use the path parser in order to find out what element it's looking for.
				
				//This exclusively finds members, so I'll likely adjust it for that purpose.
				// That being said, it will still come as a field to serve as information. Globals can provide that information directly.
				// correction, finding a field like this is something that should be present in ITypeInfo, not this.
				//virtual Field* FindFieldPath(std::string_view path) = 0;
				//virtual IGlobal* FindGlobalPath(std::string_view path) = 0;
			};
			#define DEF_FUNC_IMPL_ENVIRONMENT_1 \
			MAP_UD(DEF_USING_IMPL,Environment,\
			)
		}

		CURRENT_VERSION(IEnvironmentBase, 1);
	}



	struct IMPL_VERSION(IEnvironmentBase)
	{
		
	};


	#define DEF_FUNC_IMPL_ENVIRONMENT_MAIN \
	MAP_UD(DEF_USING_IMPL,Environment,\
	)

#define DECL_IMPL_FUNC_ENVIRONMENT /*DEF_FUNC_IMPL_ENVIRONMENT_MAIN DEF_FUNC_IMPL_ENVIRONMENT_1*/  DECL_IMPL_FUNC_DIRECTORY

}