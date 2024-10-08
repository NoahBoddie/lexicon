#pragma once

#include "RGL/Impl/Record.h"
#include "Lexicon/Exception.h"//May move to src

#include "Lexicon/Engine/Component.h"
#include "Lexicon/Engine/Element.h"
#include "Lexicon/Engine/IProjectImpl.h"

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



	class Project : public Element, public IProjectImpl
	{
	public:
		Script* GetCommons(bool = {}) override;


		//Needs sourcing
		void AddFormat(std::string_view name, std::string_view content, IScript* source) override;

		void AddFormat(std::string_view name, std::string_view content, Script* source);

		IElement* base() override { return this; }
		const IElement* base() const override { return this; }

		void* Cast(std::string_view name) override
		{
			switch (Hash(name))
			{
			case Hash(TypeName<IProject>::value):
				return (IProject*)this;
			case Hash(TypeName<Project>::value):
				return this;
			}
			return nullptr;
		}

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


		Project* GetProject(bool = {}) override
		{
			return this;
		}
		
		Element* GetParent(bool = {}) override
		{
			auto share = GetShared();
			//Should return null on shared projects.
			return share == this ? nullptr : share;
		}

		

		



		Environment* GetEnvironment(bool = {}) override
		{
			//It has no environment.
			return nullptr;
		}


		void SetParent(Element*) override
		{
			//nothing. maybe an error?
		}


		SyntaxRecord* GetSyntaxTree() override
		{
			//It has no syntax tree.
			return nullptr;
		}

		void SetSyntaxTree(SyntaxRecord& rec) final override {}

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