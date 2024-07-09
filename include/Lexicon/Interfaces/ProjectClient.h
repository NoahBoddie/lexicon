#pragma once

namespace LEX
{
	class Project;
	class Script;


	namespace Version
	{
		namespace Current { struct ProjectClient; }

		namespace _1
		{
			struct INTERFACE_VERSION(ProjectClient)
			{
				//I may actually make these optional virtuals.
				virtual void RecieveMessage(uint64_t severity, std::string_view message, Current::ProjectClient* sender) = 0;
				virtual bool HandleFormat(Project* project, Script* script, std::string_view format, std::string_view name, std::string_view content) = 0;

				virtual HMODULE GetParentModule() const = 0;
			};
		}


		//ProjectClient is a class that's used to communicate with the owner of a project.
		CURRENT_VERSION(ProjectClient, 1);
		
		namespace Current
		{
			using ProjectClientSingleton = ProjectClient;
		}
	}

	using IProjectClient = Version::Current::ProjectClient;

	//This will allow for forward declaration where type alias will not.
	struct ProjectClient : public Version::Current::ProjectClient 
	{
		HMODULE GetParentModule() const override final { return GetCurrentModule(); }
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