#include "Lexicon/Engine/Repository.h"

#include "Lexicon/Interfaces/ProjectClient.h"


#include "Lexicon/Engine/Script.h"
#include "Lexicon/Engine/Project.h"
#include "Lexicon/Engine/ParserTest.h"
#include "Lexicon/Engine/Subdirectory.h"
#include "Lexicon/Engine/Director.h"
namespace LEX
{


	static void tmp_AddCompileOptions(std::vector<std::string_view>& options, Project* project)
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


	static void PrintAST(SyntaxRecord& tree, std::string indent)
	{
		constexpr std::string_view __dent = "| ";

		std::string log = tree.Print();

		RGL_LOG(trace, "{}{}", indent, log);

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

		ScriptString(const std::string_view& n, const std::string_view& view) :
			_name{ n }, _value{ std::string(view) }
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

		std::string_view GetText(Line start, Line end, bool exclusive = false)
		{
			if (exclusive) {
				start++;
				end--;
			}

			auto first = _lines[start - 1].begin;
			auto last = _lines[end - 1].end;
			return std::string_view{ first, last };
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


		std::string& Clear()
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

				if (it != end) it++;
			}
		}

	private:
		std::string_view _name;
		std::string _value;
		std::vector<ScriptLine> _lines;

	};



	static bool CheckCondition(Record& expression, std::vector<std::string_view>& options)
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



	bool FormatProcess(ScriptString& script, Project* project, std::vector<std::string_view>& options, Record::Iterator it, Record::Iterator end)
	{
		Line start = it->get().SYNTAX().line;
		Line finish = 0;

		bool cont = true;

		int fin = 1;

		std::string_view format = it->get().GetView();
		logger::trace("format:\n {}", format);
		while (++it != end && cont)
		{
			Record& record = *it;

			record.SYNTAX().type = SyntaxType::Disposable;


			switch (Hash(record.GetTag()))
			{
			default:
				++fin;
				break;

			case "endformat"_h:
				if (--fin) {
					break;
				}
				
				finish = record.SYNTAX().line;
				cont = false;
				break;
			}
		}

		std::string_view data = script.GetText(start, finish, true);

		logger::info("format '{}':\n{}", format, data);

		if (!finish) {
			report::parse::warn("Un-ended #format detected.");
			return false;
		}

		script.SetLines(false, start, finish);

		return true;
	}


	bool ConditionalProcess(ScriptString& script, std::vector<std::string_view>& options, Record::Iterator it, Record::Iterator end)
	{
		bool enabled = CheckCondition(*it, options);

		Line start = it->get().SYNTAX().line;
		Line finish = 0;

		bool cont = true;

		bool fin = true;

		while (++it != end && cont)
		{

			Record& record = *it;

			if (record.SYNTAX().type != SyntaxType::Conditional) {
				if (!enabled)
					record.SYNTAX().type = SyntaxType::Disposable;

				continue;
			}

			//All conditionals are disposable regardless.
			record.SYNTAX().type = SyntaxType::Disposable;


			switch (Hash(record.GetTag()))
			{
			case "if"_h:
				fin = ConditionalProcess(script, options, it, end);
				if (!fin) return fin;
				break;

			case "endif"_h:
				finish = record.SYNTAX().line;
				cont = false;
				break;
			}
		}

		if (!finish) {
			report::parse::warn("Un-ended #if detected.");
			return false;
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

	bool RequireProcess(ScriptString& script, std::vector<std::string_view>& options, Record::Iterator it, Record::Iterator end)
	{
		//I would like to change how requirement works, and instead make it something that works like a global if, requiring that
		// something exists or the entire thing just gets dumped.

		bool enabled = CheckCondition(*it, options);

		if (!enabled) {
			logger::break_warn("Requirement not met for {} to load.", script.name());
			return false;
		}

		return true;
	}


	bool GeneralProcess(Project* project, ScriptString& script, Record& directives, std::vector<std::string_view>& options)
	{
		auto& dirs = directives.children();

		Record::Iterator it = dirs.begin();
		Record::Iterator end = dirs.end();

		auto _begin = options.begin();
		auto _end = options.end();

		bool result = true;

		while (it != end && result)
		{

			Record& record = *it;


			auto old = it;

			auto line = record.SYNTAX().line;

			if (script.IsLineEnabled(line) == true)
			{
				script.SetLines(false, record.SYNTAX().line);

				switch (record.SYNTAX().type)
				{
				case SyntaxType::Requirement:
					result = RequireProcess(script, options, it, end);
					break;
					if (record.GetView() == parse_strings::option_req) {
						auto& front = record.GetFront();

						if (std::find(_begin, _end, front.GetView()) == _end) {
							logger::break_warn("Requirement '{}' was not present for {} to load.", front.GetView(), script.name());
							return false;
						}
					}
					break;
				case SyntaxType::Conditional:
					result = ConditionalProcess(script, options, it, end);
					break;

				case SyntaxType::Format:
					result = FormatProcess(script, project, options, it, end);
				}
			}

			if (it == old)
				it++;
		}

		dirs.erase(std::remove_if(dirs.begin(), dirs.end(), [](Record& q) {return q.SYNTAX().type == SyntaxType::Disposable; }), dirs.end());



		return result;
	}


	bool Repository::CreateSyntaxTreeFromString(std::string_view file, std::string_view contents,
		Director& director, SyntaxRecord& ast)
	{

		if (director.HandlePreprocess(contents) == false)
			return false;

		contents = director.Clear();

		if (Parser::CreateSyntaxTree(ast, contents, file) == false) {
			return false;
		}

		PrintAST(ast);

		return true;
	}


	bool Repository::CreateSyntaxTree(std::string_view file, std::string_view extension, std::string_view path,
		Director& director, SyntaxRecord& ast)
	{
		std::string script_path = std::format("{}/{}{}", path, file, extension);

		if (std::filesystem::exists(script_path) == false) {
			report::compile::failure("File not found at {}", script_path);
			return false;
		}

		std::ifstream file_input;
		file_input.open(script_path);

		std::stringstream stream;
		stream << file_input.rdbuf();
		std::string contents = stream.str();

		file_input.close();//Don't really need to do this, seeing as the destructor does, but eh

		return CreateSyntaxTreeFromString(file, contents, director, ast);

	}



	//TODO: I'd like to make this virtual function a wrapper function for the create, which would allow it to create a commons not by name but by value
	// so basically, if an "is_commons" variable was flipped it would make it commons regardless of name, if not, commons would be a strongly
	// disallowed name.
	Script* Repository::CreateScriptImpl(const std::string_view& name, std::span<std::string_view> opts, const std::string_view& path, std::optional<std::string_view> content)
	{
		return MakeScript(name, false, opts, path, content);
	}

	Script* Repository::MakeScript(const std::string_view& name, bool is_commons, std::span<std::string_view> opts, std::string_view path, std::optional<std::string_view> content)
	{
		if (name == "Commons" && is_commons) {
			report::compile::failure("Cannot create a non-commons script named Commons.");
			return nullptr;
		}


		std::string_view script_name = is_commons ? "Commons" : name;

		if (FindScript(script_name) != nullptr) {
			report::compile::failure("Script named '{}' already exists in {}", script_name, GetName());
			return nullptr;
		}


		if (path.empty() == true) {
			path = GetFilepath();
		}


		//Path and name are still included, as they can be used to help refer to the location of the file when debugging.
		SyntaxRecord ast;

		bool made_from_script;

		std::vector<std::string_view> options{ opts.begin(), opts.end() };

		//const std::string_view& nm, Project* pro, std::span<std::string_view> ops
		Director director{ name, GetProject(), opts };

		{
			bool result;

			if (content.has_value()) {
				made_from_script = true;
				result = CreateSyntaxTreeFromString(script_name, content.value(), director, ast);
			}
			else {
				made_from_script = false;
				result = CreateSyntaxTree(script_name, ".lsi", path, director, ast);
			}

			if (!result){
				return nullptr;
			}
		}

		Script* script = is_commons ? Component::Create<CommonScript>(ast) : Component::Create<Script>(ast);
		
		AddScript(script);

		director.HandlePostprocess(script);

		if (!is_commons && !IsBatchLoading())
			Component::RelinkComponents();

		return script;
	}


	void Repository::AddScript(Script* script)
	{

		if (script->IsCommons() == true) {
			report::fault::error("Cannot add a CommonScript to non-project repository {}", GetName());
		}
		
		auto end = _scripts.end();
		
		//Should search for script, throwing if within, then call SetParent on the script.
		if (auto it = std::find(_scripts.begin(), end, script); it != end)
			report::compile::error("Script '{}' already exists in repo {}", script->GetName(), GetName());

		_scripts.push_back(script);
		
		DeclareParentTo(script);

	}


	Script* Repository::FindScriptImpl(const std::string_view& name)
	{
		auto end = _scripts.end();


		//Proper version of Script not implement
		auto it = std::find_if(_scripts.begin(), end, [&](Script* search) { return search->GetName() == name; });

		if (it != end) {
			return *it;
		}

		return nullptr;
	}


	Directory* Repository::FindDirectory(SyntaxRecord& record, ITemplateInserter* inserter)
	{
		return FindScript(record.GetView());
	}

	Subdirectory* Repository::FindSubdirectoryImpl(const std::string_view& name)
	{
		auto end = _subdirectories.end();


		//Proper version of Script not implement
		auto it = std::find_if(_subdirectories.begin(), end, [&](Subdirectory* search) { return search->GetName() == name; });

		if (it != end) {
			return *it;
		}

		return nullptr;
	}

	Subdirectory* Repository::CreateSubdirectoryImpl(const std::string_view& name, Script* sub_to, std::span<std::string_view> options, std::string_view path)
	{

		if (auto subdirectory = FindSubdirectory(name)) {
			if (sub_to || subdirectory->IsSubproject() == true) {
				report::compile::failure("subproject named '{}' already exists", name);
				return nullptr;
			}
		}

		Subdirectory* directory = Component::Create<Subdirectory>();
		Directory* parent = sub_to ? (Directory*)sub_to : this;

		directory->SetName(name);

		parent->DeclareParentTo(directory);
		//directory->SetParent(sub_to ? (Directory*)sub_to : this);

		std::string dir_path;

		if (path.empty() == true) {
			dir_path = std::format("{}/{}", GetFilepath(), name);
		}
		else {
			dir_path = path;
		}

		directory->SetFilepath(dir_path);

		directory->LoadRepository(options);

		_subdirectories.push_back(directory);

		return directory;
	}

	void Repository::LoadRepository(const std::span<std::string_view>& options)
	{
	
		assert_if (IsDirectoryLoaded() == true)
			return;

		GetFlags() |= Flag::kDirectoryLoaded;

		//If the name doesn't exist, this should just use the core path (this is how we detect the commons.
		//std::string path = std::string(SettingManager::GetSingleton()->dataDir) + "/scripts";
		std::string_view path = GetFilepath();

		std::filesystem::path commons_path = std::filesystem::path(std::format("{}/Commons.lsi", path));

		//These actually shouldn't through, that's a parsing process thing. What it should actually be doing is probably returning in some failure and
		// communicating to the project client.

		{
			if (std::filesystem::exists(commons_path) && GetCommons() == nullptr) {
				//TODO: This for some reason seems to be triggering everywhere
				report::compile::warn("Repository {} detected a unused commons script at {}", GetName(), path);
			}

			{
				//Commons will always be ignored in repositories from this perspective, a different thing handles this
				std::vector<std::pair<std::string, std::string>> scripts = SearchFiles(path, ".lsi", "Commons.lsi");

				auto last = std::unique(scripts.begin(), scripts.end());

				if (auto end = scripts.end(); last != end) {
					logger::warn("Copies of scripts present in file search");
					scripts.erase(last, scripts.end());
				}


				SetBatchLoading(true);


				for (auto& [path, name] : scripts)
				{
					name = name.substr(0, name.size() - 4);

					if (CreateScript(name, options, path, std::nullopt) == nullptr) {
						//TODO: Do minor error or something rather other.
					}
				}

				SetBatchLoading(false);
			}
		}

		DeclareOrphan();
		
	}


}