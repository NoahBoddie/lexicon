#pragma once

#include "Lexicon/Engine/Directory.h"
#include "Lexicon/Interfaces/IDirectory.h"


//*src
#include "Lexicon/Engine/Project.h"

namespace LEX
{
	

	//Stores projects as a directory
	struct ProjectDirectory : public Directory, public IDirectory
	{
		friend ProjectManager;

		DECL_IMPL_FUNC_DIRECTORY;

		static ProjectDirectory* GetSingleton()
		{
			ProjectDirectory& singleton = make_singleton<ProjectDirectory>();

			return &singleton;
		}

		


		std::string_view GetName() const override
		{
			return "<ProjectDirectory>";
		}

		SyntaxRecord* GetSyntaxTree() override
		{
			return nullptr;
		}

		void SetSyntaxTree(SyntaxRecord&) override
		{
			//Cannot set this
		}
		void SetParent(Directory*) override
		{
			//cannot set parent here either
		}
		Directory* GetParentImpl() override
		{
			return Project::GetShared();
		}

		const Component* GetComponent() const override
		{
			return this;
		}

		Environment* GetEnvironmentImpl() override
		{
			return nullptr;
		}

		const std::vector<Project*>& GetProjects() const
		{
			return _projects;
		}

		Project* FindProject(const std::string_view& name);

		Directory* FindDirectory(SyntaxRecord& record, ITemplateInserter*) override
		{
			return FindProject(record.GetView());
		}


	private:
		std::vector<Project*> _projects;
	};
}