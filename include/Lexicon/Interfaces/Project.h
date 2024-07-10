#pragma once

#include "RGL/Impl/Record.h"
#include "Lexicon/Exception.h"//May move to src

#include "Lexicon/Component.h"
#include "Lexicon/Interfaces/Element.h"
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
		Script* GetCommons() override;

		void AddFormat(std::string_view name, std::string_view content, Script* source) override
		{
			Format format;
			format.formatName = name;
			format.formatContent = content;
			format.formatScript = source;

			formatList.push_back(format);

			report::debug("adding format {}", name);
		}

		void AddFormat(std::string& name, std::string& content, Script* source)
		{
			return AddFormat(std::string_view{ name }, std::string_view{ content }, source);
		}



		//Instead of doing this shit, just tack it onto an existing script's AST or something.
		//ScriptStack* stack = nullptr;

		//Project Environment. Keeps all scripts and is the scope for a given set of scripts. Interaction outside of is prohibited unless in shared project.

		CommonScript* _commons = nullptr;

		ProjectClient* _client = nullptr;

		std::string _name;

		std::vector<Script*> _scripts;
		std::vector<Format> formatList;


		
		//Linkage status, needs to link later depending on the performance of its commons

		//Also, a project COULD be responsible for making it's commons. I sorta like that idea personally.
		// But probably JUST it's commons.




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


		Project* GetProject() override
		{
			return this;
		}
		
		Element* GetParent() override
		{
			auto share = GetShared();
			//Should return null on shared projects.
			return share == this ? nullptr : share;
		}

		

		



		Environment* GetEnvironment() override
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

		

		Script* FindScript(std::string name);
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