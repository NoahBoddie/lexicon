#pragma once

#include "Lexicon/Component.h"
#include "ExpressionType.h"
#include "Exception.h"
#include "Parser.h"
#include "Element.h"

#include "TypeID.h"
#include "VarInfo.h"

//*src
#include "Function.h"
#include "GlobalVariable.h"

namespace LEX
{
	using Syntax = Impl::Syntax;
	using SyntaxType = Impl::SyntaxType;

	class TypeInfo;

	class Variable;

	class Global;


	struct TypePolicy;
	struct AbstractTypePolicy;

	//For now, this is just a fucking dummy class, as nothing uses it, but it's here to remind myself of it. For now.
	class CompilerMemory;
	




	enum struct SearchResult
	{
		Continue,
		Skip,
		Force
	};


	struct SearchParams
	{
		std::string					name{};
		Environment*				scope = nullptr;
		ITypePolicy*				target = nullptr;		//Useless for type searches but literally, why make another type.
		std::vector<ITypePolicy*>	tempArgs{};
		std::vector<ITypePolicy*>	funcArgs{};	//For type search this is what's in the constructor. You can only have one or the other ideally though.

		
	};

	//Will expand later.
	using FieldSearch = std::function<SearchResult(SearchParams&, Field*)>;

	using TypeSearch = std::function<SearchResult(SearchParams&, ITypePolicy*)>;
	 
	//TODO: TypeContainer needs to be be PolicyBase, not ITypePolicy. Excludes specializations that way.
	//For now only one per name. I'm not dealing with function and type signatures.
	//using FunctionContainer = std::vector<FunctionInfo>;
	//using TypeContainer = std::vector<ITypePolicy*>;
	using FunctionContainer = FunctionInfo;
	using TypeContainer = ITypePolicy*;



	//I may not need to use IFunction for these because generic functions will carry around members, as they cannot 
	// look exactly the same.


	struct DataTypeMembersAndMethods
	{

		struct Method
		{
			//This is a type that you'd only ever deal with 
			MemberPointer memberID;
			IFunction* function;
		};

		struct TestFunctionHolder
		{
			//>-------------------------
			//This is for environment

			std::map<String, FunctionContainer> functionMap;

			//>------------------------
			//This is for classes.

			//This is basically exclusively used for compiled code, 
			// also, if the memberpointer both matches the type and specialization of the policy inspected, 
			std::vector<Method> methods;

			size_t overrideIndex;//This is the index to start at if you're trying to search for an overriden function.

		};

		struct TestVariableHolder
		{
			//>-------------------------
			//This is for environment
			std::map<String, TypeContainer> typeMap;
			std::vector<Global*> variables;//should be global variables



			//>------------------------
			//This is for classes.



			//This is what is used when searching 
			std::vector<VariableInfo> member;

			uint32_t _varIndex;//used to tell where the first index actually is, helps shift where exactly the variable is.

		};


	};





	struct Environment : public Element
	{
		//virtual names and fancy names. Virtuals are the ones that are implemented by class, fancy names are embelished ones that call the virtual ones and are safe on nullptrs.
		// Add/Emplace
		// Find/Search
		// Get/Fetch
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





	public:

		//Move this down.
		virtual void CompileExpression(Record& expression) = 0;


		//>------------------------------------------------------------------------------------------------------------
		//AddVariable- doesn't need to be exposed in environment, no variable info could be added here.
		//AddFunction- pivotal, would likely need to be virtual, but also can be implemented.
		//AddType- likely wouldn't need to be virtual.

		//Finding functions, no need for sorting, just front load the requirements. That is what search is for, finding is for local (and it's includes and such).
		//FindFunctions -> FunctionInfo*[]
		//FindVariables -> Global*[]
		//FindTypes -> ITypePolicy*[] (2 versions, one for string, other for records, which will sift through classes and scripts both).
		//>Not doing yet
		//FindMembers ->VariableInfo?*[]

		virtual void AddFunction(FunctionBase* tar)
		{
			if (!tar) {
				RGL_LOG(critical, "Non - FunctionData IFunction attempted to be added");
				throw nullptr;
			}

			auto end = functionMap.end();

			auto name = tar->GetName();

			if (auto it = functionMap.find(name); end != it) {
				RGL_LOG(critical, "Non - FunctionData IFunction attempted to be added");
				throw nullptr;
			}
			else {
				functionMap[name].function = tar;
				DeclareParentTo(tar);
			}

		}


		virtual void AddVariable(Global* tar)
		{
			//This won't have an issue yet, because no abstraction.
			
			auto end = variables.end();

			auto name = tar->GetName();

			if (auto it = std::find_if(variables.begin(), end, [&](auto i) {return name == i->GetName(); }); end != it) {
				RGL_LOG(critical, "Variable already existed");
				throw nullptr;
			}
			else {
				variables.emplace_back(tar);
				DeclareParentTo(tar);
			}
		}

		virtual void AddType(ITypePolicy*)
		{

			RGL_LOG(critical, "AddType no abstraction");
			throw nullptr;
		}

		virtual std::vector<FunctionInfo*> FindFunctions(std::string name)
		{
			auto end = functionMap.end();

			if (auto it = functionMap.find(name); end != it){
				return { &it->second };
			}
			else{
				return {};
			}
		}




