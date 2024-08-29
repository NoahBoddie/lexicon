#pragma once

#include "Lexicon/Interfaces/ProjectManager.h"

#include "Lexicon/Engine/ParserTest.h"
#include "Lexicon/Exception.h"//May move to src


#include "Lexicon/ElementType.h"


#include "Lexicon/Engine/Component.h"
#include "Lexicon/Engine/Project.h"
#include "Lexicon/Engine/Script.h"
#include "Lexicon/Engine/Parser.h"
#include "Lexicon/Engine/Expression.h"
#include "Lexicon/Engine/DefaultClient.h"

#include "Lexicon/Engine/SettingManager.h"
namespace LEX
{


	inline static Project* _shared;
	inline static std::vector<Project*> _projects;


	inline static std::unordered_map<size_t, std::pair<IElement*, ElementType>> _lookupMap;


	constexpr std::string_view cmpExtension = "lex";//This is prefered
	constexpr std::string_view prsExtension = "lsp";//Then this
	constexpr std::string_view intExtension = "lsi";//Then this.
	constexpr std::string_view srcExtension = "lsc";

	//constexpr std::string_view core_path = "Data/Lexicon";
	constexpr std::string_view core_path = "C:/Users/Noah/Desktop/Projects/[Project Data]/Mod Projects/Arithmetic/3.0/{scripted code}/Lexicon";








	//Move this to some  one off junk folder.
	struct PathParser : public LEX::Impl::ParseModule, public LEX::Impl::IdenDeclBoilerPlate
	{
		//The idea of this is it's a simple parser that acts like the identifier parser, but will handle this in a way that can handle
		bool CanHandle(LEX::Impl::Parser* parser, Record* target, LEX::Impl::ParseFlag flag) const override
		{
			return true;
		}

		bool IsAtomic() const override
		{
			return true;
		}


		Record _HandleThis(LEX::Impl::Parser* parser)
		{
			RecordData next = parser->next();
			next.GetTag() = parse_strings::this_word;
			return LEX::Impl::Parser::CreateExpression(next, SyntaxType::Field);
		}


		Record HandleToken(LEX::Impl::Parser* parser, Record*) override
		{

			return _HandlePath(parser, SyntaxType::ProjectName);

		}
		// Need some boilerplate resolver.
		std::string_view GetModuleName() override
		{
			return typeid(PathParser).name();
		}

		bool ContextAllowed(LEX::Impl::ProcessContext*, LEX::Impl::ProcessChain*) override
		{
			//This prevents anything from following it up for the most part. This shit is a one man show!
			// If this causes any issues with parsers I may use later, feel free to make a variable to help handle when this is allowed be handled.
			return false;
		}
	};






	static IElement* LookupElement(size_t hash, ElementType type)
	{
		auto it = _lookupMap.find(hash);

		if (_lookupMap.end() == it || it->second.second == type)
			return nullptr;

		return it->second.first;
	}

	static void SaveElement(IElement* elem, size_t hash, ElementType type)
	{

		_lookupMap[hash] = std::make_pair(elem, type);

	}

	static size_t HashPath(std::string_view path)
	{
		//TODO: -Check: I think unordered map already hashes stuff so maybe not needed. Still will handle later
		std::hash<std::remove_reference_t<decltype(path)>> hasher{};

		return hasher(path);

	}

	static void PrintAST(Record& tree, std::string indent = "")
	{
		constexpr std::string_view __dent = "| ";

		std::string log = tree.PrintAs<LEX::Syntax>();

		RGL_LOG(info, "{}{}", indent, log);

		indent += __dent;

		for (auto& child_rec : tree.GetChildren())
		{
			PrintAST(child_rec, indent);
		}
	}
	




	IProject* ProjectManager::INT_NAME(GetProject)(std::string_view name)
	{
		if (stricmp("Shared", name.data()) == 0)
			return _shared;

		for (auto project : _projects)
		{
			std::string_view p_name = project->GetName();

			if (stricmp(p_name.data(), name.data()) == 0)
				return project;
		}

		return nullptr;
	}

	bool tmp_CheckCompileOptions(std::string_view tag)
	{
		logger::critical("tag {}", tag);
		return tag == "Test";
	}

	void tmp_AddCompileOptions(std::vector<std::string_view>& options, Project* project)
	{
		if (project && project->client())
		{
			auto client = project->client();

			if (client) {
				int i = 0;

				std::string_view new_opt;

				while (new_opt = client->GetCompileOptions(i++), new_opt.empty() == false)
				{
					options.push_back(new_opt);
				}
			}
		}


	}

