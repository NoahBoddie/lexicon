#pragma once

#include "Lexicon/Component.h"
#include "ExpressionType.h"
#include "Exception.h"
#include "Parser.h"
#include "Element.h"

#include "TypeID.h"
#include "FunctionInfo.h"
#include "VariableInfo.h"//For tests only

#include "SearchPredicate.h"

//*src
#include "ConcreteFunction.h"

#include "GlobalVariable.h"


namespace LEX
{
	using Syntax = Impl::Syntax;
	using SyntaxType = Impl::SyntaxType;

	class TypeInfo;

	class Variable;

	class Global;

	class PolicyBase;

	struct ConcretePolicy;
	struct AbstractTypePolicy;

	struct OverloadKey;

	//For now, this is just a fucking dummy class, as nothing uses it, but it's here to remind myself of it. For now.
	class CompilerMemory;
	

	struct QualifiedField;


	enum struct SearchResult
	{
		Continue,
		Skip,
		Force
	};
	
	struct [[deprecated("replaced by OverloadInput")]] SearchParams//Deprecated, 
	{
		std::string					name{};
		Environment*				scope = nullptr;
		ITypePolicy*				target = nullptr;		//Useless for type searches but literally, why make another type.
		std::vector<ITypePolicy*>	tempArgs{};
		std::vector<ITypePolicy*>	funcArgs{};	//For type search this is what's in the constructor. You can only have one or the other ideally though.

		
	};

	
	//TODO: TypeContainer needs to be be PolicyBase, not ITypePolicy. Excludes specializations that way.
	//For now only one per name. I'm not dealing with function and type signatures.
	//using FunctionContainer = std::vector<FunctionInfo>;
	//using TypeContainer = std::vector<ITypePolicy*>;
	using FunctionContainer = FunctionInfo;
	using TypeContainer = PolicyBase*;



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

		virtual void AddFunction(FunctionBase* tar);


		virtual void AddVariable(Global* tar);

		
		void AddType(PolicyBase* policy);
		
		//TODO: Issue with members on FindFunction and FindVariable. See below.
		// (I think that should catch members too) should both take an 
		// ITypePolicy. Main reason why is because of member and method requires being higher priority
		// and the exclusive place to check from when there's a parenthesis. Actually, this is a search thing,
		// not a find issue.
		virtual std::vector<FunctionInfo*> FindFunctions(std::string name);




		virtual Global* FindVariable(std::string name);

		virtual std::vector<ITypePolicy*> FindTypes(std::string name);

		//These return Environment instead of scripts because I really only need script from it. That, and it might allow me to use maybe scripts too. 
		// and there's no need for IScript to be used.
		//Scripts return its include and import, classes get their scripts import/includes.
		//<!> These are scripts aren't they?
		virtual std::vector<Environment*> GetIncluded();

		virtual std::vector<Environment*> GetImport();

		//TODO: I think I'll combine get import and include so I can not have a bunch of boilerplate
		void GetRecursiveIncluded(std::set<Environment*>& cache);

		std::vector<Environment*> GetRecursiveIncluded();



		//NOTE:the search for functions has to have a boolean to say if it explicitly is using a type target or not.

		//TODO: Split SearchField and SearchFunction. Functions will need to return the overload, while Fields don't really need overloads.
		// You however can search for a function by using a field.


		//TODO: SEVERE: Read below, big important.
		//If I can, I'd like to put this in element some how. I can have calls of GetIncluded target explicitly
		// the script that the given thing is in. For some, that'd be itself. For classes and functions, elsewhere.
		QualifiedField SearchField(std::string name, OverloadKey& key, FieldPredicate pred = nullptr);
		
		Overload SearchFunction(std::string name, OverloadKey& key, FieldPredicate pred = nullptr);


		ITypePolicy* SearchType(std::string name, OverloadKey& key, TypePredicate pred = nullptr);


		QualifiedField TEMPSearchField(std::string name);

		//These are temp functions that will exist until I can sort out how better to use the above
		ITypePolicy* TEMPSearchType(std::string name);


		//Here's the question, how do we prevent looping? I know what we do. FindFunctions only focuses on itself now. Instead, there's a function I can use to collect
		// Includes and Imports, and I'll just compile all of their functions. These import functions are not nested. From there, I'll just have a wrapper function handle the 
		// nested getting.




		Environment* GetEnvironment() override;

		//source file type shit
		Element* GetParent() override;

		void SetParent(Element* par) override;

	protected:
		//Actually not going to use this.
		//virtual QualifiedField QualifyField(InfoBase* field);



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
			
			//The idea here is the virtual methods go here. But within a type.
			//Should be a more map like structure though.
			//std::vector<IFunction*> methods;

	};

	
	class SecondaryEnvironment : public Environment
	{
		//Function and For classes basically, and any subclass that would derive from it.
		// Hm. Literally just TypePolicies need this. So move it?
		Record* _syntax = nullptr;
		
	public:


		//Literally just move this to class.
		


		Record* GetSyntaxTree() override;

		void SetSyntaxTree(Record& rec) final override;
	};	
}
