#include "Lexicon/Interfaces/IElement.h"

#include "Lexicon/Engine/Component.h"
#include "Lexicon/Engine/Element.h"

namespace LEX
{
	Script* IElement::GetScriptImpl()
	{
		return AsComponent()->As<Element>()->GetScript();
	}

	Project* IElement::GetProjectImpl()
	{
		return AsComponent()->As<Element>()->GetProject();
	}

	Directory* IElement::GetParentImpl()
	{
		return AsComponent()->As<Element>()->GetParent();
	}

	Environment* IElement::GetEnvironmentImpl()
	{
		return AsComponent()->As<Element>()->GetEnvironment();
	}
	Script* IElement::GetCommonsImpl()
	{
		return AsComponent()->As<Element>()->GetCommons();
	}

	Element* IElement::GetElementFromPathImpl(std::string_view path, ElementType elem, OverloadArgument* sign)
	{
		return AsComponent()->As<Element>()->GetElementFromPath(path, elem, sign);
	}

}