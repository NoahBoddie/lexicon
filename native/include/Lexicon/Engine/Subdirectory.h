#pragma once

#include "Lexicon/Interfaces/ISubdirectory.h"

#include "Lexicon/Engine/Repository.h"

namespace LEX
{
	struct ISubdirectory;

	struct Subdirectory : public Repository, public ISubdirectory
	{
		DEFINE_COMPONENT_OFFSET(ComponentType::Subdirectory)
	public:

	};
}