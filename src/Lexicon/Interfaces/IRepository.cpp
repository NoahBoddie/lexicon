#include "Lexicon/Interfaces/IRepository.h"

#include "Lexicon/Engine/Component.h"
#include "Lexicon/Engine/Repository.h"

namespace LEX
{
	std::string_view IRepository::GetFilepath() const
	{
		return AsComponent()->As<Repository>()->GetFilepath();
	}

	Script* IRepository::FindScriptImpl(const std::string_view& name)
	{
		return AsComponent()->As<Repository>()->FindScript(name);
	}

	Subdirectory* IRepository::FindSubdirectoryImpl(const std::string_view& name)
	{
		return AsComponent()->As<Repository>()->FindSubdirectory(name);
	}


	Script* IRepository::CreateScriptImpl(const std::string_view& name, std::span<std::string_view> options, const std::string_view& path, std::optional<std::string_view> content)
	{
		return AsComponent()->As<Repository>()->CreateScript(name, options, path, content);
	}

	Subdirectory* IRepository::CreateSubdirectoryImpl(const std::string_view& name, Script* sub_to, std::span<std::string_view> options, const std::string_view& path)
	{
		return AsComponent()->As<Repository>()->CreateSubdirectory(name, sub_to, options, path);
	}
}