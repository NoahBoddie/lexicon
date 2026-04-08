#include "Lexicon/Interfaces/IElementBase.h"

#include "Lexicon/Engine/Environment.h"

#include "Lexicon/Engine/Script.h"
#include "Lexicon/Engine/Project.h"
#include "Lexicon/Engine/Directory.h"
#include "Lexicon/Engine/Element.h"

#include "Lexicon/SignatureBase.h"
#include "Lexicon/Engine/Signature.h"
namespace LEX
{	
	IScript* IElementBase::GetScriptInfc()
	{
		return GetScriptImpl();
	}

	IProject* IElementBase::GetProjectInfc()
	{
		return GetProjectImpl();
	}

	IDirectory* IElementBase::GetParentInfc()
	{
		return GetParentImpl()->As<IDirectory>();
	}

	IDirectory* IElementBase::GetDirectoryInfc()
	{
		return GetDirectoryImpl()->As<IDirectory>();
	}

	IRepository* IElementBase::GetRepositoryInfc()
	{
		return GetRepositoryImpl()->As<IRepository>();
	}

	IEnvironment* IElementBase::GetEnvironmentInfc()
	{
		return GetEnvironmentImpl()->As<IEnvironment>();
	}

	IElement* IElementBase::GetElementFromPathInfc(std::string_view path, ElementType elem, SignatureBase* base)
	{
		
		if (base) {
			Signature sign{ *base };
			return GetElementFromPathImpl(path, elem, &sign)->As<IElement>();
		}
		else {
			return GetElementFromPathImpl(path, elem, nullptr)->As<IElement>();
		}
	}

	IScript* IElementBase::GetCommonsInfc()
	{
		return GetCommonsImpl();
	}

}