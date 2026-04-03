#pragma once

#include "Lexicon/Interfaces/IRepository.h"

namespace LEX
{
	class Script;
	class Project;

	struct IScript;


	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(ISubdirectory)
			{
			private:
				
			};
		}

		CURRENT_VERSION(ISubdirectory, 1);

	}

	struct IMPL_VERSION(ISubdirectory, IRepository)
	{
		DEFINE_COMPONENT_OFFSET(ComponentType::ISubdirectory, ComponentType::Subdirectory);
	public:
	
	};
}