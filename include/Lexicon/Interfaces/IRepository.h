#pragma once

#include "Lexicon/Interfaces/IRepositoryBase.h"
#include "Lexicon/Interfaces/IDirectory.h"

namespace LEX
{
	struct IRepository : INTERFACE_DERIVES(IRepositoryBase, IDirectory)
	{
		DEFINE_COMPONENT_OFFSET(ComponentType::IRepository, ComponentType::Repository)
	private:

	};


}