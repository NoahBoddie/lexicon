#pragma once

#include "Lexicon/Interfaces/IRepositoryBase.h"

#include "Lexicon/Engine/Directory.h"

namespace LEX
{

	struct Repository : public Directory, public IRepositoryBase
	{
		DEFINE_COMPONENT_OFFSET(ComponentType::Repository)
	public:

		//AddSubdirectory
		//GetSubdirectory
		//GetFilePath
		//FindScript
		
	};
}