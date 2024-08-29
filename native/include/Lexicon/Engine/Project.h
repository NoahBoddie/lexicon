#pragma once

#include "RGL/Impl/Record.h"
#include "Lexicon/Exception.h"//May move to src

#include "Lexicon/Engine/Component.h"
#include "Lexicon/Engine/Element.h"
#include "Lexicon/Interfaces/IProject.h"

namespace LEX
{
	class Script;

	struct ProjectClient;

	struct Format
	{
		std::string formatName;
		std::string formatContent;
		LEX::Script* formatScript = nullptr;
	};



	class Project : public Element, public IProject
	{
	public:
		IScript* GetCommonsI() override;

		//TODO: Why is this virtual.
		Script* GetCommons();

		void AddFormat(std::string_view name, std::string_view content, IScript* source) override
		{
			Format format;
			format.formatName = name;
			format.formatContent = content;
			format.formatScript = source->TryPromote();

			formatList.push_back(format);

			report::debug("adding format {}", name);
		}

		void AddFormat(std::string& name, std::string& content, Script* source);


		Project* Promote() override { return this; }

		const Project* Promote() const override { return this; }

		//////////////////



		//Instead of doing this shit, just tack it onto an existing script's AST or something.
		//ScriptStack* stack = nullptr;

		//Project Environment. Keeps all scripts and is the scope for a given set of scripts. Interaction outside of is prohibited unless in shared project.

		CommonScript* _commons = nullptr;

		ProjectClient* _client = nullptr;

		std::filesystem::path _filePath;

		std::string _name;

		std::vector<Script*> _scripts;
		std::vector<Format> formatList;


		
		//Linkage status, needs to link later depending on the performance of its commons

		//Also, a project COULD be responsible for making it's commons. I sorta like that idea personally.
		// But probably JUST it's commons.


		ProjectClient* client()
		{
			return _client;
		}

		std::string_view GetName() const override
		{
			return _name;
		}

		void SetName(std::string_view name)
		{
			_name = name;
		}
		
		void AddScript(Script* script);
		//*/


		IProject* GetProjectI() override
		{
			return this;
		}
		
		IElement* GetParentI() override
		{
			auto share = GetShared();
			//Should return null on shared projects.
			return share == this ? nullptr : share;
		}

		

		



		IEnvironment* GetEnvironmentI() override
		{
			//It has no environment.
			return nullptr;
		}


		void SetParent(Element*) override
		{
			//nothing. maybe an error?
		}


		Record* GetSyntaxTree() override
		{
			//It has no syntax tree.
			return nullptr;
		}

		void SetSyntaxTree(Record& rec) final override {}

		ComponentType GetComponentType() override
		{
			return typeid(Project);
		}

		

		Script* FindScript(std::string_view name);
	};


	namespace ExperimentZone
	{

		struct SharedProject : public Project
		{


			//void AddFormat(std::string& name, std::string& content, Script* source) override
			//{
			//	//report::error("Shared project cannot use any format");
			//}

			//Projects might be handled else where actually
			
		};
	}

}