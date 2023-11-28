#pragma once 

//#include "Variable.h"
//#include "Object.h"
#include "Lexicon/Impl/Prototype.h"

namespace LEX
{
	/*
	struct Construct
	{
		//Used uint32_t because it's not possible to store the max value of size_t in an Operand
		//When access, always
		constexpr static uint32_t kSelfIndex = full_value<uint32_t>;

	private:
		Prototype* _prototype;

		//For getting the object it extends, that can be the first member, and to tell if there is a fourth member, 
		// one can check the prototype.
		// only and maybes objects can be extended.



		//This would need to be a fancy variable that has some accountability on it's type.
		// or I would think, but I can probably use the prototype for that. For quick access and because a vector
		//I also think I could just have a managed pointer for this. Makes a struct even smaller, and it's not as if I'll have many issues with that.
		// as long as the prototype is valid, shouldn't have issues. Probably.
		std::vector<Variable> _members;

	public:
		bool ExtendsObject()
		{
			return _prototype ? _prototype->ExtendsObject() : false;
		}

		uint32_t Size()
		{
			return _members.size() - ExtendsObject();
		}

		//Make some copy versions of these

		Variable* GetMember(uint32_t index)
		{
			if (Size() <= index)
				return nullptr;

			return &_members[index];
		}

		Object* GetSelf()
		{
			//If a struct extends an actual object, then it will store that object as it's last variable.

			//Trust
			
			//TODO:Restore construct::Self function
			//if (ExtendsObject() == true)
			//	return &_members.back().As<ArgumentType::Object>();

			return nullptr;
		}


		//Doesn't do anything (clearly) for not, but this is one of the types that object turns into

		//When referenced via API works much like a variable does, gives a pointer but never actually lets you do anything with it.
		// Similarly, this can only be accessed by pointer and likely will never be copied.

		//Needs
	};

	//*/
}