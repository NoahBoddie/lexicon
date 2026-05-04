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


	struct IDirectory;
	struct SyntaxRecord;
	enum struct RelateType;

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


#ifdef LEX_SOURCE

		virtual IDirectory* FindDirectory(SyntaxRecord& record) {return nullptr;};
		//virtual std::vector<IDirectory*> GetAssociates(RelateType) { return {}; }

#endif
	};

	#define DEF_FUNC_IMPL_DIRECTORY_MAIN \
	MAP_UD(DEF_USING_IMPL,Directory,FindDirectory,GetAssociates)

#define DECL_IMPL_FUNC_DIRECTORY /*DEF_FUNC_IMPL_DIRECTORY_MAIN DEF_FUNC_IMPL_DIRECTORY_1*/  DECL_IMPL_FUNC_ELEMENT


}
