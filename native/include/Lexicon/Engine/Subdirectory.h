#pragma once

#include "Lexicon/Interfaces/ISubdirectory.h"

#include "Lexicon/Engine/Repository.h"

namespace LEX
{
	struct ISubdirectory;

	struct Subdirectory : public Repository, public ISubdirectory
	{
		DEFINE_COMPONENT_OFFSET(ComponentType::Subdirectory)

	protected:
		ELEM_ENUM
		{
			ELEM_FLAG(IsSubproject, 0),

			ELEM_NEXT,
		};


	public:



	};
}