#pragma once

#include "Lexicon/Component.h"

namespace LEX
{
	class Prototype;
	class Coroutine;

	//Make independent
	enum struct FieldQuery
	{
		Success,
		Failure,
		Ambiguous,
	};




	//Should be expressly abstract.
	struct Environment : public Component
	{
		//Note, the evironment itself is very interested in doing things in post declaration, so it can load it's script directives.




		//To pointer or not to pointer?
		//Record* header = nullptr;
		Record header;



		//The environment is the representation and holding body of it's functions and properties.
		//Environments have no parentage, as that would imply other top level scopes, or functions being used as environments.

		//What an environment will have is other environments that are also stored and managed, as sibling environments

		//It actually may have child environments, and this environment is likely going to be for structs (if I ever fucking make those).
		// But generally, an environment is going to be a 1 class show, not alone expecting any parents.



		//This might have some different way of accounting.
		std::vector<Coroutine*> functions;//





		//Need to make a GetFunction here to work here, so I can do something along the lin
	};



	

	class Script_;
	class Function_;
	class Variable_;
	
	class Subroutine__;


	//A helper class that *might* be used by functions (but not subroutines) to organize what variables can and cannot be addressed during compile time.
	// Issue is, it's going to actually be a temporary object. And when I say temporary, I mean VERY temporary, kinda like how process chains work.
	//The representation of scope will then probably not be very relevant outside of compiling.
	class Scope_;

	//AKA VarData, this should also store the subroutine that functions as the default value of the complex.
	class MemberInfo_;
	struct EX__11
	{
		//Should contain name, subroutine, TypeID
		std::string name;
		LEX::ObjectID varType;
		
	};


	class RuntimeTypeInfo_;//AKA, prototype which didn't actually fit the proper functionality.

	class Project_;



	struct Environment_ : public Component
	{
		virtual Project_* GetProject()
		{
			return nullptr;
		}

		virtual Environment_* GetParent()
		{
			return nullptr;
		}
		
		virtual Script_* GetScript()
		{
			return nullptr;
		}

		virtual RuntimeTypeInfo_* GetTypeInfo()
		{
			return nullptr;
		}

		
		virtual Function_* GetFunction()
		{
			//Needs a way to differentiate between these

			// _GetInternalFunction
			// _GetExtneralFunction?
			// _GetStaticFunction
			// _GetExtensionFunction

			return nullptr;
		}


		virtual Function_* GetVariable()
		{
			return nullptr;
		}

		virtual MemberInfo_* GetMember()
		{
			return nullptr;
		}
	};
}