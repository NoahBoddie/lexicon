#include "Lexicon/Engine/IElementImpl.h"

#include "Lexicon/Engine/Script.h"
#include "Lexicon/Engine/Project.h"

#include "Lexicon/Engine/Signature.h"
namespace LEX
{

	
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