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
			struct INTERFACE_VERSION(IProject)
			{
			private:

			};
		}

		CURRENT_VERSION(IProject, 1);

	}

	struct IMPL_VERSION(IProject, IRepository)
	{
	public:
		DEFINE_COMPONENT_OFFSET(ComponentType::IProject, ComponentType::Project);

	private:
#ifdef LEX_SOURCE
		using script_t = Script;
#else
		using script_t = IScript;
#endif	
	public:




	private:

	};
}