#pragma once

#include "Lexicon/Component.h"

#include "Lexicon/Exception.h"

#include "Lexicon/Interfaces/Element.h"

#include "Lexicon/TypeID.h"
#include "Lexicon/MemberPointer.h"


#include "Lexicon/Interfaces/IEnvironment.h"


//*src
//#include "ConcreteFunction.h"//For some reason concrete function will not function if not included here. Mess with the order.
//#include "Element.h"
//#include "IFunction.h"
//#include "FunctionData.h"
//#include "OverloadClause.h"

//#include "FunctionBase.h"
//#include "AbstractFunction.h"


//#include "GlobalVariable.h"

namespace LEX::Impl
{
	enum struct SyntaxType;
	struct Syntax;
}

namespace LEX
{
	class TypeInfo;

	class Variable;

	struct Global;

	struct PolicyBase;
	class FunctionBase;

	struct ConcretePolicy;
	struct AbstractTypePolicy;

	struct Overload;
	struct OverloadKey;

	//For now, this is just a fucking dummy class, as nothing uses it, but it's here to remind myself of it. For now.
	class CompilerMemory;
	

	struct QualifiedField;
	struct ITypePolicy;
	struct IFunction;


	enum struct SearchResult
	{
		Continue,
		Skip,
		Force
	};


	enum struct Prefer
	{
		None,
		Type,
		Script,
		Project,
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

	struct FunctionInfo;
	struct VariableInfo;

	using FunctionContainer = FunctionInfo*;
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

			std::map<std::string, FunctionContainer> functionMap;

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
			std::map<std::string, TypeContainer> typeMap;
			std::vector<Global*> variables;//should be global variables



			//>------------------------
			//This is for classes.



			//This is what is used when searching 
			std::vector<VariableInfo> member;

			uint32_t _varIndex;//used to tell where the first index actually is, helps shift where exactly the variable is.

		};


	};



	struct IEnvironment_only : public Element
	{
		//This part of an environment can only find other things. I
		virtual std::vector<FunctionInfo*> FindFunctions(std::string name) = 0;



		//TODO: Change name to find field, and use a variableInfo for this.
		virtual VariableInfo* FindVariable(std::string name) = 0;

		virtual std::vector<PolicyBase*> FindTypes(std::string name) = 0;


	};



	struct Environment : public Element, public IEnvironment
	{

		//virtual names and fancy names. Virtuals are the ones that are implemented by class, fancy names are embelished ones that call the virtual ones and are safe on nullptrs.
		// Add/Emplace
		// Find/Search
		// Get/Fetch
		// Set/Set


		//TODO:Add Get AccessModifier function. Operational by default, set to public. Likely, should derive from some pivot that can be shared with global and other components.

	public:
		ITypePolicy* FindTypePath(std::string_view path) override
		{
			
			return nullptr;
		}
		IFunction* FindFunctionPath(std::string_view path) override
		{
			return nullptr;
		}

		//virtual IGlobal* FindGlobalPath(std::string_view path) overide { return nullptr; }

	public:
		~Environment() override;


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



		//TODO: Change name to find field, and use a variableInfo for this.
		virtual VariableInfo* FindVariable(std::string name);

		virtual std::vector<PolicyBase*> FindTypes(std::string name);

		//These return Environment instead of scripts because I really only need script from it. That, and it might allow me to use maybe scripts too. 
		// and there's no need for IScript to be used.
		//Scripts return its include and import, classes get their scripts import/includes.
		//<!> These are scripts aren't they?
		virtual std::vector<Environment*> GetIncluded();

		virtual std::vector<Environment*> GetImport();

		//TODO: I think I'll combine get import and include so I can not have a bunch of boilerplate
		void GetRecursiveIncluded(std::set<Environment*>& cache);

		std::vector<Environment*> GetRecursiveIncluded();





		//Here's the question, how do we prevent looping? I know what we do. FindFunctions only focuses on itself now. Instead, there's a function I can use to collect
		// Includes and Imports, and I'll just compile all of their functions. These import functions are not nested. From there, I'll just have a wrapper function handle the 
		// nested getting.


		//This is in Environment.cpp, needs to be moved out.
		bool CheckOverload(OverloadKey& input, std::vector<FunctionInfo*> clauses, Overload& ret);


		//All versions end up at these last 2 locations.
		PolicyBase* SearchTypeImpl(std::string name, OverloadKey* key);


		QualifiedField SearchFieldImpl(std::string name, OverloadKey* key);


		FunctionInfo* SearchFunctionImpl(std::string name, OverloadKey& key, Overload& out);





		FunctionInfo* SearchFunction(Record& path, OverloadKey& key, Overload& out);


		///These functions are supposed to resolve generic ambiguity.
		PolicyBase* SearchType(Record& path);


		QualifiedField SearchField(Record& path);





		Environment* SearchEnvironment(Record& path);





		Environment* SearchEnvironmentPath(Record*& _path);


		PolicyBase* SearchTypePath(Record& _path);

		FunctionInfo* SearchFunctionPath(Record& _path, OverloadKey& input, Overload& out);

		QualifiedField SearchFieldPath(Record& _path);




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

		std::map<std::string_view, FunctionContainer> functionMap;


		//>-------------------------
		//This is for environment
		std::map<std::string_view, TypeContainer> typeMap;
		//TODO: I need a class called policy base that will handle PolicyBases. Basically it'd be something like what IType is to AbstractType.
		// largely, it'd serve as a good wrapper for type aliases or other types such as generic arguments.

		
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

	static_assert(std::derived_from<Environment, Component>);
}
