#pragma once

#include "Lexicon/Interfaces/IProject.h"
#include "Lexicon/Interfaces/IElement.h"

namespace LEX
{

	struct IScript;
	struct IProject;
	
	class Project;
	struct ITypePolicy;
	struct IFunction;
	struct IGlobal;
	class Script;
	struct GlobalBase;
	struct ProjectClient;

	enum ElementType : uint8_t;


	enum struct APIResult
	{
		Success,
		AbstractFailure, //Unaccounted for failure or non descript failure. Basically API kinda out of date.
		ElementExists,
		FileMissing,
		CreationFailed,
		MissingRequirement,
		Total, 


		Failure = 1,
	};


	//Move

		static std::vector<std::pair<std::string, std::string>> SearchFiles(std::string a_folder, std::string_view a_extension, std::set<std::string_view> ignore = {})
		{
			//Having no extension will mean I want to search for folders, which is how I'll say that

			//This may exclude commons to make that part easier.
			std::vector<std::pair<std::string, std::string>> result{};

			for (const auto iterator = std::filesystem::directory_iterator(a_folder); const auto& entry : iterator) {
				if (entry.exists() == false)
					continue;

				bool expects_directory = a_extension == "";

				if (entry.is_directory() != expects_directory)
					continue;

				const auto& path = entry.path();
				//path.extension() !=
				if (path.empty() || a_extension.contains(path.extension().string()) == false)
					continue;

				auto filename = path.filename().string();

				if (ignore.contains(filename) == true)
					continue;

				result.push_back({ path.parent_path().string(), filename });


				//if (entry.exists()) {
				//	if (const auto& path = entry.path(); !path.empty() && path.extension() == a_extension) {
				//		std::pair<std::string, std::string> script = std::make_pair(path.parent_path().string(), path.filename().string());
				//		result.push_back(script);
				//	}
				//}
			}

			std::ranges::sort(result);

			return result;
		}

		//Use as a part of the finish to report project folders that have nothing using them.
		static std::vector<std::pair<std::string, std::string>> SearchFiles(std::string a_folder, std::string_view a_extension, std::string_view&& ignore)
		{
			return SearchFiles(a_folder, a_extension, { ignore });
		}

	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(ProjectManager)
			{
			EXTERNAL:
				virtual LEX::IProject* GetProject(std::string_view name, EXTERN_NAME) = 0;
			public:
				virtual APIResult CreateScript(Project* project, std::string_view name, std::string_view path, Script** out = nullptr, api::container<std::vector<std::string_view>> options = {}) = 0;
				virtual APIResult CreateProject(std::string_view name, ProjectClient* client, Project** out = nullptr, HMODULE source = GetCurrentModule()) = 0;
				

				virtual IElement* GetElementFromPath(std::string_view path, ElementType elem) = 0;
						
			};
		}


		CURRENT_VERSION(ProjectManager, 1);
	}

	struct IMPL_SINGLETON(ProjectManager)
	{
	EXTERNAL://Interface functions
	
		IProject* GetProject(std::string_view name, EXTERN_NAME) override;

		IProject* GetShared(EXTERN_NAME)
		{
			static IProject* shared = nullptr;
			
			if (!shared)
				//Either way, this will be accessing something that can handle it
				shared = GetProject("shared", EXTERN_TYPE{});

			return shared;
		}

	

	public:
		APIResult CreateScript(Project* project, std::string_view name, std::string_view path, Script** out = nullptr, api::container<std::vector<std::string_view>> options = {}) override;
			
		APIResult CreateProject(std::string_view name, ProjectClient* client, Project** out = nullptr, HMODULE source = GetCurrentModule()) override;


		IElement* GetElementFromPath(std::string_view path, ElementType elem) override;

	public://Ease of Use Functions


		APIResult CreateScript(Project* project, std::string_view name, std::string_view path, Script*& out)
		{
			return CreateScript(project, name, path, std::addressof(out));
		}

		APIResult CreateProject(std::string_view name, ProjectClient* client, Project*& out, HMODULE source = GetCurrentModule())
		{
			return CreateProject(name, client, std::addressof(out), source);
		}

		IFunction* GetFunctionFromPath(std::string_view path)
		{
			if (auto elem = GetElementFromPath(path, kFuncElement); elem)
				return elem->As<IFunction>();

			return nullptr;
		}

		ITypePolicy* GetTypeFromPath(std::string_view path)
		{
			if (auto elem = GetElementFromPath(path, kTypeElement); elem)
				return elem->As<ITypePolicy>();

			return nullptr;
		}


		GlobalBase* GetGlobalFromPath(std::string_view path, INTERN_NAME)
		{
			if (auto elem = GetElementFromPath(path, kGlobElement); elem)
				return elem->As<GlobalBase>();

			return nullptr;
		}


		IGlobal* GetGlobalFromPath(std::string_view path, EXTERN_NAME)
		{
			if (auto elem = GetElementFromPath(path, kGlobElement); elem)
				return elem->As<IGlobal>();

			return nullptr;
		}


		LEX::IScript* GetScriptFromPath(std::string_view path, INTERN_NAME)
		{
			if (auto elem = GetElementFromPath(path, kScrpElement); elem)
				return elem->As<IScript>();

			return nullptr;
		}

		Script* GetScriptFromPath(std::string_view path, EXTERN_NAME)
		{
			if (auto elem = GetElementFromPath(path, kScrpElement); elem)
				return elem->As<Script>();

			return nullptr;
		}



	INTERNAL://Hidden functions
		
		Project* GetShared(INTERN_NAME);

		Project* GetProject(std::string_view name, INTERN_NAME);

		APIResult InitMain();
	};

}