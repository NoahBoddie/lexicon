#pragma once

#include "Lexicon/ComponentType.h"
#include "Lexicon/Interfaces/Interface.h"

#include "Lexicon/ElementType.h"

#include "Lexicon/Impl/ComponentDetails.h"

#include "Lexicon/Interfaces/IElementBase.h"



namespace LEX
{
	struct IEnvironment;
	struct Environment;


	namespace Version
	{
		namespace _1
		{
			struct M_INTERFACE_VERSION(IDirectoryBase)
			{

			};

			#define DEF_FUNC_IMPL_DIRECTORY_1 \
			MAP_UD(DEF_USING_IMPL,Directory)
		}

		CURRENT_VERSION(IDirectoryBase, 1);
	}


	struct IMPL_VERSION(IDirectoryBase)
	{
	public:

	};

	#define DEF_FUNC_IMPL_DIRECTORY_MAIN \
	MAP_UD(DEF_USING_IMPL,Directory)

#define DECL_IMPL_FUNC_DIRECTORY /*DEF_FUNC_IMPL_DIRECTORY_MAIN DEF_FUNC_IMPL_DIRECTORY_1*/  DECL_IMPL_FUNC_ELEMENT


}
