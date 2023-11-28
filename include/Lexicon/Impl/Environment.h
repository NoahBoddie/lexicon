#pragma once

#include "Lexicon/Component.h"
#include "ExpressionType.h"
#include "Exception.h"
#include "Parser.h"
#include "Element.h"

#include "TypeID.h"
#include "VarInfo.h"
namespace LEX
{
	using Syntax = Impl::Syntax;
	using SyntaxType = Impl::SyntaxType;

	class TypeInfo;

	class Variable;

	struct AbstractTypePolicy;

	//For now, this is just a fucking dummy class, as nothing uses it, but it's here to remind myself of it. For now.
	class CompilerMemory;
	
	struct Environment : public Element
	{
		//virtual names and fancy names. Virtuals are the ones that are implemented by class, fancy names are embelished ones that call the virtual ones and are safe on nullptrs.
		// Add/Emplace
		// Find/Search
		// Get/Get
		// Set/Set


		//TODO:Add Get AccessModifier function. Operational by default, set to public. Likely, should derive from some pivot that can be shared with global and other components.



		//<!> All search functions may be a little far behind in concept, I need to sort out my thoughts on how to handle these.
		//This is used for being able to search scripts
		//virtual Environment* _SearchScript(bool is_shared, std::string name)
		//{
			//While you can search other projects too, the only viable excepted use will 
			//return nullptr;
		//}
		//There might be multiple environments, not so sure about this.
		std::set<Environment*> SearchEnvironment(Record& ast)
		{
			//BREAK THIS DOWN INTO PIECES. GetShared, GetScript, GetNext thing.

			//Can only percieve Keyword{shared}, script name.
			//The picked environment shouldn't need to be only one type, the type doesn't matter.
			// it's basically a scopedenvironment or a public environment. {Scripts, Projects (maybe, via keyword only, and only shared), classes, structs}
			// when it detects a scope that isn't a script, the loop should expect that to be the end.
			// to do this, I could search for an evironment type like the above, and continuously  looping through each type, catching environment errors. until I get my answer

			//What this doesn't do is get functions, at least not exactly.

			return {};
		}

		//These 2 will exist as sort of wrapper functions get function and co, basically to help print errors and such. Find exists when such errors are not required.
		//SearchFunction
		//SearchVariable

		//Search field isn't going to work with this, too many things it can't work with, so I need to be specific.
		// Actually, in though, all of the legwork is done through the actual functions, so I'mma prolly just use that instead.
		std::set<Element*> SearchField(std::string name, Impl::SyntaxType expr, Environment* spec_env = nullptr, TypePolicy* policy = nullptr)
		{
			//<!>NOTE: Should use ITypePolicy

			//mem_env, or member environment isn't quite correct. What it's supposed to carry is the 

			using SyntaxType = Impl::SyntaxType;


			Environment* search = spec_env ? spec_env : this;


			//If speci

			//What is it this should do? It should search an environment, either it's own, or a specified environment. In this, the member environment isn't entirely correct.
			// What it should be is something like a something that contains hierarchy information, however that isn't So I think there should be a function to transfer environments
			// into a a hierarchy.


			//For using find function, It should take a type policy. See, this allows us to specify the difference


			//perhaps this should take expression instead of expression type.
			//This could and should probably use 
			switch (expr)
			{
			case SyntaxType::Function:
				//return search->FindFunction(name, policy);
			case SyntaxType::VarDeclare:
				return {};//search->FindVariable(name);

			default:
				throw EnvironmentError("Syntax type not valid.");
			}


			return {};
		}

		//Environment* GetEnvironFromName(std::string name, Record* member, Record* )

		//Setters





	public:


		virtual void CompileExpression(Record& expression) = 0;


		virtual std::map<Signature, Function*> FindFunction(std::string name, AbstractTypePolicy* policy)
		{
			//Using a map is mostly on the convience of the chain of FindFunctions.
			// On the other hand, SearchField will only care about the results. By that point, it should
			// be unambiguous in the ways that matter.

			//Needs a way to differentiate between these

			// _GetInternalFunction
			// _GetExtneralFunction?
			// _GetStaticFunction
			// _GetExtensionFunction

			//Nevermind just search and report if one is using the wrong type.

			throw EnvironmentError("No implementation 'GetMember'");
		}

