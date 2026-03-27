#include "Lexicon/Interfaces/IProject.h"

#include "Lexicon/Engine/Script.h"

namespace LEX
{
	void IProject::AddFormatInfc(const std::string_view& name, const std::string_view& content, IScript* source)
	{
		return AddFormatImpl(name, content, source->As<Script>());
	}

	IScript* IProject::FindScriptInfc(const std::string_view& name)
	{
		return FindScriptImpl(name);
	}
}