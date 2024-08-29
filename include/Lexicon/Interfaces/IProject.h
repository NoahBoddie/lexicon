#pragma once

#include "Interface.h"

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
				virtual IScript* INT_NAME(GetCommons)() = 0;
				virtual void AddFormat(std::string_view name, std::string_view content, IScript* source) = 0;


				virtual Project* Promote() = 0;
				virtual const Project* Promote() const = 0;


			};
		}

		CURRENT_VERSION(IProject, 1);

	}

	struct IMPL_VERSION(IProject)
	{
	

		Project* TryPromote() { return this ? Promote() : nullptr; }
		const Project* TryPromote() const { return this ? Promote() : nullptr; }

	};
}