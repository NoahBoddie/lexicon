#include "Lexicon/Engine/Project.h"
#include "Lexicon/Engine/Script.h"

namespace LEX
{

	void  Project::AddFormat(std::string& name, std::string& content, Script* source)
	{
		AddFormat(std::string_view{ name }, std::string_view{ content }, source);
	}


	Script* Project::FindScript(std::string_view name)
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

	IScript* Project::GetCommonsI()
	{
		return _commons;
	}

	Script* Project::GetCommons()
	{
		return GetCommonsI()->TryPromote();
	}
}