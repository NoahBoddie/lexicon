#pragma once

#include "Lexicon/Engine/Component.h"

#include "Lexicon/Exception.h"

#include "Lexicon/Engine/Element.h"
#include "Lexicon/Engine/Directory.h"

#include "Lexicon/TypeID.h"
#include "Lexicon/MemberPointer.h"


#include "Lexicon/Interfaces/IEnvironmentBase.h"
#include "Lexicon/Engine/OverloadInfo.h"

#include "Lexicon/Impl/destroy_delete.h"



//#include "ConcreteFunction.h"//For some reason concrete function will not function if not included here. Mess with the order.
//#include "Element.h"
//#include "IFunction.h"
//#include "FunctionData.h"
//#include "OverloadParameter.h"

//#include "FunctionBase.h"
//#include "Function.h"


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

	struct Overload;
	struct OverloadArgument;

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

	
	

	struct OverloadInfo;
	struct FunctionInfo;
	struct VariableInfo;

	//change to unique pointer please
	


	//I may not need to use IFunction for these because generic functions will carry around members, as they cannot 
	// look exactly the same.





	struct Environment : public Directory, public IEnvironmentBase
	{
	public:
		DEFINE_COMPONENT_OFFSET(ComponentType::Environment)

		//virtual names and fancy names. Virtuals are the ones that are implemented by class, fancy names are embelished ones that call the virtual ones and are safe on nullptrs.
		// Add/Emplace
		// Find/Search
		// Get/Fetch
		// Set/Set


		//TODO:Add Get AccessModifier function. Operational by default, set to public. Likely, should derive from some pivot that can be shared with global and other components.

	public:
		

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
		virtual std::vector<OverloadInfo*> FindFunctions(const std::string_view& name);



		//TODO: Change name to find field, and use a variableInfo for this.
		virtual std::vector<VarInfo*> FindVariables(const std::string_view& name);

		virtual std::vector<TypeBase*> FindTypes(std::string_view name);





		//Here's the question, how do we prevent looping? I know what we do. FindFunctions only focuses on itself now. Instead, there's a function I can use to collect
		// Includes and Imports, and I'll just compile all of their functions. These import functions are not nested. From there, I'll just have a wrapper function handle the 
		// nested getting.





		void CreateFunction(SyntaxRecord& node);



		Directory* FindDirectory(SyntaxRecord& record, ITemplateInserter* inserter) override;


	protected:
		//Actually not going to use this.
		//virtual QualifiedField QualifyField(InfoBase* field);



		inline TypeBase* tempObtainPolicy(SyntaxRecord& ast, Element* parent = nullptr);

		virtual void LoadFromSyntaxNode(SyntaxRecord& node);

		void LoadFromSyntaxTree(SyntaxRecord::Iterator begin, SyntaxRecord::Iterator end);


		LinkResult OnLink(LinkFlag flags) override
		{

			if (flags != LinkFlag::Loaded)
				return LinkResult::Success;


			auto body = GetSyntaxTree()->FindChild(parse_strings::body);

			if (!body) {
				return LinkResult::Success;
			}

			auto& children = body->children();

			LoadFromSyntaxTree(children.begin(), children.end());

			return LinkResult::Success;
		}

		LinkFlag GetLinkFlags() override
		{
			return LinkFlag::Loaded;
		}



	private:

		Environment* GetEnvironmentImpl() override;

		//source file type shit
		Directory* GetParentImpl() override;

		void SetParent(Directory* par) override;



	protected: //Some might be private, will address later.
		Directory* _parent = nullptr;//can be project or script/class

		std::map<std::string_view, std::vector<destructible_ptr<OverloadInfo>>> functions;


		//>-------------------------
		//This is for environment
		//Later this will use type infos
		//I'd rather store this by type info btw. The actual type info.
		std::map<std::string_view, TypeBase*> typeMap;


		
		//std::vector<GlobalBase*> variables;//should be global variables
		std::map<std::string_view, destructible_ptr<DestructibleVarInfo>> variables;

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
