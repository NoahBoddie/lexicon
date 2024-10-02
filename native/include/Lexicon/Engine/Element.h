#pragma once

#include "Lexicon/Engine/Component.h"
#include "RGL/Impl/Record.h"
#include "Lexicon/Exception.h"//May move to src

#include "Lexicon/Interfaces/IElement.h"


#include "Lexicon/TypeAliases.h"
//*src
#include "Lexicon/Interfaces/IProject.h"
#include "Lexicon/Interfaces/IScript.h"
#include "Lexicon/Interfaces/IEnvironment.h"
#include "Lexicon/Engine/Overload.h"
#include "Lexicon/Engine/OverloadKey.h"
#include "Lexicon/Engine/QualifiedField.h"
#include "Lexicon/Interfaces/ProjectManager.h"
#include "Lexicon/RelateType.h"



namespace LEX
{
	struct Environment;
	class Project;
	class Script;
	class CommonScript;
	
	struct FunctionInfo;



	using ElementSearch = bool(std::vector<Environment*>&);

	

	struct Element : public Component, public IElement
	{
		enum Flag
		{
			None		= 0 << 0,
			Attached	= 1 << 0,  //When parent is set. If not attached, will not be set.

			_next		= 1 << 1
		};



	public:
		//This should take over
		virtual std::string_view GetName() const = 0;

		virtual Record* GetSyntaxTree() = 0;

	private://Maybe?
		virtual IScript* GetScriptI() override;

		virtual IProject* GetProjectI() override;

		virtual IElement* GetParentI() = 0;
		
		virtual IEnvironment* GetEnvironmentI() = 0;

		Element* Promote() override { return this; }

		const Element* Promote() const override { return this; }

	public:

		static Element* GetElementFromPath(Element* a_this, std::string_view path, ElementType elem);

		Element* GetElementFromPath(std::string_view path, ElementType elem) override 
		{ 
			return GetElementFromPath(this, path, elem);
		}

		Script* GetScript()
		{
			return GetScriptI()->TryPromote();
		}

		Project* GetProject()
		{
			return GetProjectI()->TryPromote();
		}

		Environment* GetEnvironment()
		{
			return GetEnvironmentI()->TryPromote();
		}

		Element* GetParent()
		{
			return GetParentI()->TryPromote();
		}

		
	public:

#pragma region NewSearch


		//the associated should maybe be a bool or just reject any other than include and import.

		static SyntaxRecord& GetPath(Record& path, std::optional<bool> right = std::nullopt);



		virtual std::vector<Environment*> GetAssociates(RelateType)
		{
			return {};
		}



		static Environment* GetEnvironmentTMP(Environment* a_this, Record* path, bool& search_scripts);

		static Environment* WalkEnvironmentPath(Environment* a_this, SyntaxRecord*& path, bool search_scripts = true);

		static Environment* SpecializeEnvironments(std::vector<Environment*>& generics);

		static std::vector<Environment*> GetEnvironments(Element* a_this, SyntaxRecord* step, RelateType a, std::set<Element*>& searched);


		static bool HandlePath(Element* focus, SyntaxRecord* rec, std::function<ElementSearch>& func, std::set<Element*>& searched, bool need_associate);




		

		//I think the reutnr of this function should probably be why it failed if anything.
		static bool SearchPathBase(Element* a_this, SyntaxRecord& rec, std::function<ElementSearch> func);




		static PolicyBase* SearchTypePath(Element* a_this, Record& _path);



		PolicyBase* SearchTypePath(Record& _path)
		{
			return SearchTypePath(this, _path);
		}



		static bool CheckOverload(OverloadKey& input, std::vector<FunctionInfo*> clauses, Overload& ret);


		//TODO: Make this take pointers to overload stuff. The idea being if no overload is provided it fails when trying 
		// to handle multiple different functions.
		static FunctionInfo* SearchFunctionPath(Element* a_this, Record& path, OverloadKey* key = nullptr, Overload* out = nullptr);

		static FunctionInfo* SearchFunctionPath(Element* a_this, Record& path, OverloadKey& key, Overload& out)
		{
			return SearchFunctionPath(a_this, path, &key, &out);
		}

		FunctionInfo* SearchFunctionPath(Record& path, OverloadKey& key, Overload& out)
		{
			return SearchFunctionPath(this, path, key, out);
		}

		static QualifiedField SearchFieldPath(Element* a_this, Record& path);

		QualifiedField SearchFieldPath(Record& path)
		{
			return SearchFieldPath(this, path);
		}


		static Script* SearchScriptPath(Element* a_this, Record& path);


		Script* SearchScriptPath(Record& path)
		{
			return SearchScriptPath(this, path);
		}

#pragma endregion





		//This will generally get parent script rather than parent environment. If it is a script, it will return script.
		

		//If it is a project, it will return project.
		

		//Basically, like get class, but can be suitable for both scripts and classes and such, and is better
		// suited to use SearchField/SearchEnvironment on than just script.
		//Make pure to search for non implementers.
		

		//The safe version of GetScript
		Script* FetchScript()
		{
			return this ? GetScript() : nullptr;
		}

		//The safe version of GetProject
		Project* FetchProject()
		{
			return this ? GetProject() : nullptr;
		}

		//The safe version of GetEvironment
		Environment* FetchEnvironment()
		{
			return this ? GetEnvironment() : nullptr;
		}

		//The safe version of GetParent
		Element* FetchParent()
		{
			return this ? GetParent() : nullptr;
		}




		//Querying the lot of these without them being the type that actually expects it should cause a compiling error.

		//returns true if it's project is a shared project.
		bool IsShared()
		{
			return GetProject() == GetShared();
		}

		static Project* GetShared();

		virtual Script* GetCommons();

		Script* FetchCommons()
		{
			return this ? GetCommons() : nullptr;
		}

		virtual void SetSyntaxTree(Record&) = 0;//This is to be made on the abstract classes.

		virtual void LoadFromRecord(Record& rec)
		{
			//should likely be a pure virtual, but holding off.
		}

		virtual void OnAttach()
		{
			//This is specifically when parentage is established.
		}

		void OnInit(Record& rec) final override
		{
			SetSyntaxTree(rec);
			//We get the node like this so the target is a viable syntax tree. Other wise, it may use a tree that doesn't exist anymore.
			auto& node = *GetSyntaxTree();
			
			LoadFromRecord(node);
			node.SetParent(this);//Doing this last allows the base most ones to set their parent first.
		}




		
	protected:
		
		virtual void SetParent(Element*) = 0;

		void NoAttached()
		{
			//This simply removes the attach check by confirming it's already attached.
			if (IsAttached() == false)
			{
				GetFlags() |= Flag::Attached;
				OnAttach();
			}
			
		}

		bool IsAttached()
		{
			return GetFlags() & Flag::Attached;
		}

		Flag& GetFlags()
		{
			return GetComponentData<Flag>();
		}

		void DeclareParentTo(Element* child)
		{
			child->SetParent(this);
			child->GetFlags() |= Flag::Attached;
			child->OnAttach();
			
		}

	public:
	};


	class SecondaryElement : public Element
	{
		//Function and Global
		//std::string name;//may include name later, depending.
		Record* _syntax = nullptr;
		Environment* _parent = nullptr;
	public:

		IEnvironment* GetEnvironmentI() override;


		IElement* GetParentI() override;


		Record* GetSyntaxTree() override
		{
			return _syntax;
		}

		void SetSyntaxTree(Record& rec) final override
		{
			if (!_syntax)
				_syntax = &rec;
		}

	protected:

		void SetParent(Element* par) override;



	};
}