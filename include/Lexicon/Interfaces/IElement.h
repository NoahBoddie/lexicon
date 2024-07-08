#pragma once

#include "Interface.h"

namespace LEX
{
	class Script;
	class Project;
	struct Element;
	struct Environment;

	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(IElement)
			{
				virtual std::string_view GetName() const = 0;
				virtual Script* GetScript() = 0;
				virtual Project* GetProject() = 0;
				virtual Element* GetParent() = 0;//TODO: IElement::GetParent should be environment.
				virtual Record* GetSyntaxTree() = 0;
				virtual Environment* GetEnvironment() = 0;
			};

		}

		CURRENT_VERSION(IElement, 1);

	}

	using IElement = Version::Current::IElement;
}