#pragma once

//Temp
#include "Lexicon/TypeAliases.h"


#include "Lexicon/Interfaces/Environment.h"

//
#include "Lexicon/AbstractTypePolicy.h"
#include "Lexicon/AbstractFunction.h"

namespace LEX
{
	struct Variable_;
	struct ConcretePolicy;
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


		//Not needed,
		//bool _defined = false;

		Record _syntaxTree;

		//This is where scripts are refered
		std::unordered_map<Relationship, std::vector<Script*>> _relationMap2;
		std::unordered_map<Script*, Relationship> _relationMap;
	public:


		bool IsDefined() const;

		Script* GetCommons() override;


		Script* GetScript() override;


		ComponentType GetComponentType() override;

		Record* GetSyntaxTree() override;

		void SetSyntaxTree(Record& rec) final override;

		void LoadFromRecord(Record& ast) override;

		void OnAttach() override;

		std::string_view GetName() const override;

		//SetName will resume having no use here.

		void CompileExpression(Record& target);



		PolicyBase* tempObtainPolicy(Record& ast);



		//Includes/Imports/Requires need to be included as concepts.
		// Script is the only thing that uses these, so no reason to branch out.
		// Also, I may not need to "Find" them for that reason.
		// Instead, asking if something is one of these things might be better.
		
		std::vector<Environment*> GetIncluded() override;

		std::vector<Environment*> GetImport() override;


		Script* FindRelationship(std::string name, bool shared, Relationship bond);


		Relationship AddRelationship(Script*, Relationship bond);


		virtual bool IsCommons() const { return false; }
		void SetParent(Element* env) override;
	};

	
	struct CommonScript : public Script
	{
		CommonScript* GetCommons();

		bool IsCommons() const override { return true; }

		//The common ground of a project, mainly exists to override some functions in script.
		std::string_view GetName() const override;
	};


	namespace ExperimentalZone
	{

		struct CoreScript : public Script
		{
			//CORE holds all generated classes, these are classes that are generated not from script but from the project itself.
			// At the same time always reachable, at the same time completely unreferencable.

			std::string_view GetName() const override
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
			std::map<ConcretePolicy*, std::vector<Function*>> _methods;

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