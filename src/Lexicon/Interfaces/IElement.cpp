#include "Lexicon/Interfaces/IElement.h"

#include "Lexicon/Engine/Component.h"
#include "Lexicon/Engine/Element.h"
#include "Lexicon/Engine/Directory.h"
namespace LEX
{
	Script* IElement::GetScriptImpl()
	{
		return GetComponent()->As<Element>()->GetScript();
	}

	Project* IElement::GetProjectImpl()
	{
		return GetComponent()->As<Element>()->GetProject();
	}

	Directory* IElement::GetParentImpl()
	{
		return GetComponent()->As<Element>()->GetParent();
	}

	Environment* IElement::GetEnvironmentImpl()
	{
		return GetComponent()->As<Element>()->GetEnvironment();
	}

	Directory* IElement::GetDirectoryImpl()
	{
		return GetComponent()->As<Element>()->GetDirectory();
	}

	Repository* IElement::GetRepositoryImpl()
	{
		return GetAs<Element>()->GetRepository();
	}



	Script* IElement::GetCommonsImpl()
	{
		return GetComponent()->As<Element>()->GetCommons();
	}

	Element* IElement::GetElementFromPathImpl(std::string_view path, ElementType elem, OverloadArgument* sign)
	{
		return GetComponent()->As<Element>()->GetElementFromPath(path, elem, sign);
	}

}