	APIResult ProjectManager::CreateScript(Project* project, std::string_view name, std::string_view path, Script** out, const std::vector<std::string_view>* opts)
	{
		//Might break this down into a function that knows it's path and one that doesn't, mostly for scripts created via function.
		std::string script_path = std::format("{}/{}.lsi", path, name);


		std::ifstream file_input;
		file_input.open(script_path);

		std::stringstream stream;
		stream << file_input.rdbuf();
		std::string contents = stream.str();

		file_input.close();//Don't really need to do this, seeing as the destructor does, but eh


		std::vector<std::string_view> options = opts ? *opts : std::vector<std::string_view>{};


		tmp_AddCompileOptions(options, project);

		if (project->IsShared() == false)
		{
			tmp_AddCompileOptions(options, GetShared());
		}


		{
			Impl::PreprocessorParser direct_parse;
			Record tmp_directives = Impl::Parser__::CreateSyntaxTree(std::string{ project->GetName() }, std::string{ name }, contents, &direct_parse);



			if (tmp_directives.size() != 0) {
				auto begin = options.begin();
				auto end = options.end();

				for (auto& directive : tmp_directives.GetChildren()) {
					if (directive.SYNTAX().type == SyntaxType::Requirement && directive.GetView() == parse_strings::option_req) {
						auto& front = directive.GetFront();
						
						//if (tmp_CheckCompileOptions(front.GetView()) == false) {
						if (std::find(begin, end, front.GetView()) == end) {
							logger::break_warn("Requirement '{}' was not present for {} to load.", front.GetView(), name);
							return APIResult::MissingRequirement;
						}
					}
				}
				std::stringstream result;

				//const boost::regex regex(R"((?<=\A|\n)\s*#[^\n]*)");
				const boost::regex regex(R"((?<=\A)|(?<=\n)\s*#[^\n]*)");
				boost::regex_replace(std::ostream_iterator<char>(result), contents.begin(), contents.end(), regex, "");
				contents = result.str();
			}
		}
		

		Record ast = Impl::Parser__::CreateSyntaxTree(std::string{ project->GetName() }, std::string{ name }, contents);

		PrintAST(ast);

		//std::system("pause");

		Script* script = name == "Commons" ? Component::Create<CommonScript>(ast) : Component::Create<Script>(ast);

		if (project->GetCommons() != nullptr && name == "Commons")
		{
			return APIResult::ElementExists;
		}

		project->AddScript(script);
			
		if (out) {
			*out = script;
		}


		if (false) {//TODO: supposed to read "if ast has failed"
			//TODO: Have project be the thing that sends the failure to the project client, or just logs clientless failures itself.
			return APIResult::AbstractFailure;
		}

		return APIResult::Success;
	}


	APIResult ProjectManager::CreateProject(std::string_view name, ProjectClient* client, Project** out, HMODULE source)
	{
		//I'm thinking no api result for this, just project pointer.


		//For now, shared won't be it's own special class. But that may change in the future.
		//bool is_shared = name == "Shared";

		//Project* project = is_shared ? Component::Create<SharedProject>() : Component::Create<Project>();
		Project* project = Component::Create<Project>();
		//static_assert(std::derived_from<Project, Component>, "fafaf");
		project->SetName(name);

		//todo - Set client
		project->_client = client;
		if (out) {
			*out = project;
		}
		else {
			_projects.push_back(project);
		}

		//If the name doesn't exist, this should just use the core path (this is how we detect the commons.
		//std::string path = std::string(core_path);
		std::string path = std::string(SettingManager::GetSingleton()->scriptDir);

		if (name != "Shared") {
			path += "/";
			path += name;
		}
		std::filesystem::path commons_path = std::filesystem::path(path + "/Commons.lsi");

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
			std::vector<std::pair<std::string, std::string>> scripts = SearchFiles(path, ".lsi", "Commons.lsi");

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


	IElement* ProjectManager::GetElementFromPath(std::string_view path, ElementType elem)
	{
		

		//right now, linking isn't really set up so you know.
		if (1!=1 && Component::HasLinked(LinkFlag::Declaration) == false) {
			//Tell that this is too early to be valid.
			return nullptr;
		}


		auto hash = HashPath(path);

		IElement* element = LookupElement(hash, elem);


		if (!element)
		{
			//Record record = LEX::Impl::Parser__::CreateSyntax<PathParser>(std::string{ path });
			Record record = LEX::Impl::Parser__::CreateSyntax<Impl::IdentifierParser>(std::string{ path });

			//<!> After parsing is done, the top most type needs to be set to the standards of what it's expecting.

			//From here, use the path functions that are in environment.
			// The project should be found from the first part, and from there we should just keep the environ search should finish it out.
			
			switch (elem)
			{
				//Do the search for each type here.
			case kFuncElement:
			case kTypeElement:
			case kGlobElement:
				element = Element::GetElementFromPath(nullptr, path, elem); 
				break;
			default:
				report::critical("Unknown element type {} requested, and cannot be processed.", (int)elem);
				break;
			}


			if (element) {
				SaveElement(element, hash, elem);
			}

		}

		return element;
	}

	APIResult  ProjectManager::InitMain()
	{
		if (_shared)
			return APIResult::ElementExists;

		Project* project = nullptr;

		//Shared has no client, mainly because without a client errors are sent here, and there's never a situation where it will want to use format.
		auto result = CreateProject("Shared", DefaultClient::GetInstance(), &project);

		_shared = project;

		return result;
	}


}