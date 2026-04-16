#include "Lexicon/Interfaces/IProject.h"

#include "Lexicon/Engine/Script.h"

namespace LEX
{
	bool IProject::AddFormatInfc(const std::string_view& name, const std::string_view& content, IScript* source)
	{
		return AddFormatImpl(name, content, source->As<Script>());
	}
}