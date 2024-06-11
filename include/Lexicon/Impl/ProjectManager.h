#pragma once

#include "RGL/Impl/Record.h"
#include "Exception.h"//May move to src

#include "Expression.h"

#include "Lexicon/Component.h"
#include "Element.h"

//temp
#include "Project.h"
#include "Script.h"
#include "Parser.h"
namespace LEX
{



	//*
	//Revise this fucking interface
	//This line of idea should probably be something like ProjectClient or something like that.
	enum struct ProjectEvent
	{
		Version1,
	};
	struct IProjectEventBase
	{
		virtual ProjectEvent GetVersion() = 0;
		virtual ~IProjectEventBase() = default;
	};

	namespace Version1
	{
		//Whatever is being used should at least be using this.
		struct ProjectEventHandler : public IProjectEventBase
		{

			ProjectEvent GetVersion() override
			{
				return ProjectEvent::Version1;
			}
			virtual void RecieveMessage(int severity, const char* message) = 0;

			//The idea of the commons hash is enabling one the option of confirming that the version of commons is in line with the version uploaded. with the project.
			//The idea of this scares me however. What happens when someone makes minor superficial changes and it prevents the entire project from running?
			// TO do this accurately, it would need to validate the actual contents and I wan't want to do that much.
			//Actually, here's what I can do.
			// if it can allow me, I'll parse everything that isn't whitespace and hash that. Finding a constexpr way to load a file however would be tough.
			// I'll have to find some other form of validation I think
			virtual size_t GetCommonsHashs() = 0;
			virtual void HandleFormat(std::string_view project_name, std::string_view script_name, std::string_view format, std::string_view content) = 0;

		};
	}

	//Could use a macro for this
	using ProjectClient = Version1::ProjectEventHandler;
	//*/

	enum struct APIResult
	{
		Success = -1,
		Failure
	};



	class ProjectManager
	{
		//This is temporarily a struct.

		inline static Project* _sharedProject;
		inline static std::vector<Project*> _projects;

		//TODO: ProjectEventhandler should be the base version
		//This should always be the lowest possible version, but can also just be cast into it's respective version. I might make something to handle this easier.
		// Additionally I could make functions that act as the short hand for a lot of the functions to be used with the project event handler.
		//ProjectEventHandler* _eventHandler = nullptr;
	public:

		//Change name to find project.
		static Project* GetProject(std::string name)
		{
			if (stricmp("shared", name.c_str()) == 0)
				return _sharedProject;

			for (auto project : _projects)
			{
				std::string p_name = project->GetName();

				if (stricmp(p_name.c_str(), name.c_str()) == 0)
					return project;
			}

			return nullptr;
		}

		static Project* GetSharedProject()
		{
			return _sharedProject;
		}

		static void PrintAST(Record& tree, std::string indent = "")
		{
			const static std::string __dent = "  ";

			std::string log = tree.PrintAs<LEX::Impl::Syntax>();

			RGL_LOG(info, "{}{}", indent, log);

			indent += __dent;

			for (auto& child_rec : tree.GetChildren())
			{
				PrintAST(child_rec, indent + __dent);
			}
		}

		static APIResult CreateScript(Project* project, std::string name, std::string path)
		{
			//Might break this down into a function that knows it's path and one that doesn't, mostly for scripts created via function.
			std::string script_path = path + "/" + name + ".lex";


			std::ifstream file_input;
			file_input.open(script_path);

			std::stringstream stream;
			stream << file_input.rdbuf();
			std::string contents = stream.str();

			file_input.close();//Don't really need to do this, seeing as the destructor does, but eh



			Record ast = Impl::Parser__::CreateSyntaxTree(project->GetName(), name, contents);

			PrintAST(ast);

			Script* script = name == "Commons" ? Component::Create<CommonScript>(ast) : Component::Create<Script>(ast);

			project->AddScript(script);

			if (false) {//TODO: supposed to read "if ast has failed"
				//TODO: Have project be the thing that sends the failure to the project client, or just logs clientless failures itself.
				return APIResult::Failure;
			}

			return APIResult::Success;
		}

