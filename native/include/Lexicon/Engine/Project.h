#pragma once

#include "RGL/Impl/Record.h"
#include "Lexicon/Exception.h"//May move to src

#include "Lexicon/Engine/Component.h"
#include "Lexicon/Engine/Repository.h"
#include "Lexicon/Interfaces/IProject.h"
namespace LEX
{
	class Script;

	struct ProjectClient;

	



	class Project : public Repository, public IProject
	{
	public:
		DECL_IMPL_FUNC_REPOSITORY;
		DEFINE_COMPONENT_OFFSET(ComponentType::Project)


		friend ProjectManager;




		//Instead of doing this shit, just tack it onto an existing script's AST or something.
		//ScriptStack* stack = nullptr;

		//Project Environment. Keeps all scripts and is the scope for a given set of scripts. Interaction outside of is prohibited unless in shared project.

		CommonScript* _commons = nullptr;

		ProjectClient* _client = nullptr;

		std::string _name;


		
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
		
		void AddScript(Script* script) override;
		//*/

		bool SendFormat(const std::string_view& format, const std::string_view& content, Script* source);

	private:
		bool AddFormatImpl(const std::string_view& name, const std::string_view& content, Script* source) override;

		Script* GetCommonsImpl() override;


		Project* GetProjectImpl() override
		{
			return this;
		}
		
		Directory* GetParentImpl() override;

		Script* GetScriptImpl() override
		{
			return nullptr;
		}

		const Component* GetComponent() const override final { return this; }

		Environment* GetEnvironmentImpl() override
		{
			//It has no environment.
			return nullptr;
		}


		void SetParent(Directory*) override
		{
			//nothing. maybe an error?
		}
	public:

		SyntaxRecord* GetSyntaxTree() override
		{
			//It has no syntax tree.
			return nullptr;
		}

		void SetSyntaxTree(SyntaxRecord& rec) final override {}

		Environment* FindEnvironment(SyntaxRecord& record, ITemplateInserter& inserter) override;


		Script* FindScriptImpl(const std::string_view& name) override;
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