#pragma once

#include "Lexicon/Engine/Component.h"

#include "Lexicon/Exception.h"

#include "Lexicon/Engine/Element.h"

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


namespace LEX
{
	class TypeInfo;

	class Variable;

	struct Global;
	struct GlobalBase;

	struct TypeBase;
	class FunctionBase;

	struct ConcreteType;
	struct Type;

	struct Overload;
	struct OverloadKey;

	//For now, this is just a fucking dummy class, as nothing uses it, but it's here to remind myself of it. For now.
	class CompilerMemory;
	

	struct QualifiedField;
	struct ITypeInfo;
	struct IFunction;


	enum struct SyntaxType : uint16_t;
	struct Syntax;


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
		ITypeInfo*				target = nullptr;		//Useless for type searches but literally, why make another type.
		std::vector<ITypeInfo*>	tempArgs{};
		std::vector<ITypeInfo*>	funcArgs{};	//For type search this is what's in the constructor. You can only have one or the other ideally though.

		
	};

	


	struct FunctionInfo;
	struct VariableInfo;

	//change to unique pointer please
	using FunctionContainer = std::vector<FunctionInfo*>;
	using TypeContainer = TypeBase*;



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
			std::vector<GlobalBase*> variables;//should be global variables



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
		virtual std::vector<FunctionInfo*> FindFunctions(std::string_view name) = 0;



		//TODO: Change name to find field, and use a variableInfo for this.
		virtual VariableInfo* FindVariable(std::string_view name) = 0;

		virtual std::vector<TypeBase*> FindTypes(std::string_view name) = 0;


	};



	struct Environment : public Element, public IEnvironment
	{
	public:


		//virtual names and fancy names. Virtuals are the ones that are implemented by class, fancy names are embelished ones that call the virtual ones and are safe on nullptrs.
		// Add/Emplace
		// Find/Search
		// Get/Fetch
		// Set/Set


		//TODO:Add Get AccessModifier function. Operational by default, set to public. Likely, should derive from some pivot that can be shared with global and other components.

	public:
		ITypeInfo* FindTypePath(std::string_view path) override
		{
			
			return nullptr;
		}
		IFunction* FindFunctionPath(std::string_view path) override
		{
			return nullptr;
		}

		void* Cast(std::string_view name) override
		{
			switch (Hash(name))
			{
			case Hash(TypeName<IEnvironment>::value):
				return (IEnvironment*)this;
			case Hash(TypeName<Environment>::value):
				return this;
			}
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
		//FindTypes -> ITypeInfo*[] (2 versions, one for string, other for records, which will sift through classes and scripts both).
		//>Not doing yet
		//FindMembers ->VariableInfo?*[]

		virtual void AddFunction(FunctionBase* tar);


		virtual void AddVariable(GlobalBase* tar);

		
		void AddType(TypeBase* policy);
		
		//TODO: Issue with members on FindFunction and FindVariable. See below.
		// (I think that should catch members too) should both take an 
		// ITypeInfo. Main reason why is because of member and method requires being higher priority
		// and the exclusive place to check from when there's a parenthesis. Actually, this is a search thing,
		// not a find issue.
		virtual std::vector<FunctionInfo*> FindFunctions(std::string_view name);



		//TODO: Change name to find field, and use a variableInfo for this.
		virtual GlobalBase* FindVariable(std::string_view name);

		virtual std::vector<TypeBase*> FindTypes(std::string_view name);





		//Here's the question, how do we prevent looping? I know what we do. FindFunctions only focuses on itself now. Instead, there's a function I can use to collect
		// Includes and Imports, and I'll just compile all of their functions. These import functions are not nested. From there, I'll just have a wrapper function handle the 
		// nested getting.


		//This is in Environment.cpp, needs to be moved out.
		bool CheckOverload(OverloadKey& input, std::vector<FunctionInfo*> clauses, Overload& ret);




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
		//TODO: I need a class called policy base that will handle TypeBases. Basically it'd be something like what ITypeInfo is to ITypeInfo.
		// largely, it'd serve as a good wrapper for type aliases or other types such as generic arguments.

		
		std::vector<GlobalBase*> variables;//should be global variables
			


		//The idea here is the virtual methods go here. But within a type.
		//Should be a more map like structure though.
		//std::vector<IFunction*> methods;

	};

	
	class SecondaryEnvironment : public Environment
	{
		//Function and For classes basically, and any subclass that would derive from it.
		// Hm. Literally just TypePolicies need this. So move it?
		SyntaxRecord* _syntax = nullptr;
		
	public:


		//Literally just move this to class.
		


		SyntaxRecord* GetSyntaxTree() override;

		void SetSyntaxTree(SyntaxRecord& rec) final override;
	};

	static_assert(std::derived_from<Environment, Component>);
}
