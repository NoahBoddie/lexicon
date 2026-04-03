#include "Lexicon/Interfaces/IRepositoryBase.h"

#include "Lexicon/Engine/Script.h"
#include "Lexicon/Engine/Subdirectory.h"

namespace LEX
{
	IScript* IRepositoryBase::FindScriptInfc(const std::string_view& name)
	{
		return FindScriptImpl(name);
	}
	ISubdirectory* IRepositoryBase::FindSubdirectoryInfc(const std::string_view& name)
	{
		return FindSubdirectoryImpl(name);
	}

	IScript* IRepositoryBase::CreateScriptInfc(const std::string_view& name, std::span<std::string_view> options, const std::string_view& path, std::optional<std::string_view> content)
	{
		return CreateScriptImpl(name, options, path, content);
	}

	ISubdirectory* IRepositoryBase::CreateSubdirectoryInfc(const std::string_view& name, Script* sub_to, std::span<std::string_view> options, const std::string_view& path)
	{
		return CreateSubdirectoryImpl(name, sub_to->As<Script>(), options, path);
	}

}