		static APIResult CreateProject(std::string name, ProjectClient* client, Project** out = nullptr)
		{
			//I'm thinking no api result for this, just project pointer.

			constexpr std::string_view core_path = "Data/Lexicon";

			//For now, shared won't be it's own special class. But that may change in the future.
			//bool is_shared = name == "Shared";

			//Project* project = is_shared ? Component::Create<SharedProject>() : Component::Create<Project>();
			Project* project = Component::Create<Project>();
			//static_assert(std::derived_from<Project, Component>, "fafaf");
			project->SetName(name);

			//todo - Set client

			if (out) {
				*out = project;
			}
			else {
				_projects.push_back(project);
			}

			//If the name doesn't exist, this should just use the core path (this is how we detect the commons.
			std::string path = std::string(core_path);

			if (name != "Shared")
				path += "/" + name;

			std::filesystem::path commons_path = std::filesystem::path(path + "/Commons.lex");

			//These actually shouldn't through, that's a parsing process thing. What it should actually be doing is probably returning in some failure and
			// communicating to the project client.
			if (std::filesystem::exists(commons_path) == false) {
				throw EnvironmentError("Commons not found.");
				return APIResult::Failure;
			}

			if (CreateScript(project, "Commons", path) == APIResult::Failure) {
				throw EnvironmentError("Commons not valid.");
				return APIResult::Failure;
			}

			{
				std::vector<std::pair<std::string, std::string>> scripts = SearchFiles(path, ".lex", "Commons.lex");

				for (auto full_path : scripts)
				{
					std::string path = full_path.first;
					std::string name = full_path.second;
					name = name.substr(0, name.size() - 4);

					if (CreateScript(project, name, path) == APIResult::Failure) {
						//Do minor error or something rather other.
					}
				}
			}

			return APIResult::Success;
		}



		static APIResult InitShared()
		{
			Project* project = nullptr;

			//Shared has no client, mainly because without a client errors are sent here, and there's never a situation where it will want to use format.
			auto result = CreateProject("Shared", nullptr, &project);

			_sharedProject = project;

			return result;
		}

		//DEPRECATED
		void MakeProject(std::string name)
		{
			Project* project = Component::Create<Project>();

			project->SetName(name);

			void* test = nullptr;

			if (!test) {
				throw EnvironmentError("Commons not found.");
			}

			std::string commons_content = "";

			Record ast = Impl::Parser__::CreateSyntaxTree(name, "Commons", commons_content);

			//If the produced AST for commons is erroneous the project as a whole will be deemed unusable.
			// If this is the project is shared, it will throw an exception that prevents the loading of the plugin in general.
			CommonScript* commons_script = Component::Create<CommonScript>(ast);

			project->AddScript(commons_script);

			if (stricmp(name.c_str(), "Shared") == 0)
				_sharedProject = project;
			else
				_projects.push_back(project);

			//And that's how to make a project in a nut shell.
		}


		//These are temporary, I don't have the energy right now.
		static bool LoadFile(const std::string a_path)
		{

			return true;
		}

		//SHAMELESSLY Ripped directly from po3's clib utils
		static std::vector<std::string> get_configs(std::string a_folder, std::string_view a_extension = ".ini")
		{
			std::vector<std::string> configs{};

			for (const auto iterator = std::filesystem::directory_iterator(a_folder); const auto & entry : iterator) {
				if (entry.exists()) {

					if (const auto& path = entry.path(); !path.empty() && path.extension() == a_extension) {
						if (const auto& fileName = entry.path().string(); true) {//fileName.rfind(a_suffix) != std::string::npos) {
							configs.push_back(fileName);
						}
					}
				}
			}

			std::ranges::sort(configs);

			return configs;
		}

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

				if (path.empty() || path.extension() != a_extension)
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
	};

}