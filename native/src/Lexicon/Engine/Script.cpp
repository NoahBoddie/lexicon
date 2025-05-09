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
	//TODO: ObtainPolicy needs to be moved else where at some point.

	template <typename T>
	TypeBase* _PolicyMaker(std::string name, TypeOffset offset)
	{//helps with generic or concrete divide
		return new T{ std::string_view{name}, offset };
	}

	TypeOffset RecordToInt(SyntaxRecord& ast)
	{//helps with generic or concrete divide
		std::string tag = ast.GetTag();

		if (std::strncmp(tag.c_str(), "0x", 2) == 0 || std::strncmp(tag.c_str(), "0X", 2) == 0)
		{
			return std::stoi(tag, nullptr, 16);
		}
		else
		{
			return std::stoi(tag, nullptr, 10);
		}
	}





	inline TypeBase* Script::tempObtainPolicy(SyntaxRecord& ast, Element* parent)
	{
		SyntaxRecord& settings = ast.GetChild(0);




		//This part can be done on the policy, what really needs to be done is figuring out which policy to create, 
		// or to create at all. This relies on 1 setting. The rest can be fed verbatum later.

		//NOTE, this also includes template arguments.


		//Rules of obtaining
		//Intrinsic, no creation, just pull a policy. Doesn't matter what else it is.
		//ISpecial-Creates type policy from specific string and integer. Link error if not found.
		//Data, Creates TypePolicy plain, claiming the next free space.
		//Generic, a different TypePolicy has to be used, but otherwise it's fine.


		SyntaxRecord* genericSet = settings.FindChild(parse_strings::generic);
		bool is_generic = parent && parent->IsGenericElement() || genericSet && genericSet->size();


		using PolicyCtor = TypeBase*(std::string, TypeOffset);

		//using ConcreteType = ConcreteType;
		using GenericPolicy = ConcreteType;
		PolicyCtor* create_func = !is_generic ? _PolicyMaker<ConcreteType> : _PolicyMaker<GenericType>;




		std::string name;
		TypeOffset offset;

		auto LookUpOrMake = [&](std::string str, TypeOffset offset, bool lookup) -> TypeBase*
			{
				std::string_view name = std::string_view{ str };

				TypeBase* result = nullptr;

				if (lookup)
					result = IdentityManager::instance->GetBaseByOffset(name, offset);
				else
					result = is_generic ? static_cast<TypeBase*>(new GenericType{ name, offset }) : new ConcreteType{ name, offset };

				return result;
			};

		bool lookup = false;
		
		TypeBase* result;

		if (auto attach = ast.FindChild(parse_strings::settings)->FindChild(parse_strings::attach); attach)
		{
			SyntaxRecord& attach_data = attach->GetFront();
			switch (Hash(attach_data.GetTag()))
			{
			case "intrinsic"_h:
				//Look up
				lookup = true;
				__fallthrough;
			case "external"_h:
			{
				//Handle error, I can't fucking be bothered.
				SyntaxRecord& category = attach_data.GetFront();
				TypeOffset index;

				//this should more be if it's not number.
				if (auto& args = category.GetFront(); args.GetView() == "args")
				{
					auto& children = args.children();

					std::vector<std::string_view> string_args{ children.size() };

					std::transform(children.begin(), children.end(), string_args.begin(), [](SyntaxRecord& it) { return it.GetView(); });

					index = GetProject()->client()->GetOffsetFromArgs(category.GetView(), string_args.data(), string_args.size());
					//logger::trace("offset from args = {}", index);
				}
				else
				{
					index = RecordToInt(category.GetFront());
				}

				result = LookUpOrMake(category.GetTag(), index, lookup);

				break;

			}
			break;//create

			default:
				report::apply::debug("Couldn't ObtainPolicy");
				result = nullptr;
				break;
			}

		}
		else
		{
			result = is_generic ? new GenericPolicy{} : new ConcreteType{};
		}

		if (result)
			result->Initialize(ast);

		return result;
	}






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


	void Script::LoadFromSyntaxTree(SyntaxRecord::Iterator begin, SyntaxRecord::Iterator end)
	{


		//for (auto& node : target.children())
		while (begin != end)
		{
			auto& node = *begin++;

			//Turn this into a function
			get_switch(node.SYNTAX().type)
			{

			case SyntaxType::Project://No fucking idea why this is even here.
				break;

			case SyntaxType::Format: {
				Project* project = GetProject();
				if (project)
					project->AddFormat(node.GetFront().GetTag(), node.GetTag(), this);
			}
			break;

			case SyntaxType::Function:
			{
				//auto* function = new ConcreteFunction{};

				//AddFunction(function);

				//function->ConstructFromRecord(node);
				//AddFunction(Component::Create<ConcreteFunction>(node));

				CreateFunction(node);
				break;

			}
			case SyntaxType::Type:
			{
				//auto* policy = ObtainPolicy(node);

				//policy->ConstructFromRecord(node);

				//AddType(policy);

				AddType(tempObtainPolicy(node));
				break;
			}
			case SyntaxType::Variable:
			{
				//This is very incorrect btw
				AddVariable(Component::Create<ConcreteGlobal>(node));
				break;
			}
			//These 2 are script exclusives
			case SyntaxType::Directive:
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
				report::compile::critical("Syntax {} not valid for script", magic_enum::enum_name(switch_value)); break;
			}
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

	bool Script::AppendContent(std::string_view content, api::vector<std::string_view> options)
	{
		//In order for append to work, new objects need to  play catch up with already existing things. To do this, we can check if linkage already
		// finalized, and then finalize it again.

		//I also would need the ability to create scripts from strings, as I wouldn't want appending to be a thing for regular scripts just yet.

		//Ultimately, we just can't quite do it right, so we shouldn't bother right now. Append is important, but I'd rather like to get a make shift
		// set up going where you can just append to specific files.


		
		//Options is ignored for now. Basically does nothing. No compile time stuff either. No system for it.
		SyntaxRecord ast;

		if (Impl::Parser__::CreateSyntaxTree(ast, content, "") == false) {
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


		//Nothing to add, no reason to care.
		if (content.size() == 0)
			return true;
		//if (IsIncremental() == true)

		if (auto* tree = GetSyntaxTree(); tree) {
			auto& children = tree->children();
			
			auto index = children.size();
			//TODO: Right here, you'd want to rewrite every single syntax so it appends to the very end, instead of incorrectly being placed later.

			tree->EmplaceChildren(std::move(content.children()));
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
				LoadFromSyntaxTree(begin, end);
				RefreshLinkage();
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
		if (flags != LinkFlag::Loaded)
			return LinkResult::Failure;


		auto& children = GetSyntaxTree()->children();



		LoadFromSyntaxTree(children.begin(), children.end());

		return LinkResult::Success;
	}

	LinkFlag Script::GetLinkFlags()
	{
		return  LinkFlag::Loaded;
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