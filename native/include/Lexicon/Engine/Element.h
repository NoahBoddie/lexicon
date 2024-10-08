#pragma once

#include "Lexicon/Engine/Component.h"
#include "RGL/Impl/Record.h"
#include "Lexicon/Exception.h"//May move to src

#include "Lexicon/Engine/IElementImpl.h"


#include "Lexicon/TypeAliases.h"

#include "Lexicon/Engine/SyntaxRecord.h"


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

	

	struct Element : public Component, public IElementImpl
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

		virtual SyntaxRecord* GetSyntaxTree() = 0;

	
		Script* GetScript(bool = {}) override;

		Project* GetProject(bool = {}) override;

		Element* GetParent(bool = {}) = 0;
		
		Environment* GetEnvironment(bool = {}) = 0;

	public:

		static Element* GetElementFromPath(Element* a_this, std::string_view path, ElementType elem);

		Element* GetElementFromPath(std::string_view path, ElementType elem, bool = {}) override
		{ 
			return GetElementFromPath(this, path, elem);
		}

		
	public:

#pragma region NewSearch


		//the associated should maybe be a bool or just reject any other than include and import.

		static SyntaxRecord& GetPath(SyntaxRecord& path, std::optional<bool> right = std::nullopt);



		virtual std::vector<Environment*> GetAssociates(RelateType)
		{
			return {};
		}



		static Environment* GetEnvironmentTMP(Environment* a_this, SyntaxRecord* path, bool& search_scripts);

		static Environment* WalkEnvironmentPath(Environment* a_this, SyntaxRecord*& path, bool search_scripts = true);

		static Environment* SpecializeEnvironments(std::vector<Environment*>& generics);

		static std::vector<Environment*> GetEnvironments(Element* a_this, SyntaxRecord* step, RelateType a, std::set<Element*>& searched);


		static bool HandlePath(Element* focus, SyntaxRecord* rec, std::function<ElementSearch>& func, std::set<Element*>& searched, bool need_associate);




		

		//I think the reutnr of this function should probably be why it failed if anything.
		static bool SearchPathBase(Element* a_this, SyntaxRecord& rec, std::function<ElementSearch> func);




		static PolicyBase* SearchTypePath(Element* a_this, SyntaxRecord& _path);



		PolicyBase* SearchTypePath(SyntaxRecord& _path)
		{
			return SearchTypePath(this, _path);
		}



		static bool CheckOverload(OverloadKey& input, std::vector<FunctionInfo*> clauses, Overload& ret);


		//TODO: Make this take pointers to overload stuff. The idea being if no overload is provided it fails when trying 
		// to handle multiple different functions.
		static FunctionInfo* SearchFunctionPath(Element* a_this, SyntaxRecord& path, OverloadKey* key = nullptr, Overload* out = nullptr);

		static FunctionInfo* SearchFunctionPath(Element* a_this, SyntaxRecord& path, OverloadKey& key, Overload& out)
		{
			return SearchFunctionPath(a_this, path, &key, &out);
		}

		FunctionInfo* SearchFunctionPath(SyntaxRecord& path, OverloadKey& key, Overload& out)
		{
			return SearchFunctionPath(this, path, key, out);
		}

		static QualifiedField SearchFieldPath(Element* a_this, SyntaxRecord& path);

		QualifiedField SearchFieldPath(SyntaxRecord& path)
		{
			return SearchFieldPath(this, path);
		}


		static Script* SearchScriptPath(Element* a_this, SyntaxRecord& path);


		Script* SearchScriptPath(SyntaxRecord& path)
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

		Script* GetCommons(bool = {}) override;

		Script* FetchCommons()
		{
			return this ? GetCommons() : nullptr;
		}

		virtual void SetSyntaxTree(SyntaxRecord&) = 0;//This is to be made on the abstract classes.

		virtual void LoadFromRecord(SyntaxRecord& rec)
		{
			//should likely be a pure virtual, but holding off.
		}

		virtual void OnAttach()
		{
			//This is specifically when parentage is established.
		}

		void OnInit(SyntaxRecord& rec) final override
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
		SyntaxRecord* _syntax = nullptr;
		Environment* _parent = nullptr;
	public:

		Environment* GetEnvironment(bool = {}) override;


		Element* GetParent(bool = {}) override;


		SyntaxRecord* GetSyntaxTree() override
		{
			return _syntax;
		}

		void SetSyntaxTree(SyntaxRecord& rec) final override
		{
			if (!_syntax)
				_syntax = &rec.Transform<SyntaxRecord>();
		}

	protected:

		void SetParent(Element* par) override;



	};
}