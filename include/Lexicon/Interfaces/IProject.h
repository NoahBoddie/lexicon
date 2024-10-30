#pragma once

#include "Lexicon/Interfaces/IElement.h"

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
				virtual void AddFormat(std::string_view name, std::string_view content, LEX::IScript* source) = 0;
			};
		}

		CURRENT_VERSION(IProject, 1);

	}

	struct IMPL_VERSION(IProject), public IElement
	{
	
	};
}