		virtual Global* FindVariable(std::string name)
		{
			auto end = variables.end();

			if (auto it = std::find_if(variables.begin(), end, [&](auto i) {return name == i->GetName(); }); end != it) {
				return *it;
			}
			else {
				return nullptr;
			}
		}

		virtual std::vector<ITypePolicy*> FindTypes(std::string name)
		{
			auto end = typeMap.end();

			if (auto it = typeMap.find(name); end != it) {
				return { it->second };
			}
			else {
				return {};
			}
		}

		//These return Environment instead of scripts because I really only need script from it. That, and it might allow me to use maybe scripts too. 
		// and there's no need for IScript to be used.
		//Scripts return its include and import, classes get their scripts import/includes.
		//<!> These are scripts aren't they?
		virtual std::vector<Environment*> GetIncluded() { return {}; }

		virtual std::vector<Environment*> GetImport() { return {}; }

		//TODO: I think I'll combine get import and include so I can not have a bunch of boilerplate
		void GetRecursiveIncluded(std::set<Environment*>& cache)
		{
			if (!this || !cache.emplace(this).second)
				return;

			std::vector<Environment*> result = GetIncluded();

			cache.insert_range(result);

			for (auto environment : result){
				environment->GetRecursiveIncluded(cache);
			}
			

		}

		std::vector<Environment*> GetRecursiveIncluded()
		{
			std::set<Environment*> cache;

			GetRecursiveIncluded(cache);

			return { cache.begin(), cache.end() };

		}

		//If I can, I'd like to put this in element some how.
		Field* SearchField(std::string name, Environment* scope, ITypePolicy* target, std::vector<ITypePolicy*>* temp_args, std::vector<ITypePolicy*>* func_args, FieldSearch func)
		{
			SearchParams params;
			
			params.name = name;
			params.scope = scope;
			params.target = target;
			//This is stupid, I don't need this.
			if (temp_args) params.tempArgs = *temp_args;
			if (func_args) params.funcArgs = *func_args;

			std::vector<Environment*> query;

			//Submitting includes also includes self. If self does not exist, it will cease to continue.
			query.insert_range(query.end(), GetRecursiveIncluded());

			for (auto env : query)
			{
				std::vector<Field*> result;

				std::vector<FunctionInfo*> functions = env->FindFunctions(name);
				
				//There's no situation where multiple can be observed, so it only needs the one.
				Global* global = env->FindVariable(name);

				auto size = functions.size();

				//TODO: VERY temporary idea. No pattern matching, no checking. This is basically the same that we did before
				if (size > 1 || size && global){
					RGL_LOG(critical, "mulitple fields of same name detected.");
					throw nullptr;
				}
				else if (global)
				{
					return global;
				}
				else if (size)
				{
					//IFunction is not a field
					return functions[0];
				}

			}

			//Environment* commons = 

			//query = 

			return nullptr;
		}
		

		ITypePolicy* SearchType(std::string name, Environment* scope, std::vector<ITypePolicy*>* temp_args, std::vector<ITypePolicy*>* func_args, TypeSearch func)
		{
			//One cannot get a type
			//Returns an ITypePolicy because the template args can still be


			SearchParams params;

			params.name = name;
			params.scope = scope;
			
			//This is stupid, I don't need this.
			if (temp_args) params.tempArgs = *temp_args;
			if (func_args) params.funcArgs = *func_args;

			std::vector<Environment*> query;

			//Submitting includes also includes self. If self does not exist, it will cease to continue.
			query.insert_range(query.end(), GetRecursiveIncluded());

			for (auto env : query)
			{
				std::vector<Field*> result;

				std::vector<ITypePolicy*> types = env->FindTypes(name);

				//There's no situation where multiple can be observed, so it only needs the one.

				auto size = types.size();

				//TODO: VERY temporary idea. No pattern matching, no checking. This is basically the same that we did before
				if (size > 1) {
					RGL_LOG(critical, "mulitple types of same name detected.");
					throw nullptr;
				}
				else if (size)
				{
					//IFunction is not a field
					return types[0];
				}

			}
			return nullptr;
		}


		//Here's the question, how do we prevent looping? I know what we do. FindFunctions only focuses on itself now. Instead, there's a function I can use to collect
		// Includes and Imports, and I'll just compile all of their functions. These import functions are not nested. From there, I'll just have a wrapper function handle the 
		// nested getting.




		Environment* GetEnvironment() override
		{
			return this;
		}

		//source file type shit
		Element* GetParent() override
		{
			return _parent;
		}

		void SetParent(Element* par) override;


		protected: //Some might be private, will address later.
			Element* _parent = nullptr;//can be project or script/class

		//private:
			//>-------------------------
			//This is for environment

			std::map<String, FunctionContainer> functionMap;


			//>-------------------------
			//This is for environment
			std::map<String, TypeContainer> typeMap;
			std::vector<Global*> variables;//should be global variables


	};

	
	class SecondaryEnvironment : public Environment
	{
		//Function and For classes basically, and any subclass that would derive from it.
		// Hm. Literally just TypePolicies need this. So move it?
		Record* _syntax = nullptr;
		
	public:


		//Literally just move this to class.
		


		Record* GetSyntaxTree() override
		{
			return _syntax;
		}
	};	
}
