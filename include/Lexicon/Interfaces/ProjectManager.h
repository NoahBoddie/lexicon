#pragma once

#include "Lexicon/Interfaces/IElement.h"

namespace LEX
{

	class Project;
	struct ITypePolicy;
	struct IFunction;
	struct IGlobal;
	class Script;
	struct ProjectClient;

	enum struct APIResult
	{
		Success,
		AbstractFailure, //Unaccounted for failure or non descript failure. Basically API kinda out of date.
		ElementExists,
		FileMissing,
		CreationFailed,
		Total, 


		Failure = 1,
	};


	//Move

		static std::vector<std::pair<std::string, std::string>> SearchFiles(std::string a_folder, std::string_view a_extension, std::set<std::string_view> ignore = {})
		{
			//Having no extension will mean I want to search for folders, which is how I'll say that

			//This may exclude commons to make that part easier.
			std::vector<std::pair<std::string, std::string>> result{};

			for (const auto iterator = std::filesystem::directory_iterator(a_folder); const auto & entry : iterator) {
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
				enum Enum
				{
					NoneElement,
					TypeElement,
					FuncElement,
					GlobElement,
				};


				virtual Project* GetProject(std::string_view name) = 0;
				virtual APIResult CreateScript(Project* project, std::string_view name, std::string_view path, Script** out = nullptr) = 0;
				virtual APIResult CreateProject(std::string_view name, ProjectClient* client, Project** out = nullptr, HMODULE source = GetCurrentModule()) = 0;
				

				virtual IElement* GetElementFromPath(std::string_view path, Enum elem) = 0;
						
			};
		}


		CURRENT_VERSION(ProjectManager, 1);
	}

	struct IMPL_SINGLETON(ProjectManager)
	{
	public://Interface functions

		Project* GetProject(std::string_view name) override;
			
		APIResult CreateScript(Project* project, std::string_view name, std::string_view path, Script** out = nullptr) override;
			
		APIResult CreateProject(std::string_view name, ProjectClient* client, Project** out = nullptr, HMODULE source = GetCurrentModule()) override;


		IElement* GetElementFromPath(std::string_view path, Enum elem) override;

	public://Ease of Use Functions


		APIResult CreateScript(Project* project, std::string_view name, std::string_view path, Script*& out)
		{
			return CreateScript(project, name, path, std::addressof(out));
		}

		APIResult CreateProject(std::string_view name, ProjectClient* client, Project*& out, HMODULE source = GetCurrentModule())
		{
			return CreateProject(name, client, std::addressof(out), source);
		}

		Project* GetShared()
		{
			static Project* shared = nullptr;

			if (!shared)
				shared = GetProject("shared");

			return shared;
		}

		IFunction* GetFunctionFromPath(std::string_view path)
		{
			if (auto elem = GetElementFromPath(path, FuncElement); elem)
				return elem->AsFunction();

			return nullptr;
		}

		ITypePolicy* GetTypeFromPath(std::string_view path)
		{
			if (auto elem = GetElementFromPath(path, TypeElement); elem)
				return elem->AsType();

			return nullptr;
		}

		IGlobal* GetGlobalFromPath(std::string_view path)
		{
			if (auto elem = GetElementFromPath(path, GlobElement); elem)
				return elem->AsGlobal();

			return nullptr;
		}

	INTERNAL://Hidden functions
			
		APIResult InitMain();
	};

}