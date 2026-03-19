#include "Lexicon/Engine/Script.h"
#include "Lexicon/Engine/Project.h"

//#include "Lexicon/Engine/TestField.h"

#include "Lexicon/Interfaces/IdentityManager.h"
#include "Lexicon/Engine/ConcreteType.h"
#include "Lexicon/Engine/GenericType.h"
#include "Lexicon/Engine/ConcreteFunction.h"
#include "Lexicon/Engine/GlobalVariable.h"
#include "Lexicon/Engine/ConcreteGlobal.h"
#include "Lexicon/Engine/parse_strings.h"
#include "Lexicon/Engine/Expression.h"
#include "Lexicon/Engine/TypeBase.h"
#include "Lexicon/Engine/Parser.h"



#include "Lexicon/Interfaces/ProjectClient.h"
#include "Lexicon/Interfaces/IElement.h"


namespace LEX
{






	Script* Script::GetCommons()
	{
		//This 
		return _parent->FetchCommons();
	}

	void Script::SetParent(Element* elem)
	{
		//TODO: Script::SetParent is actually supposed to ask if new parent isn't a project. Attend to that when enum is added.
		//TODO: EnvironmentError in this situation is exclusively an error on my part, need a new exception for that.
		
		Project* project = elem->GetProject();
		
		//if (elem->IsComponentType<Project>() == false)
		//	throw EnvironmentError("Parent of script must be a project.");

		if (!project || elem != project)
			report::fault::critical("Parent of script must be project.");

		_parent = project;
	}


	bool Script::IsDefined() const
	{
		return _syntaxTree;
	}



	Script* Script::GetScript()
	{
		return this;
	}
	
	Project* Script::GetProject()
	{
		return Environment::GetProject();
	}

	ComponentType Script::GetComponentType()
	{
		return typeid(Script);
	}

	SyntaxRecord* Script::GetSyntaxTree()
	{
		if (IsDefined() == false)
			throw EnvironmentError("Syntax Tree not defined.");

		return &_syntaxTree;
	}

	void Script::SetSyntaxTree(SyntaxRecord& rec)
	{
		if (IsDefined() == true)
			return;
		//This ast likely has no interest, move resources.

		_syntaxTree = std::move(rec);
		
	}


	void Script::LoadFromSyntaxNode(SyntaxRecord& node)
	{
		get_switch(node.SYNTAX().type)
		{

			case SyntaxType::Format: 
				if constexpr (1)
				{
					Project* project = GetProject();
					if (project)
						project->AddFormat(node.GetFront().GetTag(), node.GetTag(), this);
				}
				break;

			case SyntaxType::Directive:
				if constexpr (1)
				{
					//Directives should be ordered, namely that subdirectories should be made first.
					Project* parent = GetProject();

					for (auto& directive : node.children()) {
						switch (directive.SYNTAX().type)
						{
						case SyntaxType::Relationship:
						{
							RelateType type;

							switch (Hash(directive.GetTag()))
							{
							case "import"_h: type = RelateType::Imported; break;
							case "include"_h: type = RelateType::Included; break;
							default: report::compile::error("unknown relationship directive '{}' detected.", directive.GetView());
							}

							auto view = directive.GetFront().GetView();
							Script* script = parent->FindScript(view);

							if (!script) {
								report::compile::error("Cannot find script '{}'.", view);
							}

							AddRelationship(script, type);
						}
						break;

						}

					}
				}
				break;
			default:
				return __super::LoadFromSyntaxNode(node);
		}
	}


	void Script::OnAttach()
	{
		//Really I can and probably will just split this shit.
		//Record& ast = *GetSyntaxTree();
		//CompileExpression(ast);
	}


	std::string_view Script::GetName() const
	{
		if (IsDefined() == false)
			//This is more than likely a fault actually.
			throw EnvironmentError("Syntax Tree not defined, script is nameless.");

		return _syntaxTree.GetView();
	}
	//SetName will resume having no use here.

	
	void Script::CompileExpression(Record& target)
	{
		
	}

