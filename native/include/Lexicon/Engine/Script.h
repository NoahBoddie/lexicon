#pragma once

#include "Lexicon/RelateType.h"

//Temp
#include "Lexicon/TypeAliases.h"


#include "Lexicon/Engine/Environment.h"
#include "Lexicon/Engine/IScriptImpl.h"
//
#include "Lexicon/AbstractTypePolicy.h"
#include "Lexicon/AbstractFunction.h"




namespace LEX
{
	struct Variable_;
	struct ConcretePolicy;
	struct PolicyBase;


	class Script : public Environment, public IScriptImpl
	{
	public:

		//Scripts have functions/globals(vars)/types/(parents/projects)
	public:
		
	private:

		//Not needed,
		//bool _defined = false;
		
		std::filesystem::path _filePath;

		SyntaxRecord _syntaxTree;

		//This is where scripts are refered
		std::unordered_map<RelateType, std::vector<Script*>> _relationMap;
	public:


		bool IsDefined() const;

		Script* GetCommons(bool = {}) override;


		Script* GetScript(bool = {}) override;


		IElement* base() override { return this; }
		const IElement* base() const override { return this; }


		void* Cast(std::string_view name) override
		{
			switch (Hash(name))
			{
			case Hash(TypeName<IScript>::value):
				return (IScript*)this;
			case Hash(TypeName<Script>::value):
				return this;
			}
			return nullptr;
		}


		ComponentType GetComponentType() override;

		SyntaxRecord* GetSyntaxTree() override;

		void SetSyntaxTree(SyntaxRecord& rec) final override;

		void LoadFromRecord(SyntaxRecord& ast) override;

		void OnAttach() override;

		std::string_view GetName() const override;

		//SetName will resume having no use here.

		void CompileExpression(Record& target);



		LinkResult OnLink(LinkFlag flags);

		LinkFlag GetLinkFlags();




		PolicyBase* tempObtainPolicy(SyntaxRecord& ast);



		//Includes/Imports/Requires need to be included as concepts.
		// Script is the only thing that uses these, so no reason to branch out.
		// Also, I may not need to "Find" them for that reason.
		// Instead, asking if something is one of these things might be better.
		

		Script* FindRelationship(std::string name, bool shared, RelateType bond);


		std::vector<Script*> GetRelationships(RelateType bond)
		{
			
		}

		std::vector<Environment*> GetAssociates(RelateType bond)
		{
			auto it = _relationMap.find(bond);

			if (_relationMap.end() != it) {
				auto& list = it->second;
				return { list.begin(), list.end() };
			}
				

			return {};
		}

		void AddRelationship(Script*, RelateType bond);


		virtual bool IsCommons() const { return false; }
		void SetParent(Element* env) override;
	};

	
	struct CommonScript : public Script
	{
		CommonScript* GetCommons(bool);

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