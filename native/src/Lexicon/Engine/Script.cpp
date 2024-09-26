#include "Lexicon/Engine/Script.h"
#include "Lexicon/Engine/Project.h"

//#include "Lexicon/Engine/TestField.h"

#include "Lexicon/Interfaces/IdentityManager.h"
#include "Lexicon/Engine/ConcretePolicy.h"
#include "Lexicon/Engine/ConcreteFunction.h"
#include "Lexicon/Engine/GlobalVariable.h"
#include "Lexicon/Engine/ConcreteGlobal.h"
#include "Lexicon/Engine/parse_strings.h"

#include "Lexicon/Engine/Expression.h"

#include "Lexicon/Interfaces/ProjectClient.h"

namespace LEX
{
	//TODO: ObtainPolicy needs to be moved else where at some point.

	template <typename T>
	PolicyBase* _PolicyMaker(std::string name, TypeOffset offset)
	{//helps with generic or concrete divide
		return new T{ std::string_view{name}, offset };
	}

	TypeOffset RecordToInt(Record& ast)
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





	inline PolicyBase* Script::tempObtainPolicy(Record& ast)
	{
		Record& settings = ast.GetChild(0);




		//This part can be done on the policy, what really needs to be done is figuring out which policy to create, 
		// or to create at all. This relies on 1 setting. The rest can be fed verbatum later.

		//NOTE, this also includes template arguments.


		//Rules of obtaining
		//Intrinsic, no creation, just pull a policy. Doesn't matter what else it is.
		//ISpecial-Creates type policy from specific string and integer. Link error if not found.
		//Data, Creates TypePolicy plain, claiming the next free space.
		//Generic, a different TypePolicy has to be used, but otherwise it's fine.


		Record* genericSet = settings.FindChild(parse_strings::generic);
		bool is_generic = genericSet && genericSet->size();


		using PolicyCtor = PolicyBase*(std::string, TypeOffset);

		//using ConcretePolicy = ConcretePolicy;
		using GenericPolicy = ConcretePolicy;
		PolicyCtor* create_func = !is_generic ? _PolicyMaker<ConcretePolicy> : _PolicyMaker<GenericPolicy>;




		std::string name;
		TypeOffset offset;

		auto LookUpOrMake = [&](std::string str, TypeOffset offset, bool lookup) -> PolicyBase*
			{
				std::string_view name = std::string_view{ str };

				PolicyBase* result = nullptr;

				if (lookup)
					result = IdentityManager::instance->GetBaseByOffset(name, offset);
				else
					result = is_generic ? new GenericPolicy{ name, offset } : new ConcretePolicy{ name, offset };

				return result;
			};

		bool lookup = false;
		
		PolicyBase* result;

		if (auto attach = ast.FindChild(parse_strings::settings)->FindChild(parse_strings::attach); attach)
		{
			Record& attach_data = attach->GetFront();
			switch (Hash(attach_data.GetTag()))
			{
			case "intrinsic"_h:
				//Look up
				lookup = true;
				__fallthrough;
			case "external"_h:
			{
				//Handle error, I can't fucking be bothered.
				Record& category = attach_data.GetFront();
				TypeOffset index;

				//this should more be if it's not number.
				if (auto& args = category.GetFront(); args.GetView() == "args")
				{
					auto& children = args.GetChildren();

					std::vector<std::string_view> string_args{ children.size() };

					std::transform(children.begin(), children.end(), string_args.begin(), [](Record& it) { return it.GetView(); });

					index = GetProject()->client()->GetOffsetFromArgs(category.GetView(), string_args.data(), string_args.size());
					logger::info("offset from args = {}", index);
				}
				else
				{
					index = RecordToInt(category.GetFront());
				}

				result = LookUpOrMake(category.GetTag(), index, lookup);
				logger::debug("looked up {}", (int)result->FetchTypeID());
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
			result = is_generic ? new GenericPolicy{} : new ConcretePolicy{};
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

	void Script::SetParent(Element* env)
	{
		//TODO: Script::SetParent is actually supposed to ask if new parent isn't a project. Attend to that when enum is added.
		//TODO: EnvironmentError in this situation is exclusively an error on my part, need a new exception for that.
		if (env->IsComponentType<Project>() == false)
			throw EnvironmentError("Parent of script must be a project.");

		_parent = env->GetProject();
	}


	bool Script::IsDefined() const
	{
		return _syntaxTree;
	}



	IScript* Script::GetScriptI()
	{
		return this;
	}


	ComponentType Script::GetComponentType()
	{
		return typeid(Script);
	}

	Record* Script::GetSyntaxTree()
	{
		if (IsDefined() == false)
			throw EnvironmentError("Syntax Tree not defined.");

		return &_syntaxTree;
	}

	void Script::SetSyntaxTree(Record& rec)
	{
		if (IsDefined() == true)
			return;
		//This ast likely has no interest, move resources.
		_syntaxTree = std::move(rec);
		
	}


	void Script::LoadFromRecord(Record& ast)
	{
		//if (IsInitialized() == true)
		//	throw EnvironmentError("No Syntax Tree already.");

		//Should chec
		//_defined = true;
		//_syntaxTree = ast;

		//CompileExpression(ast);
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


	LinkResult Script::OnLink(LinkFlag flags)
	{
		Record& target = *GetSyntaxTree();

		if (flags != LinkFlag::Loaded)
			return LinkResult::Failure;



		for (auto& node : target.GetChildren())
		{
			get_switch(node.SYNTAX().type)
			{

			case SyntaxType::Project://No fucking idea why this is even here.
				break;

			case SyntaxType::Format: {
				Project* project = GetProject();

				project->AddFormat(node.GetFront().GetTag(), node.GetTag(), this);
			}
		   break;

			case SyntaxType::Function:
			{
				//auto* function = new ConcreteFunction{};

				//AddFunction(function);

				//function->ConstructFromRecord(node);
				AddFunction(Component::Create<ConcreteFunction>(node));
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

				for (auto& directive : node.GetChildren()) {
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
		

		return LinkResult::Success;
	}

	LinkFlag Script::GetLinkFlags()
	{
		return  LinkFlag::Loaded;
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