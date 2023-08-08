#pragma once 

#include "Lexicon/Impl/ObjectID.h"
#include "Lexicon/Impl/Script.h"
#include "Lexicon/Impl/Environment.h"
#include "Lexicon/Impl/Hierarchy.h"

namespace LEX
{
	
	struct Prototype : public Hierarchy, public Environment
	{
		//As a hierarchy object, this should only have hierarch to the core object it derives from

		//So prototype will work with heirachy in order to establish its parent. This is helpful because if I do make them able to extend 
		//objects. So another aspect to wonder is, how will I be able to tell which it is? I'll use object type.
		LEX::Script* home = nullptr;//This equals is grandparent environment, the script it came from.

		Prototype* extends = nullptr;//Probably can cover with hierarchy.

		//TODO:Prototype needs the ability to restrict any variable that has it's own type from existing within itself.
		// It's important to note, structs are a lot less like objects, and more like value types, even if nothing is used, it will instantiate a value.
		// To get around this, you can use the construct type in order to get away with it


		//The concept of the prototype would be the struct prototype, it in of itself is an environment, it owns functions, and makes ones of its own first.
		// Then it will import functions from it's parent environments, and if it finds one with the same name, it will not import it (basically, a virtual function).

		//So, the when a function is called from a struct, first it checks the prototype, then the scope it's from. Then it checks the scopes its included.
		// Then it checks the scopes it's imported.

		struct VarData
		{
			std::string name;
			LEX::ObjectID varType;
		};



		//If I could have this fulfill it's name and sorta have


		//This is what is used when searching 
		std::vector<VarData> variableData;

		//This is important to use to include and know what offset it's variables are from where you should be looking.
		// Example @ $VarIndex
		uint32_t _varIndex;//I'll never need THAT much, and I can use the rest of this value for base id.
		
		ObjectID baseID;//Used so I can tell if it derives from a real object better. NOTE, hierarchy covers that


		Coroutine* GetConstructor()
		{
			return functions.back();
		}

		Coroutine* GetMethod(std::string name)
		{
			//Get method should go the a similar song and dance that Script does looking for extension functions.
			return nullptr;
		}


		size_t GetMember(std::string name)
		{
			//The idea is it should give you the prototype of the variable data
			// this instead will simply give you the index of said place.
			// If invalid it will be equal to npos.


			return full_value<size_t>;
		}

		Prototype* GetExtends()
		{
			return nullptr;
		}

		bool ExtendsObject()
		{
			//TODO:0xFF used for struct, please cahnge
			return baseID.value[0] != 0xFF && baseID.value[0] != 0x0;
		}



		//$VarIndex: Say I have 2 structs. A comes first, and has 4 variables. Then struct B extends it, and has 5 variables.
		// Someone is interacting with struct B, and is trying to find ITS 4th variable. It finds it, but in the actual struct,
		// its index is 8, because the first 4 variables belong to A. So instead what it will do once it finds the forth, it adds the varIndex, correcting
		// it for the structs it extends.

	};

}