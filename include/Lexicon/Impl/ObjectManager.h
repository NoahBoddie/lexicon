#pragma once

#include "ObjectInfo.h"


#include "FieldDirectory.h"
#include "IdentityManager.h"

namespace LEX
{
	//TODO: So registering an interface won't be like this anymore, instead we'll use the whole category thing to 
	// link an interface


	//This classes purpose is to manage custom objects and what native classes are seen as that type.
	class ObjectManager
	{
		//I thought about searching this via type id, but this is basically just for back end stuff. SO maybe not.
		// But some way to access it from there might be helpful.
		//Actually, that sort of thing might search by using ID so maybe give that some thought.

		//TODO: Remake object manager's object register. Instructions below.
		inline static std::map<std::string, ObjectInfo> objectRegistry;
		//First rename to registry table or somthing of the like. 
		// make an unordered_map that takes string views and has an ObjectInfo pointer, because multiple types
		//  can share the same info.
		// If I want to make searching easier, make an optional vector or something like that.

	public:

		//Manages how many entries of class is being referenced, 
		inline static std::map<Object*, size_t> referenceManager;

		static void RegisterType(FieldDirectory script_dir, std::type_info& type, ObjectCtor constructor, uint16_t range = 0, HMODULE origin = nullptr)
		{
			//Was likely going to make a different version of this.

			//May return some error at a later point
			static std::mutex lock;

			std::lock_guard<std::mutex> guard{ lock };


			auto base_id = IdentityManager::ObtainID(script_dir, range);

			//After this, it should try to find the base type by name. Then assign these as a name. Or does the above do this?

			ObjectInfo reggie{};

			reggie.claimedID = base_id;
			reggie._ctor = constructor;
			reggie.idOffset = range;
			reggie._origin.program = origin;
			reggie._origin.type = &type;

			objectRegistry[script_dir.GetFullName()] = reggie;

		}

		static void AddSourceType(FieldDirectory script_dir, std::type_info& type)
		{
			//This associates an existing imaginary type with a source type of a different name but the same functionality.
		}



		static ObjectInfo* GetInterface(std::string name)
		{
			return nullptr;
		}

		//void Test()
		//{
		//	RegisterType<Type>("Share::Common::Array"/*, costructor but the type makes this*/, 10/*, origin but that's made per automatic*/)
		//	TrueRegisterType(script_dir, typeid(Type), []() { return new Type();}, range, origin)
		//}
	};
}