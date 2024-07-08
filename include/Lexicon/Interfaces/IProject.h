#pragma once

#include "Interface.h"

namespace LEX
{
	class Script;

	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(IProject)
			{
				virtual Script* GetCommons() = 0;
				virtual void AddFormat(std::string_view name, std::string_view content, Script* source) = 0;
			};
		}

		CURRENT_VERSION(IProject, 1);

	}

	using IProject = Version::Current::IProject;
}