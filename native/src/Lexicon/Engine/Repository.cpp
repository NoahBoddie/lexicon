#include "Lexicon/Engine/Repository.h"

#include "Lexicon/Interfaces/ProjectClient.h"


#include "Lexicon/Engine/Script.h"
#include "Lexicon/Engine/Project.h"
#include "Lexicon/Engine/ParserTest.h"
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

	bool ConditionalProcess(ScriptString& script, std::vector<std::string_view>& options, Record::Iterator it, Record::Iterator end)
	{
		bool enabled = CheckCondition(*it, options);

		Line start = it->SYNTAX().line;
		Line finish = 0;

		bool cont = true;

		bool fin = true;

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
				if (!fin) return fin;
				break;

			case "endif"_h:
				finish = it->SYNTAX().line;
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


	bool GeneralProcess(ScriptString& script, Record& directives, std::vector<std::string_view>& options)
	{
		auto& dirs = directives.children();

		Record::Iterator it = dirs.begin();
		Record::Iterator end = dirs.end();

		auto _begin = options.begin();
		auto _end = options.end();

		bool result = true;

		while (it != end && result)
		{
			auto old = it;

			auto line = it->SYNTAX().line;

			if (script.IsLineEnabled(line) == true)
			{
				script.SetLines(false, it->SYNTAX().line);

				switch (Hash(it->GetView()))
				{

				}

				switch (it->SYNTAX().type)
				{
				case SyntaxType::Requirement:
					result = RequireProcess(script, options, it, end);
					break;
					if (it->GetView() == parse_strings::option_req) {
						auto& front = it->GetFront();

						if (std::find(_begin, _end, front.GetView()) == _end) {
							logger::break_warn("Requirement '{}' was not present for {} to load.", front.GetView(), script.name());
							return false;
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


	bool Repository::CreateSyntaxTreeFromString(std::string_view file, std::string_view contents,
		std::vector<std::string_view>& options, SyntaxRecord& ast)
	{

		Project* project = GetProject();

		if (project)
		{
			tmp_AddCompileOptions(options, project);

			if (project->IsShared() == false)
			{
				tmp_AddCompileOptions(options, ProjectManager::instance->GetShared());
			}
		}

		ScriptString test_content{ file, contents };


		Record tmp_directives;


		PreprocessorParser direct_parse;


		//tmp_directives = Parser__::CreateSyntaxTree(std::string{ project->GetName() }, std::string{ name }, contents, &direct_parse);

		if (Parser__::CreateSyntaxTree(tmp_directives, contents, file, &direct_parse) == false) {
			return false;
		}

		if (GeneralProcess(test_content, tmp_directives, options) == false)
			return false;
		else
		{
			contents = test_content.Clear();
		}



		if (Parser__::CreateSyntaxTree(ast, contents, file) == false) {
			return false;
		}

		ast.ObtainChild(parse_strings::body, SyntaxType::None).EmplaceChild(std::move(tmp_directives));

		PrintAST(ast);

		return true;
	}


	bool Repository::CreateSyntaxTree(std::string_view file, std::string_view extension, std::string_view path,
		std::vector<std::string_view>& options, SyntaxRecord& ast)
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

		return CreateSyntaxTreeFromString(file, contents, options, ast);

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




		{
			bool result;

			if (content.has_value()) {
				made_from_script = true;
				result = CreateSyntaxTreeFromString(script_name, content.value(), options, ast);
			}
			else {
				made_from_script = false;
				result = CreateSyntaxTree(script_name, ".lsi", path, options, ast);
			}

			if (!result){
				return nullptr;
			}
		}

		Script* script = is_commons ? Component::Create<CommonScript>(ast) : Component::Create<Script>(ast);

		for (auto& entry : options)
		{
			constexpr std::string_view inc = "incremental";

			if (strnicmp(entry.data(), inc.data(), inc.size()) == 0) {
				script->SetIncremental(true);
				report::debug("Script {} made incremental.", script->GetName());
			}
		}



		AddScript(script);

		Component::RefreshLinkage();

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
}