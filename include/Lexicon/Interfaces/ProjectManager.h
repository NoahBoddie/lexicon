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
				virtual IProject* INT_NAME(GetProject)(std::string_view name) = 0;
			public:
				virtual APIResult CreateScript(Project* project, std::string_view name, std::string_view path, Script** out = nullptr, const std::vector<std::string_view>*options = nullptr) = 0;
				virtual APIResult CreateProject(std::string_view name, ProjectClient* client, Project** out = nullptr, HMODULE source = GetCurrentModule()) = 0;
				

				virtual IElement* GetElementFromPath(std::string_view path, ElementType elem) = 0;
						
			};
		}


		CURRENT_VERSION(ProjectManager, 1);
	}

	struct IMPL_SINGLETON(ProjectManager)
	{
	EXTERNAL://Interface functions
	
		IProject* INT_NAME(GetProject)(std::string_view name) override;

		IProject* INT_NAME(GetShared)()
		{
			static IProject* shared = nullptr;

			if (!shared)
				shared = INT_NAME(GetProject)("shared");

			return shared;
		}

	public:
		APIResult CreateScript(Project* project, std::string_view name, std::string_view path, Script** out = nullptr, const std::vector<std::string_view>* options = nullptr) override;
			
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
				return elem->AsFunction();

			return nullptr;
		}

		ITypePolicy* GetTypeFromPath(std::string_view path)
		{
			if (auto elem = GetElementFromPath(path, kTypeElement); elem)
				return elem->AsType();

			return nullptr;
		}

		IGlobal* GetGlobalFromPath(std::string_view path)
		{
			if (auto elem = GetElementFromPath(path, kGlobElement); elem)
				return elem->AsGlobal();

			return nullptr;
		}


		LEX::IScript* GetScriptFromPath(std::string_view path)
		{
			if (auto elem = GetElementFromPath(path, kScrpElement); elem)
				return elem->AsScript();

			return nullptr;
		}


	INTERNAL://Hidden functions
		
		Project* IMP_NAME(GetShared)()
		{
			return INT_NAME(GetShared)()->TryPromote();
		}

		Project* IMP_NAME(GetProject)(std::string_view name)
		{
			return INT_NAME(GetProject)(name)->TryPromote();
		}

		APIResult InitMain();
	};

}