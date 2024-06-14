#pragma once

#include "Interface.h"

namespace LEX
{
	struct Interface;
	
	
	extern "C" __declspec(dllexport) bool RegisterInterface_Impl(Interface& ifc, std::string_view name);

	//This should be an external function.
	extern "C" __declspec(dllexport) Update RequestInterface_Impl(Interface*& out, std::string_view name, uintptr_t version);

	struct InterfaceManager
	{
#define TEMP__EXPECTED_MODULE() "Lexicon_Test.exe"



		static bool RegisterInterface(Interface& ifc, std::string_view name);



		static Update RequestInterface(Interface*& out, std::string_view name, uintptr_t version);

//#undef TEMP__EXPECTED_MODULE

		//*
		template<std::derived_from<Interface> T>
		static bool RegisterInterface(T& ifc)
		{
			return RegisterInterface(ifc, typeid(T).name());
		}





		template<std::derived_from<Interface> T>
		static Update RequestInterface(T*& out, uintptr_t version)
		{
			Interface* buffer = nullptr;

			auto result = RequestInterface(buffer, typeid(T).name(), version);

			if (buffer)//Have to reinterpret cast because the dynamic cast will NOT work for interfaces. Getting the shit is good enough.
				out = reinterpret_cast<T*>(buffer);


			return result;
		}
		//*/

	};

}