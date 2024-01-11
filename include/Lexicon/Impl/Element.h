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
		using GenericSig = int;

		//Signature is the thing that is used to handle functions being ambiguious but different in their parameters
		// 2 signatures are counted as equals if they are the same, the parameters of one can qualify as the parameters of another
		// ie void name(float, float, float) vs void name(int, int, int) will be ambiguious to 
		//Note though, a function signature is an actual type, this is just a container to help the comparison.
		//Also remember function signatures are generic and one is created each time a function with a different arg count is created.

		//Also note, I'd very much so rather use a set for this.
		using Signature = int;
		//FuncSig
	}

	struct Element : public Component
	{
		//TODO: For all of these, there should be an "errorless" version, that just wraps the function and returns some result enum or something.


		virtual Project* GetProject()
		{
			//If it is a project, it will return project.
			throw EnvironmentError("No implementation 'GetMember'.");
			return nullptr;
		}

		virtual Script* GetScript()
		{
			//This will generally
			//Gets parent script rather than parent environment. If it is a script, it will return script.
			throw EnvironmentError("No implementation 'GetMember'.");
			return nullptr;
		}

		
		
		virtual Environment* GetEnvironment()
		{
			//Basically, like get class, but can be suitable for both scripts and classes and such, and is better
			// suited to use SearchField/SearchEnvironment on than just script.
			//Make pure to search for non implementers.
			return nullptr;
		}

		virtual Element* GetParent()
		{
			throw EnvironmentError("No implementation 'GetMember'");
			return nullptr;
		}


		//Querying the lot of these without them being the type that actually expects it should cause a compiling error.

		//returns true if it's project is a shared project.
		bool IsShared()
		{

			return GetProject() == GetShared();
		}

		static Project* GetShared();

		virtual CommonScript* GetCommons()
		{
			//This 
			return nullptr;//GetScript()->GetCommons();
		}

		
		virtual Script* FindScript(std::string name)
		{
			//Note, this being virtual is simply the essence of convience.

			//The gist of this script should use the current project + shared if not shared to find the script.
			// The real implementation should be project. Should be used from Search (because search is safe find can fail)
			//return GetProject()->FindScript(name);
			return nullptr;
		}



		virtual Record* GetSyntaxTree()
		{
			throw EnvironmentError("No implementation of Syntax Tree.");
		}


		//Will likely remain unused, the ComponentType might work more suitably.
		//template <class Self>
		//constexpr std::string_view GetName(this Self&& self) {
		//	return typeid(Self).name();
		//	//return TypeName<Self>::value;
		//}

		virtual std::string GetName() = 0;
	protected:
		
		//virtual void SetName(std::string name)
		//{
			//Not required I don't think, this is upto the record given, 
			// and if it's not, it's an individual class thing (like for projects).
			//throw EnvironmentError("No implementation 'SetName'");
		//}

		virtual void SetParent(Element*)
		{
			throw EnvironmentError("No implementation 'SetParent'");
		}

		void DeclareParentTo(Element* child)
		{
			child->SetParent(this);
		}

		//virtual void SetSyntaxTree(Record& ast)
		//{
			//This isn't even really required as a function, that's what the construction is for no?
			//throw EnvironmentError("No implementation of 'SetSyntaxTree'.");
		//}

	public:
	};

}