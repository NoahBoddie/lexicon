#pragma once

#include "Lexicon/Component.h"
#include "RGL/Impl/Record.h"
#include "Exception.h"//May move to src

namespace LEX
{
	struct Environment;
	class Project;
	class Script;
	class CommonScript;
	
	namespace {

		//GenericSig is like the below, and carries the ISpecializable or something like that? The idea is you can set 2 classes apart by the differences
		// in their generic parameters (note, the very default counts as a child of any). So the resolution should be handled similarly to the below.
		//using GenericSig = int;

		//Signature is the thing that is used to handle functions being ambiguious but different in their parameters
		// 2 signatures are counted as equals if they are the same, the parameters of one can qualify as the parameters of another
		// ie void name(float, float, float) vs void name(int, int, int) will be ambiguious to 
		//Note though, a function signature is an actual type, this is just a container to help the comparison.
		//Also remember function signatures are generic and one is created each time a function with a different arg count is created.

		//Also note, I'd very much so rather use a set for this.
		//using Signature = int;
		//FuncSig
	}

	ENUM(ElementFlag)
	{
		None		= 0 << 0,
		Attached	= 1 << 0,	//When parent is set. If not attached, will not be set.

	};

	struct Element : public Component
	{
		struct Data
		{
			ElementFlag flags;
		};

		//TODO: For all of these, there should be an "errorless" version, that just wraps the function and returns some result enum or something.


		//This will generally get parent script rather than parent environment. If it is a script, it will return script.
		virtual Script* GetScript()
		{
			return GetParent()->FetchScript();
		}

		//If it is a project, it will return project.
		virtual Project* GetProject()
		{
			return GetParent()->FetchProject();
		}

		//Basically, like get class, but can be suitable for both scripts and classes and such, and is better
		// suited to use SearchField/SearchEnvironment on than just script.
		//Make pure to search for non implementers.
		virtual Environment* GetEnvironment() = 0;

		virtual Element* GetParent() = 0;

		//The safe version of GetScript
		Script* FetchScript()
		{
			return !this ? GetScript() : nullptr;
		}

		//The safe version of GetProject
		Project* FetchProject()
		{
			return !this ? GetProject() : nullptr;
		}

		//The safe version of GetEvironment
		Environment* FetchEnvironment()
		{
			return !this ? GetEnvironment() : nullptr;
		}

		//The safe version of GetParent
		Element* FetchParent()
		{
			return !this ? GetParent() : nullptr;
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
			return !this ? GetCommons() : nullptr;
		}

		virtual Record* GetSyntaxTree() = 0;
		
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
			LoadFromRecord(rec);
		}




		virtual std::string GetName() = 0;
	protected:
		
		virtual void SetParent(Element*) = 0;

		void NoAttached()
		{
			//This simply removes the attach check by confirming it's already attached.
			if (IsAttached() == false)
			{
				GetElementFlags() |= ElementFlag::Attached;
				OnAttach();
			}
			
		}

		bool IsAttached()
		{
			return GetElementFlags() & ElementFlag::Attached;
		}

		ElementFlag& GetElementFlags()
		{
			return GetComponentData<Data>().flags;
		}

		void DeclareParentTo(Element* child)
		{
			child->SetParent(this);
			child->GetElementFlags() |= ElementFlag::Attached;
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

		Environment* GetEnvironment() override
		{
			return _parent;
		}


		Element* GetParent() override;


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