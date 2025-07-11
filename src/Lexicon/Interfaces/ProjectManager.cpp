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

#include "Lexicon/Engine/Signature.h"

#include "Lexicon/Engine/ConcreteFunction.h"

namespace LEX
{


	inline static Project* _shared;
	
	inline static std::vector<Project*> _projects;
	
	//This is the core script. Regardless of the name of the script, they all should get included into this, the core script.
	// This script cannot be added to except by from here.
	// When shared commons is created, this is automatically added to it.
	inline static Script* _core = nullptr;



	inline static std::unordered_map<size_t, std::pair<IElement*, ElementType>> _lookupMap;


	constexpr std::string_view cmpExtension = "lex";//This is prefered
	constexpr std::string_view prsExtension = "lsp";//Then this
	constexpr std::string_view intExtension = "lsi";//Then this.
	constexpr std::string_view srcExtension = "lsc";

	//constexpr std::string_view core_path = "Data/Lexicon";
	constexpr std::string_view core_path = "C:/Users/Noah/Desktop/Projects/[Project Data]/Mod Projects/Arithmetic/3.0/{scripted code}/Lexicon";








	//Move this to some  one off junk folder.
	struct PathParser : public LEX::ParseModule, public LEX::IdenDeclBoilerPlate
	{
		//The idea of this is it's a simple parser that acts like the identifier parser, but will handle this in a way that can handle
		bool CanHandle(ParsingStream* parser, Record* target, LEX::ParseFlag flag) const override
		{
			return true;
		}

		bool IsAtomic() const override
		{
			return true;
		}


		Record _HandleThis(ParsingStream* parser)
		{
			RecordData next = parser->next();
			next.GetTag() = parse_strings::this_word;
			return ParsingStream::CreateExpression(next, SyntaxType::Field);
		}


		Record HandleToken(ParsingStream* parser, Record*) override
		{

			return _HandlePath(parser, SyntaxType::ProjectName);

		}
		
		bool ContextAllowed(ParseModule*, ModuleChain*) override
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

	static void PrintAST(SyntaxRecord& tree, std::string indent)
	{
		constexpr std::string_view __dent = "| ";

		std::string log = tree.Print();

		RGL_LOG(debug, "{}{}", indent, log);

		indent += __dent;

		for (auto& child_rec : tree.children())
		{
			PrintAST(child_rec, indent);
		}
	}


	static void PrintAST(SyntaxRecord& tree)
	{
		PrintAST(tree, "");
		
		logger::debug("Record uses {} Kilobytes", tree.GetMemoryUsage() / 1000.f);
	}




	IProject* ProjectManager::GetProject(std::string_view name, EXTERN_TYPE)
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


	Project* ProjectManager::GetShared(INTERN_TYPE)
	{
		return static_cast<Project*>(GetShared(EXTERN_TYPE{}));
	}

	Script* ProjectManager::GetCore()
	{
		return _core;
	}

