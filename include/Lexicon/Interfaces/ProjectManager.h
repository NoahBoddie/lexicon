#pragma once

#include "Lexicon/Interfaces/IProject.h"
#include "Lexicon/Interfaces/IElement.h"

namespace LEX
{

	struct IScript;
	struct IProject;
	
	class Project;
	struct IFunction;
	struct IGlobal;
	class Script;
	struct GlobalBase;
	struct ProjectClient;
	struct ISignature;


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

	inline bool ends_with(std::string_view value, std::string_view ending)
	{
		if (ending.size() > value.size()) return false;
		return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
	}

	static std::vector<std::pair<std::string, std::string>> SearchFiles(std::string_view a_folder, std::string_view a_extension, std::set<std::string_view> ignore = {})
	{
		//Having no extension will mean I want to search for folders, which is how I'll say that

		//This may exclude commons to make that part easier.
		std::vector<std::pair<std::string, std::string>> result{};

		for (const auto iterator = std::filesystem::directory_iterator(a_folder); const auto& entry : iterator) {
			if (entry.exists() == false)
				continue;

			bool expects_directory = a_extension == "";

			bool contains = false;

			if (a_extension.front() == '*') {
				contains = true;
				a_extension = { a_extension.begin() + 1, a_extension.end() };
			}

			if (entry.is_directory() != expects_directory)
				continue;

			const auto& path = entry.path();
			//path.extension() !=
			if (path.empty())
				continue;

			if (contains && a_extension.contains(path.extension().string()) == false)
				continue;
			else if (!contains && ends_with(path.filename().string(), a_extension) == false)
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
	static std::vector<std::pair<std::string, std::string>> SearchFiles(std::string_view a_folder, std::string_view a_extension, std::string_view&& ignore)
	{
		return SearchFiles(a_folder, a_extension, { ignore });
	}

	static std::vector<std::pair<std::string, std::string>> SearchFiles(std::string_view a_folder, std::string_view a_extension, std::set<std::string>& ignore)
	{
		return SearchFiles(a_folder, a_extension, std::set<std::string_view>{ ignore.begin(), ignore.end() });
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
				virtual APIResult CreateScript(LEX::IProject* project, std::string_view name, std::string_view path, 
					std::optional<std::string_view> content = std::nullopt, LEX::IScript** out = nullptr, api::vector<std::string_view> options = {}) = 0;
				
				virtual APIResult CreateProject(std::string_view name, LEX::ProjectClient* client, LEX::IProject** out = nullptr, HMODULE source = GetCurrentModule()) = 0;
				

				virtual IElement* GetElementFromPath(std::string_view path, ElementType elem, const LEX::ISignature* sign = nullptr) = 0;
						
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

		LEX::IScript* GetScriptFromPath(std::string_view path, EXTERN_NAME)
		{
			if (auto elem = GetElementFromPath(path, kScrpElement); elem)
				return elem->As<IScript>();

			return nullptr;
		}


		IGlobal* GetGlobalFromPath(std::string_view path, EXTERN_NAME)
		{
			if (auto elem = GetElementFromPath(path, kGlobElement); elem)
				return elem->As<IGlobal>();

			return nullptr;
		}



	public:
		APIResult CreateScript(IProject* project, std::string_view name, std::string_view path, 
			std::optional<std::string_view> content = std::nullopt, IScript** out = nullptr, api::vector<std::string_view> options = {}) override;
			
		APIResult CreateProject(std::string_view name, LEX::ProjectClient* client, IProject** out = nullptr, HMODULE source = GetCurrentModule()) override;


		IElement* GetElementFromPath(std::string_view path, ElementType elem, const ISignature* sign = nullptr) override;

	public://Ease of Use Functions

		//Make a version without project, something like defaults to a certain project.
		APIResult CreateScript(IProject* project, std::string_view name, std::string_view path, std::string_view content, IScript*& out, api::vector<std::string_view> options = {})
		{
			return CreateScript(project, name, path, content, std::addressof(out), options);
		}

		APIResult CreateProject(std::string_view name, ProjectClient* client, IProject*& out, HMODULE source = GetCurrentModule())
		{
			return CreateProject(name, client, std::addressof(out), source);
		}

		IFunction* GetFunctionFromPath(std::string_view path, const ISignature& sign)
		{
			if (auto elem = GetElementFromPath(path, kFuncElement, &sign); elem)
				return elem->As<IFunction>();

			return nullptr;
		}

		ITypeInfo* GetTypeFromPath(std::string_view path)
		{
			if (auto elem = GetElementFromPath(path, kTypeElement); elem)
				return elem->As<ITypeInfo>();

			return nullptr;
		}

	INTERNAL:

		GlobalBase* GetGlobalFromPath(std::string_view path, INTERN_NAME)
		{
			if (auto elem = GetElementFromPath(path, kGlobElement); elem)
				return elem->As<GlobalBase>();

			return nullptr;
		}



		Script* GetScriptFromPath(std::string_view path, INTERN_NAME)
		{
			if (auto elem = GetElementFromPath(path, kScrpElement); elem)
				return elem->As<Script>();

			return nullptr;
		}



	INTERNAL://Hidden functions
		
		Script* GetCore();

		Project* GetShared(INTERN_NAME);

		Project* GetProject(std::string_view name, INTERN_NAME);

		APIResult InitMain();

		APIResult CreateScript(Project* project, std::string_view name, std::string_view path, std::string_view content, Script*& out);

		APIResult CreateProject(std::string_view name, ProjectClient* client, Project*& out, HMODULE source = GetCurrentModule());

	};

}