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
				//virtual Record* GetSyntaxTree() = 0;//Do not use this, it won't be able to handle the request.
				
				virtual IScript* GetScript() = 0;
				virtual IProject* GetProject() = 0;
				virtual LEX::IElement* GetParent() = 0;
				virtual IEnvironment* GetEnvironment() = 0;
				virtual LEX::IScript* GetCommons() = 0;
				

				//Will not need an interface name due to being able to be a covariant
				virtual LEX::IElement* GetElementFromPath(std::string_view path, ElementType elem) = 0;
			protected:
				virtual void* Cast(std::string_view name) = 0;


			};

		}

		CURRENT_VERSION(IElement, 1);

	}

	struct IMPL_VERSION(IElement) 
	{

		std::string GetFullName() //const
		{

			std::string result = std::string{ GetName() };

			IElement* element = GetParent();

			while (element)
			{
				result = std::format("{}::{}", element->GetName(), result);
				element = element->GetParent();
			}

			return result;
		}



		//Each of these muse derive from the same type that IElement does.
		template<typename T>
		T* As()
		{
			if (!this)
				return nullptr;

			void* result = Cast(TypeName<T>::value);

			return reinterpret_cast<T*>(result);
		}

		template <typename T>
		operator T& ()
		{
			return *As<T>();
		}
	};
}