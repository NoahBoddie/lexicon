#pragma once

#include "Interface.h"

#include "Lexicon/ElementType.h"

namespace LEX
{
	class Project;
	struct Element;
	struct Environment;
	
	struct IScript;
	struct IProject;
	struct IEnvironment;
	class Script;

	struct ITypePolicy;
	struct IFunction;
	struct IGlobal;
	
	//This should not be in interfaces, it's never used.

	struct IElement;

	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(IElement)
			{
				virtual std::string_view GetName() const = 0;
				virtual Record* GetSyntaxTree() = 0;
				
				virtual IScript* INT_NAME(GetScript)() = 0;
				virtual IProject* INT_NAME(GetProject)() = 0;
				virtual LEX::IElement* INT_NAME(GetParent)() = 0;
				virtual IEnvironment* INT_NAME(GetEnvironment)() = 0;
				
				virtual ITypePolicy* AsType() { return nullptr; }
				virtual IFunction* AsFunction() { return nullptr; }
				virtual IGlobal* AsGlobal() { return nullptr; }
				virtual LEX::IScript* AsScript() { return nullptr; }

				//This promote, and all promotes need to be internal only. There's no use to it outside
				// of this.
				virtual Element* Promote() = 0;
				virtual const Element* Promote() const = 0;

				//Will not need an interface name due to being able to be a covariant
				virtual LEX::IElement* GetElementFromPath(std::string_view path, ElementType elem) = 0;

			protected:
				//virtual void* demote() {};
				

			};

		}

		CURRENT_VERSION(IElement, 1);

	}

	struct IMPL_VERSION(IElement) 
	{

		Element* TryPromote() { return this ? Promote() : nullptr; }
		const Element* TryPromote() const { return this ? Promote() : nullptr; }


		ITypePolicy* ToType() { return this ? AsType() : nullptr; }
		IFunction* ToFunction() { return this ? AsFunction() : nullptr; }
		IGlobal* ToGlobal() { return this ? AsGlobal() : nullptr; }



		std::string GetFullName() //const
		{

			std::string result = std::string{ GetName() };

			IElement* element = INT_NAME(GetParent)();

			while (element)
			{
				result = std::format("{}::{}", element->GetName(), result);
				element = element->INT_NAME(GetParent)();
			}

			return result;
		}

	};
}