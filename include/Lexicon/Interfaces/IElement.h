#pragma once

#include "Interface.h"

namespace LEX
{
	class Script;
	class Project;
	struct Element;
	struct Environment;
	struct ITypePolicy;
	struct IFunction;
	struct IGlobal;

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

				virtual ITypePolicy* AsType() { return nullptr; }
				virtual IFunction* AsFunction() { return nullptr; }
				virtual IGlobal* AsGlobal() { return nullptr; }

				ITypePolicy* ToType() { return this ? AsType() : nullptr; }
				IFunction* ToFunction() { return this ? AsFunction() : nullptr; }
				IGlobal* ToGlobal() { return this ? AsGlobal() : nullptr; }
			};

		}

		CURRENT_VERSION(IElement, 1);

	}

	using IElement = Version::Current::IElement;
}