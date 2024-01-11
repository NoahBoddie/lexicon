#pragma once

#include "Origin.h"

#include "ObjectInterface.h"
namespace LEX
{

	//main function of the object Ctor is to tell Lexicon how another project would create it's object, since that
	// object is ultimately foreign to it.
	//Also playing with the concept of this taking a pointer to args similar to that of 
	using ObjectCtor = Object * (*)();




	class ObjectInfo : public ObjectInterface
	{
	public:
		//This holds data.


		//This contains the original type_info and module the object came from.
		Origin _origin{};

		//To all things that will submit and try to request an object that's submitted, this is what will handle
		// different interpretations of that object. (Such as being in a different namespace, a different version
		// or biggest of all, SKSE's TESForm vs RE::TESForm
		//May delegate this to the object manager.
		std::vector<std::string_view> _sourceNames;

		//Unsure if this would even be needed, but it would make the looking up a bit faster.
		//This should be as large as id offset.
		//^//std::vector<ITypePolicy*> _typeRange;

		//This function carried from another project is a constructor that allows an object to be able to
		// create new versions of itself, or allow one to request a new object merely by knowing its type even
		// if the object didn't come from that very project.
		ObjectCtor _ctor;


		//All ideas from the claimed id to  the end of the offset are a part of the registered type.
		// So think of from form all the way
		uint32_t claimedID;
		uint32_t idOffset;


	};

}