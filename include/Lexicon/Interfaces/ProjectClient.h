#pragma once

#include "Lexicon/TypeID.h"

namespace LEX
{
	class Project;
	class Script;

	struct ProjectClient;

	namespace Version
	{
		namespace Current { struct ProjectClient; }

		namespace _1
		{
			struct INTERFACE_VERSION(ProjectClient)
			{
			

				virtual HMODULE GetParentModule() = 0;


				//I may actually make these optional virtuals.
				virtual void RecieveMessage(uint64_t severity, std::string_view message, LEX::ProjectClient* sender) = 0;
				
				virtual bool HandleFormat(Script* script, std::string_view format, std::string_view name, std::string_view content) = 0;
				
				//returns an empty string view when it reaches the end.
				virtual std::string_view GetCompileOptions(size_t index) = 0;

			};
		}


		//ProjectClient is a class that's used to communicate with the owner of a project.
		CURRENT_VERSION(ProjectClient, 1);
		
	}

	
	struct IMPL_VERSION(ProjectClient)
	{
		HMODULE GetParentModule() override final { return GetCurrentModule(); }

		std::string_view GetCompileOptions(size_t index) override { return {}; }



		IProject* GetProject() { return _project; }


	INTERNAL:
		IProject* _project = nullptr;

	};

	/*
	struct IMPL_SINGLETON(ProjectClientSingleton)
	{
		//Define these in your own project if you want to use the singleton.
		void RecieveMessage(uint64_t severity, std::string_view message) override;
		bool HandleFormat(Project * project, Script * script, std::string_view format, std::string_view name, std::string_view content) override;
	};
	//*/
}