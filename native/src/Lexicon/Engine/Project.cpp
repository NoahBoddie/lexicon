#include "Lexicon/Engine/Project.h"
#include "Lexicon/Engine/Script.h"
#include "Lexicon/Engine/ProjectDirectory.h"
#include "Lexicon/Interfaces/ProjectClient.h"
#include "Lexicon/Interfaces/SharedClient.h"
namespace LEX
{

	bool Project::AddFormatImpl(const std::string_view& name, const std::string_view& content, Script* source)
	{
		return false;

		Format format;
		format.formatName = name;
		format.formatContent = content;
		format.formatScript = source;

		report::debug("adding format {}", name);
	}



	bool Project::SendFormat(const std::string_view& format, const std::string_view& content, Script* source)
	{
		bool sent = false;
		if (_client) {
			sent = _client->HandleFormat(source, format, content);
		}
		if (!sent) {
			if (auto shared = GetShared(); shared && this != shared) {
				sent = SharedClient::instance->HandleFormat(source, format, content);
				
				//sent = shared->SendFormat(format, content, source);
			}
		}

		return sent;
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




}