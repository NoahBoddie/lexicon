#include "Lexicon/Engine/IElementImpl.h"

#include "Lexicon/Engine/Script.h"
#include "Lexicon/Engine/Project.h"

namespace LEX
{
	template <std::derived_from<IElement>  Base>
	IScript* IElementBase<Base>::GetScript()
	{
		return GetScript({});
	}

	template <std::derived_from<IElement>  Base>
	IProject* IElementBase<Base>::GetProject()
	{
		return GetProject({});
	}

	template <std::derived_from<IElement>  Base>
	IElement* IElementBase<Base>::GetParent()
	{
		return GetParent({});
	}

	template <std::derived_from<IElement>  Base>
	IEnvironment* IElementBase<Base>::GetEnvironment()
	{
		return GetEnvironment({});
	}

	template <std::derived_from<IElement>  Base>
	IElement* IElementBase<Base>::GetElementFromPath(std::string_view path, ElementType elem)
	{
		return GetElementFromPath(path, elem, {});
	}
	template <std::derived_from<IElement>  Base>
	IScript* IElementBase<Base>::GetCommons()
	{
		return GetCommons({});
	}
	
	void linkworkaround()
	{
		Script _2{};
		Project _1{};
	}



	/*
	IScript* IElementImpl::GetScript()
	{
		return GetScript({});
	}


	IProject* IElementImpl::GetProject()
	{
		return GetProject({});
	}


	IElement* IElementImpl::GetParent()
	{
		return GetParent({});
	}


	IEnvironment* IElementImpl::GetEnvironment()
	{
		return GetEnvironment({});
	}


	IElement* IElementImpl::GetElementFromPath(std::string_view path, ElementType elem)
	{
		return GetElementFromPath(path, elem, {});
	}

	IScript* IElementImpl::GetCommons()
	{
		return GetCommons({});
	}
	//*/
}