#include "Lexicon/Engine/IElementImpl.h"

#include "Lexicon/Engine/Script.h"
#include "Lexicon/Engine/Project.h"

#include "Lexicon/Engine/Signature.h"
namespace LEX
{
	template <std::derived_from<IElement>  Base>
	IScript* IElementBase<Base>::GetScript(bool)
	{
		return GetScript();
	}

	template <std::derived_from<IElement>  Base>
	IProject* IElementBase<Base>::GetProject(bool)
	{
		return GetProject();
	}

	template <std::derived_from<IElement>  Base>
	IElement* IElementBase<Base>::GetParent(bool)
	{
		return GetParent();
	}

	template <std::derived_from<IElement>  Base>
	IElement* IElementBase<Base>::GetEnvironment(bool)
	{
		return GetEnvironment();
	}

	template <std::derived_from<IElement>  Base>
	IElement* IElementBase<Base>::GetElementFromPath(std::string_view path, ElementType elem, SignatureBase* base, bool)
	{
		if (base) {
			Signature sign{ *base };
			return GetElementFromPath(path, elem, &sign);
		}
		else {
			return GetElementFromPath(path, elem, nullptr);
		}
	}
	template <std::derived_from<IElement>  Base>
	IScript* IElementBase<Base>::GetCommons(bool)
	{
		return GetCommons();
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