	Project* ProjectManager::GetProject(std::string_view name, INTERN_TYPE)
	{
		return static_cast<Project*>(GetProject(name, EXTERN_TYPE{}));
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




	class ScriptString
	{
	public:
		using iterator = std::string::iterator;

		static constexpr auto _max = max_value<Line>;

	private:

		struct ScriptLine
		{
			ScriptLine(iterator b, iterator e) : begin{ b }, end{ e } {}

			iterator begin;
			iterator end;


			bool enabled = true;	//If it's not enabled it will remove everything within the line but keep the line. Will be disabled in reverse
			//
		};

	public:

		ScriptString(std::string_view n, const char* str) : ScriptString{ n, std::string_view {str} }
		{

		}


		ScriptString(std::string_view n, std::string& str) : ScriptString{ n , std::string_view {str} }
		{
			
		}

		ScriptString(std::string_view n, std::string_view view) : _name{ n }, _value{std::string(view)}
		{
			Init();
		}

		std::string_view GetLine(Line i)
		{
			if (--i == _max)
				return "";

			auto& line = _lines[i];
			return std::string_view{ line.begin, line.end };

		}

		bool IsLineEnabled(Line i)
		{
			if (--i == _max)
				return "";

			auto& line = _lines[i];
			return line.enabled;

		}


		void SetLines(bool value, Line begin, Line end = _max)
		{
			if (begin == _max)
				return;

			begin--;

			

			if (end == _max || end == 0)
				end = begin;
			else
				end--;

			if (begin > end) {
				std::swap(begin, end);
			}

			for (; begin <= end; begin++)
			{
				_lines[begin].enabled = value;
			}
		}


		std::string Clear()
		{
			if (_lines.size() == 0)
				return _value;

			//after using this it becomes invalid.
			for (int i = (int)_lines.size() - 1; i >= 0; i--)
			{
				auto& line = _lines[i];

				if (!line.enabled)
					_value.erase(line.begin, line.end);
			}

			return _value;
		}

		std::string_view name()
		{
			return _name;
		}

	private:

		void Init()
		{
			auto it = _value.begin();
			auto end = _value.end();

			while (it != end)
			{
				auto begin = it;

				it = std::find(it, end, '\n');

				_lines.emplace_back(begin, it);
				
				if (it!= end) it++;
			}
		}

	private:
		std::string_view _name;
		std::string _value;
		std::vector<ScriptLine> _lines;

	};



	bool CheckCondition(Record& expression, std::vector<std::string_view>& options)
	{
		//At a later point this will take an expression of basic numbers and such and try to compile it. No functions, just literals and possible macros.
		//For now there's only one option.

		auto& it = expression.GetFront();

		switch (it.SYNTAX().type)
		{
		case SyntaxType::Requirement:
		case SyntaxType::Prefunc:
			switch (Hash(it.GetTag()))
			{
				case "option"_h:
				{
					auto begin = options.begin();
					auto end = options.end();

					return std::find(begin, end, it.GetFront().GetView()) != end;
				}
			}
		}

		
		return false;

	}

	APIResult ConditionalProcess(ScriptString& script, std::vector<std::string_view>& options, Record::Iterator it, Record::Iterator end)
	{
		bool enabled = CheckCondition(*it, options);

		Line start = it->SYNTAX().line;
		Line finish = 0;

		bool cont = true;

		APIResult fin = APIResult::Success;
		
		while (++it != end && cont)
		{

			if (it->SYNTAX().type != SyntaxType::Conditional) {
				if (!enabled)
					it->SYNTAX().type = SyntaxType::Disposable;
				
				continue;
			}
			
			//All conditionals are disposable regardless.
			it->SYNTAX().type = SyntaxType::Disposable;


			switch (Hash(it->GetTag()))
			{
			case "if"_h:
				fin = ConditionalProcess(script, options, it, end);
				if (fin != APIResult::Success) return fin;
				break;

			case "endif"_h:
				finish = it->SYNTAX().line;
				cont = false;
				break;
			}
		}

		if (!finish) {
			report::parse::warn("Un-ended #if detected.");
			return APIResult::CreationFailed;
		}

		if (enabled)
		{
			script.SetLines(false, start);
			script.SetLines(false, finish);
		}
		else
		{
			script.SetLines(false, start, finish);
		}
		return fin;
	}
	
	APIResult RequireProcess(ScriptString& script, std::vector<std::string_view>& options, Record::Iterator it, Record::Iterator end)
	{
		//I would like to change how requirement works, and instead make it something that works like a global if, requiring that
		// something exists or the entire thing just gets dumped.

		bool enabled = CheckCondition(*it, options);
		
		if (!enabled){
			logger::break_warn("Requirement not met for {} to load.", script.name());
			return APIResult::MissingRequirement;
		}

		return APIResult::Success;
	}


	APIResult GeneralProcess(ScriptString& script, Record& directives, std::vector<std::string_view>& options)
	{
		auto& dirs = directives.children();

		Record::Iterator it = dirs.begin();
		Record::Iterator end = dirs.end();

		auto _begin = options.begin();
		auto _end = options.end();

		APIResult result = APIResult::Success;

		while (it != end && result == APIResult::Success)
		{
			auto old = it;

			auto line = it->SYNTAX().line;

			if (script.IsLineEnabled(line) == true)
			{
				script.SetLines(false, it->SYNTAX().line);

				switch (it->SYNTAX().type)
				{
				case SyntaxType::Requirement:
					result = RequireProcess(script, options, it, end);
					break;
					if (it->GetView() == parse_strings::option_req) {
						auto& front = it->GetFront();

						//if (tmp_CheckCompileOptions(front.GetView()) == false) {
						if (std::find(_begin, _end, front.GetView()) == _end) {
							logger::break_warn("Requirement '{}' was not present for {} to load.", front.GetView(), script.name());
							return APIResult::MissingRequirement;
						}
					}
					break;
				case SyntaxType::Conditional:
					result = ConditionalProcess(script, options, it, end);
					break;

				}
			}

			if (it == old)
				it++;
		}

		dirs.erase(std::remove_if(dirs.begin(), dirs.end(), [](Record& q) {return q.SYNTAX().type == SyntaxType::Disposable; }), dirs.end());
		


		return result;
	}



	APIResult CreateSyntaxTreeFromString(std::string_view file, std::string& contents, Project* project,
		std::vector<std::string_view>& options, SyntaxRecord& ast)
	{
		//For now this won't do anything I guess.
		if (project)
		{
			tmp_AddCompileOptions(options, project);

			if (project->IsShared() == false)
			{
				tmp_AddCompileOptions(options, _shared);
			}
		}

		ScriptString test_content{ file, contents };


		Record tmp_directives;


		PreprocessorParser direct_parse;


		//tmp_directives = Parser__::CreateSyntaxTree(std::string{ project->GetName() }, std::string{ name }, contents, &direct_parse);

		if (Parser__::CreateSyntaxTree(tmp_directives, contents, file, &direct_parse) == false) {
			return APIResult::CreationFailed;
		}

		if (APIResult res = GeneralProcess(test_content, tmp_directives, options); res != APIResult::Success)
			return res;
		else
		{
			contents = test_content.Clear();
		}



		if (Parser__::CreateSyntaxTree(ast, contents, file) == false) {
			return APIResult::CreationFailed;
		}

		ast.EmplaceChild(std::move(tmp_directives));

		PrintAST(ast);

		return APIResult::Success;
	}


	APIResult CreateSyntaxTree(std::string_view file, std::string_view extension, std::string_view path, Project* project, 
		std::vector<std::string_view>& options, SyntaxRecord& ast)
	{
		std::string script_path = std::format("{}/{}{}", path, file, extension);


		std::ifstream file_input;
		file_input.open(script_path);

		std::stringstream stream;
		stream << file_input.rdbuf();
		std::string contents = stream.str();

		file_input.close();//Don't really need to do this, seeing as the destructor does, but eh

		return CreateSyntaxTreeFromString(file, contents, project, options, ast);
		
	}


	APIResult ProjectManager::CreateScript(IProject* a_project, std::string_view name, std::string_view path, std::optional<std::string_view> content, IScript** out, std::span<std::string_view> a_options)
	{
		//Path and name are still included, as they can be used to help refer to the location of the file when debugging.
		SyntaxRecord ast;
	
		APIResult result;

		bool made_from_script;

		auto project = dynamic_cast<Project*>(a_project);

		std::vector<std::string_view> options { a_options.begin(), a_options.end() };

		if (content.has_value() == true) {
			made_from_script = true;
			std::string send = std::string{ content.value() };
			result = CreateSyntaxTreeFromString(name, send, project, options, ast);
		}
		else {
			made_from_script = false;
			result = CreateSyntaxTree(name, ".lsi", path, project, options, ast);
		}


		if (result != APIResult::Success)
		{
			return result;
		}

		Script* script = name == "Commons" ? Component::Create<CommonScript>(ast) : Component::Create<Script>(ast);

		for (auto& entry : options)
		{
			constexpr std::string_view inc = "incremental";

			if (strnicmp(entry.data(), inc.data(), inc.size()) == 0) {
				script->SetIncremental(true);
				report::debug("Script {} made incremental.", script->GetName());
			}
		}
		


		//TODO: This is a small memory leak. do this check sooner.
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

		Component::RefreshLinkage();

		return APIResult::Success;
	}


	APIResult ProjectManager::CreateProject(std::string_view name, ProjectClient* client, IProject** out, HMODULE source)
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
		
		if (strnicmp(name.data(), "shared", name.size()) != 0) {
			_projects.push_back(project);
		}

		//If the name doesn't exist, this should just use the core path (this is how we detect the commons.
		//std::string path = std::string(SettingManager::GetSingleton()->dataDir) + "/scripts";
		std::string path = std::string(SettingManager::GetSingleton()->dataDir) + "/scripts";

		path += "/";
		path += name;


		std::filesystem::path commons_path = std::filesystem::path(path + "/Commons.lsi");

		//These actually shouldn't through, that's a parsing process thing. What it should actually be doing is probably returning in some failure and
		// communicating to the project client.
		if (std::filesystem::exists(commons_path) == false) {
			throw EnvironmentError(std::format("Commons not found at {}.", commons_path.string()));
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


	IElement* ProjectManager::GetElementFromPath(std::string_view path, ElementType elem, const ISignature* sign)
	{
		//right now, linking isn't really set up so you know.
		if (Component::HasLinked(LinkFlag::Declaration) == false) {
			report::warn("Declaration linkage hasn't occured, search for {} cannot be completed.", path);
			return nullptr;
		}


		//The hash path system is kinda useless without regard for signatures.
		//size_t hash = HashPath(path);
		IElement* element = nullptr;//LookupElement(hash, elem);


		if (!element)
		{
			//<!> After parsing is done, the top most type needs to be set to the standards of what it's expecting.

			//From here, use the path functions that are in environment.
			// The project should be found from the first part, and from there we should just keep the environ search should finish it out.
			


			switch (elem)
			{
				//Do the search for each type here.
			case kFuncElement:
			case kTypeElement:
			case kGlobElement:
			case kScrpElement:
			{
				Signature key = sign ? Signature{ *sign } : Signature{};
				element = Element::GetElementFromPath(nullptr, path, elem, sign ? &key : nullptr);
				break;
			}
				
			default:
				report::critical("Unknown element type {} requested, and cannot be processed.", magic_enum::enum_name(elem));
				break;
			}


			//if (element) {
			//	SaveElement(element, hash, elem);
			//}

		}

		return element;
	}




	void CompileCoreScripts(std::string_view path)
	{
		//remake to core script
		_core = Component::Create<Script>(SyntaxRecord{ "__CORE__", Syntax{SyntaxType::Script} });

		_core->SetIncremental(true);

		std::filesystem::path full_path = std::filesystem::path(path);

		//This doesn't work if not searched from core.
		std::vector<std::pair<std::string, std::string>> scripts = SearchFiles(path, ".lsi");

		for (auto full_path : scripts)
		{
			std::string path = full_path.first;
			std::string name = full_path.second;
			name = name.substr(0, name.size() - 4);


			SyntaxRecord ast;

			std::vector<std::string_view> _{};

			if (auto result = CreateSyntaxTree(name, ".lsi", path, _shared, _, ast); result != APIResult::Success)
			{
				//report failure
				break;
			}

			_core->AppendContent(ast);
		}
	}




	APIResult ProjectManager::CreateScript(Project* project, std::string_view name, std::string_view path, std::string_view content, Script*& out)
	{
		IScript* buffer = nullptr;

		auto result = CreateScript(project, name, path, content, std::addressof(buffer));

		out = static_cast<Script*>(buffer);

		return result;
	}

	APIResult ProjectManager::CreateProject(std::string_view name, ProjectClient* client, Project*& out, HMODULE source)
	{
		IProject* buffer = nullptr;

		auto result = CreateProject(name, client, std::addressof(buffer), source);

		out = static_cast<Project*>(buffer);

		return result;
	}



	APIResult  ProjectManager::InitMain()
	{
		if (_shared || _core)
			return APIResult::ElementExists;


		Project* project = nullptr;

		//Shared has no client, mainly because without a client errors are sent here, and there's never a situation where it will want to use format.
		auto result = CreateProject("Shared", DefaultClient::GetInstance(), project);

		_shared = project;


		CompileCoreScripts(SettingManager::GetSingleton()->coreDir);
		
		project->GetCommons()->AddRelationship(_core, RelateType::Included);

		project->DeclareParentTo(_core);

		return result;
	}


}