		virtual void AddFunction(Function*, AbstractTypePolicy*)
		{
			//MOVARI: ImplEnvironment 
			//Needs a way to differentiate between these

			// _GetInternalFunction
			// _GetExtneralFunction?
			// _GetStaticFunction
			// _GetExtensionFunction

			throw EnvironmentError("No implementation 'AddFunction'");
		}


		virtual Variable* FindVariable(std::string name)
		{
			//Move 

			//Finding a variable might take a runtime, this way, I can use the actual function runtime
			throw EnvironmentError("No implementation 'GetMember'");
		}
		virtual void AddVariable(Variable*)
		{
			//MOVARI: ImplEnvironment 
			throw EnvironmentError("No implementation 'AddVariable'");
		}

		

		//>Conditional
		
		//May not actually get abstract types, as you can't get specialized names from a name alone.
		//Also, may want an object to handle this, so I can accumulate possible ambiguities, but also have it handle failure due to ambiguity.
		virtual std::map<GenericSig, AbstractTypePolicy*> FindType(std::string name)
		{
			//MOVARI: Move script only. I MAY, let classes in classes happen, but you know. Simple for now.
			throw EnvironmentError("No implementation 'GetMember'");
		}

		virtual void AddType(AbstractTypePolicy*)
		{
			//Might be more like policy.

			//MOVARI: ImplEnvironment 
			throw EnvironmentError("No implementation 'GetMember'");
		}

		//Deprecated section

		//DEPRECATED, CREATE FINDMEMBERINFO in TypeInfo, and FINDPARAMETERINFO in Function.
		// ... and in scope... But the cross over there would be a pain. I might as well just have 2 seperate ones.
		InfoBase* FindVariableInfo(std::string name)
		{
			//This only needs to be used in classes and functions, IE things that can create members
			//And no need to create a pivot class, the demands of each are pretty different, so yeah, fuck it, just
			// make 2 different functions.
			throw EnvironmentError("No implementation 'FindVariableInfo'.");
		}


		void AddVariableInfo(InfoBase*)
		{
			//MOVARI: To a pivot between TypeInfo's and Functions
			throw EnvironmentError("No implementation 'AddVariableInfo'.");
		}

	};


	struct CanMakeClassEnviron : public Environment
	{
		//The common place for things that can create classes.
		virtual void CompileExpression(Record& target)
		{
			switch (target.SYNTAX().type)
			{
				//case ExpressionType::
			}
			//throw
		}
	};




	//Make independent
	enum struct FieldQuery
	{
		Success,
		Failure,
		Ambiguous,
	};



	
	


	struct SyntaxEnvironment : public Environment
	{
		Record* _syntaxNode;

		virtual void SetSyntaxTree(Record& ast)
		{
			throw EnvironmentError("No implementation of 'SetSyntaxTree'.");
		}
	};


	struct UnnamedEnvironment : public Environment
	{
		Record* _syntaxNode;

		std::string GetName() override
		{
			//Should this throw an exception isntead?
			return "";
		}
	};

	



	




	namespace ExperimentZone
	{


		struct Scope : public Environment
		{
			//A helper class that *might* be used by functions (but not subroutines) to organize what variables can and cannot be addressed during compile time.
			// Issue is, it's going to actually be a temporary object. And when I say temporary, I mean VERY temporary, kinda like how process chains work.
			//The representation of scope will then probably not be very relevant outside of compiling.

			//A nameless temporary environment used for the orchestration of function variables. Likely to be default created


			Function* _parent = nullptr;

		};




		//These are likely the names I'm going to choose when it comes to how the specializations work.
		// clients are partials, while servers are complete. Additionally, I don't think they'll for certain be a type, or function or global,
		// but I'll instead make functions to convert them into one of those. That moves the inheritence down the line, but it also prevents type functions from being
		// where routine functions would be.


			//The idea was that local types have their own typepolicy, this way I cna

			//The job of this is to inherit the focus class and include it as a specialization.
			// Basically makes it so I don't have to type out. But we'll see.
			template <typename T>
			struct SpecialClass__ {};

		
	}


}
