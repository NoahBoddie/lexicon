#include "Lexicon/Impl/Script.h"
#include "Lexicon/Impl/Project.h"

//#include "Lexicon/Impl/TestField.h"

#include "Lexicon/Impl/IdentityManager.h"
#include "Lexicon/Impl/ConcretePolicy.h"
#include "Lexicon/Impl/ConcreteFunction.h"

#include "Lexicon/Impl/parse_strings.h"

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


	inline PolicyBase* ObtainPolicy(Record& ast)
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
				TypeOffset index = RecordToInt(category.GetFront());
				result = LookUpOrMake(category.GetTag(), index, lookup);
				logger::info("looked up {}", (int)result->FetchTypeID());
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



	Script* Script::GetScript()
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

		CompileExpression(ast);
	}


	std::string Script::GetName()
	{
		if (IsDefined() == false)
			//This is more than likely a fault actually.
			throw EnvironmentError("Syntax Tree not defined, script is nameless.");

		return _syntaxTree.GetTag();
	}
	//SetName will resume having no use here.

	
	void Script::CompileExpression(Record& target)
	{
		for (auto& node : target.GetChildren())
		{
			switch (node.SYNTAX().type)
			{
			case SyntaxType::Project:
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
				
				AddType(ObtainPolicy(node));
				break;
			}
			case SyntaxType::Variable:
			{
				//This is very incorrect btw
				AddVariable(Component::Create<Global>(node));
				break;
			}
			//These 2 are script exclusives
			case SyntaxType::Format:
			case SyntaxType::Directive:

			default:
				report::compile::critical("Syntax not valid for script"); break;
			}
		}
	}







	//Includes/Imports/Requires need to be included as concepts.
	// Script is the only thing that uses these, so no reason to branch out.
	// Also, I may not need to "Find" them for that reason.
	// Instead, asking if something is one of these things might be better.

	std::vector<Environment*> Script::GetIncluded()
	{
		return {};
	}

	std::vector<Environment*> Script::GetImport()
	{
		return {};
	}


	Script* Script::FindRelationship(std::string name, bool shared, Relationship bond)
	{
		//The shared is because between shared, 2 scripts can have the same name.
		if (bond == Relationship::None)
			return nullptr;

		return nullptr;
	}


	Script::Relationship Script::AddRelationship(Script*, Script::Relationship bond)
	{
		//Return the relationship it's been assigned or the relationship it has previously been assign if it
		// can't the relationship.
		return Relationship::None;
	}




	CommonScript* CommonScript::GetCommons()
	{
		return this;
	}

	//The common ground of a project, mainly exists to some functions in script.
	std::string CommonScript::GetName()
	{
		return "Commons";
	}


	//std::string CoreScript::GetName()
	//{
	//	return "CORE";
	//}



}