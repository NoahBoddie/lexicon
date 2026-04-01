#pragma once

#include "Lexicon/ComponentType.h"
#include "Lexicon/Interfaces/Interface.h"

#include "Lexicon/ElementType.h"

#include "Lexicon/Impl/ComponentDetails.h"

#include "Lexicon/Interfaces/IDirectoryBase.h"



namespace LEX
{
	struct IEnvironment;
	struct Environment;


	namespace Version
	{
		namespace _1
		{
			struct M_INTERFACE_VERSION(IRepositoryBase)
			{

			};

#define DEF_FUNC_IMPL_REPOSITORY_1 \
			MAP_UD(DEF_USING_IMPL,Repository)
		}

		CURRENT_VERSION(IRepositoryBase, 1);
	}


	struct IMPL_VERSION(IRepositoryBase)
	{
	public:

	};

#define DEF_FUNC_IMPL_REPOSITORY_MAIN \
	MAP_UD(DEF_USING_IMPL,Repository)

#define DECL_IMPL_FUNC_REPOSITORY /*DEF_FUNC_IMPL_REPOSITORY_MAIN DEF_FUNC_IMPL_REPOSITORY_1*/  DECL_IMPL_FUNC_DIRECTORY


}