	bool Script::AppendContent(const std::string_view& content, std::span<std::string_view> options)
	{
		//In order for append to work, new objects need to  play catch up with already existing things. To do this, we can check if linkage already
		// finalized, and then finalize it again.

		//I also would need the ability to create scripts from strings, as I wouldn't want appending to be a thing for regular scripts just yet.

		//Ultimately, we just can't quite do it right, so we shouldn't bother right now. Append is important, but I'd rather like to get a make shift
		// set up going where you can just append to specific files.


		
		//Options is ignored for now. Basically does nothing. No compile time stuff either. No system for it.
		SyntaxRecord ast;

		if (Parser__::CreateSyntaxTree(ast, content, "") == false) {
			return false;
		}

		return AppendContent(ast);
	}



	void CorrectRecordPages(SyntaxRecord::Iterator begin, SyntaxRecord::Iterator end, Line line)
	{
		while (begin != end)
		{
			auto& node = *begin++;
		
			if (auto& syntax = node.GetSyntax(); syntax.line)
			{
				//TODO:Please check this shit for overflow.
				syntax.line += line;

				if (node.size() != 0)
					CorrectRecordPages(node.children().begin(), node.children().end(), line);
			}
		}
	}

	bool Script::AppendContent(SyntaxRecord& content)
	{
		if (IsIncremental() == false) {
			report::compile::warn("Script '{}' is not incremental and cannot be appended to.", GetName());
			return false;
		}


		auto content_body = content.FindChild(parse_strings::body);

		//Nothing to add, no reason to care.
		if (!content_body)
			return true;
		//if (IsIncremental() == true)

		if (auto* tree = GetSyntaxTree(); tree) {
			auto& body = tree->ObtainChild(parse_strings::body, SyntaxType::None);
			auto& children = body.children();
			
			auto index = children.size();
			//TODO: Right here, you'd want to rewrite every single syntax so it appends to the very end, instead of incorrectly being placed later.

			
			body.EmplaceChildren(std::move(content_body->children()));
			auto begin = children.begin() + index;
			auto end = children.end();

			auto& column = tree->GetSyntax().column;
			auto& line = tree->GetSyntax().line;

			CorrectRecordPages(begin, end, line);
			
			line += content.GetSyntax().line;
			column = content.GetSyntax().column;
			
			if (HasLinked(LinkFlag::Loaded) == true)
			{
				//TODO: If this hasn't reached a certain level of linkage, it shouldn't add this. Notably, loaded must have passed.
				
				try
				{
					LoadFromSyntaxTree(begin, end);
					RefreshLinkage();

				}
				catch (CompileError& error)
				{
					report::compile::failure("Failed to append content to {}", GetName());
				}
			}
			else
			{
				//TODO: This needs to emit a warning, but the warning should be quelled if some kind of flag is on it, some kind of flag that core would have.
			}
			return true;
		}
		return false;
	}



	LinkResult Script::OnLink(LinkFlag flags)
	{
		return __super::OnLink(flags);
		if (flags != LinkFlag::Loaded)
			return LinkResult::Success;


		auto body = GetSyntaxTree()->FindChild(parse_strings::body);

		if (!body) {
			logger::warn("script {} is empty", GetName());
			return LinkResult::Success;
		}

		auto& children = body->children();

		logger::info("Loading script: {}", GetName());

		LoadFromSyntaxTree(children.begin(), children.end());

		return LinkResult::Success;
	}

	LinkFlag Script::GetLinkFlags()
	{
		return LinkFlag::Loaded;
	}

	Environment* Script::FindEnvironment(SyntaxRecord& path, ITemplateInserter& inserter)
	{
		auto types = FindTypes(path.GetView());

		if (types.size() == 1)
			//result = nullptr;
			return types[0];

		return nullptr;
	}





	//Includes/Imports/Requires need to be included as concepts.
	// Script is the only thing that uses these, so no reason to branch out.
	// Also, I may not need to "Find" them for that reason.
	// Instead, asking if something is one of these things might be better.


	Script* Script::FindRelationship(std::string name, bool shared, RelateType bond)
	{
		//The shared is because between shared, 2 scripts can have the same name.
		if (bond == RelateType::None)
			return nullptr;

		return nullptr;
	}


	void Script::AddRelationship(Script* script, RelateType bond)
	{
		//Return the relationship it's been assigned or the relationship it has previously been assign if it
		// can't the relationship.
		_relationMap[bond].push_back(script);
	}




	CommonScript* CommonScript::GetCommons()
	{
		return this;
	}

	//The common ground of a project, mainly exists to some functions in script.
	std::string_view CommonScript::GetName() const
	{
		return "Commons";
	}


	//std::string CoreScript::GetName()
	//{
	//	return "CORE";
	//}





}