#include "Lexicon/Engine/Project.h"
#include "Lexicon/Engine/Script.h"
#include "Lexicon/Engine/ProjectDirectory.h"
namespace LEX
{

	void Project::AddFormatImpl(const std::string_view& name, const std::string_view& content, Script* source)
	{
		Format format;
		format.formatName = name;
		format.formatContent = content;
		format.formatScript = source;

		formatList.push_back(format);

		report::debug("adding format {}", name);
	}


	Script* Project::FindScriptImpl(const std::string_view& name)
	{
		if (name == "Commons") {
			return _commons;
		}

		return Repository::FindScriptImpl(name);
	}

	Directory* Project::GetParentImpl()
	{
		return IsShared() ? nullptr : ProjectDirectory::GetSingleton();
	}

	void Project::AddScript(Script* script)
	{
		//Should search for script, throwing if within, then call SetParent on the script.

		if (script->IsCommons() == true) 
		{
			if (_commons) {
				report::compile::error("CommonScript for project {} already exists", GetName());
			}

			_commons = static_cast<CommonScript*>(script);
			DeclareParentTo(script);
		}
		else
		{
			return Repository::AddScript(script);
		}

	}


	Script* Project::GetCommonsImpl()
	{
		return _commons;
	}

	Environment* Project::FindEnvironment(SyntaxRecord& record, ITemplateInserter& inserter)
	{
		return FindScript(record.GetView());
	}


	Directory* Project::FindDirectory(SyntaxRecord& record, ITemplateInserter* inserter)
	{
		return FindScript(record.GetView());
	}


}