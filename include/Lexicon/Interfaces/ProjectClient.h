#pragma once

#include "Lexicon/TypeID.h"

namespace LEX
{
	struct IProject;
	class Project;
	class Script;
	struct IScript;
	struct IRepository;
	struct ProjectClient;
	class RuntimeVariable;

	using ScriptFormatFn = bool(*)(
		IScript* script,
		const std::string_view& format,
		const std::string_view& content);



	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(ProjectClient)
			{
				virtual HMODULE GetParentModule() = 0;

				//I may actually make these optional virtuals.
				virtual bool RecieveMessage(const std::string_view& msg, std::span<RuntimeVariable> args, ProjectClient* sender) = 0;
				
				virtual bool HandleFormat(IScript* script, const std::string_view& format, const std::string_view& content) = 0;
				
				//returns an empty string view when it reaches the end.
				virtual std::string_view GetCompileOptions(size_t index) = 0;
				
				virtual bool AddFormatter(const std::string_view& format, const std::string_view& owner_name, ScriptFormatFn func) = 0;
			};
		}


		//ProjectClient is a class that's used to communicate with the owner of a project.
		CURRENT_VERSION(ProjectClient, 1);
		
	}

	
	struct IMPL_VERSION(ProjectClient)
	{
		HMODULE GetParentModule() override { return GetCurrentModule(); }

		std::string_view GetCompileOptions(size_t index) override { return {}; }
		
		bool AddFormatter(const std::string_view& format, const std::string_view& owner_name, ScriptFormatFn func) override
		{
			return false;
		}



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