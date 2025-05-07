#include "Lexicon/Engine/Project.h"
#include "Lexicon/Engine/Script.h"

namespace LEX
{

	void Project::AddFormat(std::string_view name, std::string_view content, IScript* source) 
	{
		AddFormat(name, content, static_cast<Script*>(source));
	}


	void  Project::AddFormat(std::string_view name, std::string_view content, Script* source)
	{
		Format format;
		format.formatName = name;
		format.formatContent = content;
		format.formatScript = source;

		formatList.push_back(format);

		report::debug("adding format {}", name);
	}


	Script* Project::FindScript(const std::string_view& name)
	{
		if (name == "Commons") {
			return _commons;
		}

		auto end = _scripts.end();


		//Proper version of Script not implement
		auto it = std::find_if(_scripts.begin(), end, [&](Script* search) { return search->GetName() == name; });

		if (it != end) {
			return *it;
		}

		return nullptr;
	}

	void Project::AddScript(Script* script)
	{
		//Should search for script, throwing if within, then call SetParent on the script.

		if (script->IsCommons() == true) {

			_commons = static_cast<CommonScript*>(script);
		}
		else
		{
			auto end = _scripts.end();

			//TODO: Report name when Project::AddScript is used, seems pretty important.
			if (auto it = std::find(_scripts.begin(), end, script); it != end)
				throw EnvironmentError("Script already added.");

			_scripts.push_back(script);
		}

		//Call add parent function
		DeclareParentTo(script);

	}


	Script* Project::GetCommons()
	{
		return _commons;
	}

	Environment* Project::FindEnvironment(SyntaxRecord& record, ITemplateInserter& inserter)
	{
		return FindScript(record.GetView());
	}

}