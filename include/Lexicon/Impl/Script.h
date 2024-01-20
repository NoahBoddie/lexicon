#pragma once

//Temp
#include "Lexicon/Impl/TypeAliases.h"


#include "Lexicon/Impl/Environment.h"

//
#include "AbstractTypePolicy.h"
#include "AbstractFunction.h"

namespace LEX
{
	struct Variable_;
	struct TypePolicy;
	struct PolicyBase;


	class Script : public Environment
	{
	public:

		//Scripts have functions/globals(vars)/types/(parents/projects)
	private:
		enum struct Relationship
		{
			None,
			Required,
			Imported,
			Included,
		};


		Project* _parent = nullptr;

		//Not needed,
		//bool _defined = false;

		Record _syntaxTree;

		//This is where scripts are refered
		std::unordered_map<Relationship, std::vector<Script*>> _relationMap2;
		std::unordered_map<Script*, Relationship> _relationMap;
	public:


		bool IsDefined() const
		{
			return _syntaxTree;
		}

		Script* GetCommons() override;


		Script* GetScript() override
		{
			return this;
		}


		ComponentType GetComponentType() override
		{
			return typeid(Script);
		}

		Record* GetSyntaxTree() override
		{
			if (IsDefined() == false)
				throw EnvironmentError("Syntax Tree not defined.");

			return &_syntaxTree;
		}


		void LoadFromRecord(Record& ast) override
		{
			if (IsDefined() == true)
				throw EnvironmentError("No Syntax Tree already.");

			//Should chec
			//_defined = true;
			_syntaxTree = ast;
		}


		std::string GetName() override
		{
			if (IsDefined() == false)
				//This is more than likely a fault actually.
				throw EnvironmentError("Syntax Tree not defined, script is nameless.");

			return _syntaxTree.GetTag();
		}
		//SetName will resume having no use here.

		virtual void CompileExpression(Record& target)
		{

			for (auto& node : target.GetChildren())
			{
				switch (node.SYNTAX().type)
				{
				case SyntaxType::Function:
				{
					AddFunction(Component::Create<Function>(node));
					return;

				}
				case SyntaxType::VarDeclare:
				{
					AddVariable(Component::Create<Global>(node));
					return;
				}
					//These 2 are script exclusives
				case SyntaxType::Format:
				case SyntaxType::Directive:
				
				default:
					RGL_LOG(critical, "Syntax not valid for script");
					throw nullptr;
				}
			}
		}







		//Includes/Imports/Requires need to be included as concepts.
		// Script is the only thing that uses these, so no reason to branch out.
		// Also, I may not need to "Find" them for that reason.
		// Instead, asking if something is one of these things might be better.
		
		std::vector<Environment*> GetIncluded() override
		{ 
			return {}; 
		}

		std::vector<Environment*> GetImport() override
		{ 
			return {}; 
		}


		Script* FindRelationship(std::string name, bool shared, Relationship bond)
		{
			//The shared is because between shared, 2 scripts can have the same name.
			if (bond == Relationship::None)
				return nullptr;

			return nullptr;
		}


		Relationship AddRelationship(Script*, Relationship bond)
		{
			//Return the relationship it's been assigned or the relationship it has previously been assign if it
			// can't override the relationship.
			return Relationship::None;
		}



		void SetParent(Element* env) override;
	};

	
	struct CommonScript : public Script
	{
		CommonScript* GetCommons()
		{
			return this;
		}

		//The common ground of a project, mainly exists to override some functions in script.
		std::string GetName() override
		{
			return "Commons";
		}
	};


	namespace ExperimentalZone
	{

		struct CoreScript : public Script
		{
			//CORE holds all generated classes, these are classes that are generated not from script but from the project itself.
			// At the same time always reachable, at the same time completely unreferencable.

			std::string GetName() override
			{
				return "CORE";
			}
		};

		//Basicallly 
		struct MaybeScript : public Script
		{
			//Notes if it was specified like Shared::GeneralUtil::GetObjectByString(), it would be putting it into shared.
			bool _isShared = false;

			std::string _name;
			Script* _parent = nullptr;
			Script* _mirror = nullptr;



			//So notably for this, functions and extend methods exist obvs, but I think having globals is actually a really interesting thing I can do.
			// the main reason is you can have this as an interface between 2 different scripts to tell if it's installed by having a maybe global set up.
			// maybe globals should come in pairs.

			//Also, maybe some core pivot with script, since they share so much
			std::vector<Function*> _functions;
			std::vector<Variable_*> _globals;
			std::map<TypePolicy*, std::vector<Function*>> _methods;

			//When using, it's very important to treat this like an imported script.


			//Just about all functions are mirrored, but the only things that aren't really mirrored are are things like get script and get parent
			// 
			//Script* GetScript() override
			//{
			//	return _parent;
			//}



		};

		namespace ExperimentZone
		{

			struct InvalidScript : public Script
			{
				//A concept for searching an environment when it's no longer flagged as valid. It's basically created when something goes critical, and it can no longer be properly
				// referenced. Though, this is for top level stuff like scripts rather than for things like functions and such.

				//This can mainly exist as something that's created to communicate aptly and naturally when it comes around to it the failures of such a script.
				// such scripts are caried on the actual script, but should be submitted in the places scripts can, the same as maybescripts.

				//For this reason, it's highly likely I'll make a joint abstraction for them all to share, so GetScript can return this error object.


				//Also, unlike maybe this will likely mirror Script a lot closer, though mainly should have all of it's functions send descriptive exceptions about why this is an
				// invalid script.


				//REVISION
				//Actual, scratch that. This should be somewhere on the environment, but I think instead I'll accumulate it within the environment some how. For now, I won't worry about it,
				// but in concept the idea should just be that it's a temporary store of data that when something is being gotten and it's wondering why it's not there (because if it has an
				// error I'll be deleting it) it will print an error message, possibly with the invalidity object attached, so a new invalidity message referencing this invalidity message can
				// be used.fs
			};
		}
	}
}