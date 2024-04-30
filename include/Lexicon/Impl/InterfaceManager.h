#pragma once


#include "Interface.h"

#ifdef LEX_SOURCE
#include "InterfaceManagerImpl.h"
#endif

namespace LEX
{
	struct Interface;
	
	
	struct InterfaceManager
	{
#define TEMP__EXPECTED_MODULE() "Lexicon_Test.exe"



		static bool RegisterInterface(Interface& ifc, std::string_view name)
		{
#ifdef LEX_SOURCE
			//return RegisterInterface_Impl(ifc, name);
#endif
			
			//If this is the source, it should just directly use the implemetation.
			using _Self = bool(Interface&, std::string_view);
			return ExternCall<_Self>(TEMP__EXPECTED_MODULE(), "RegisterInterface_Impl", ifc, name);
		}



		static Update RequestInterface(Interface*& out, std::string_view name, uintptr_t version)
		{
#ifdef LEX_SOURCE
			//return RequestInterface_Impl(out, name, version);
#endif

			using _Self = Update(Interface*&, std::string_view, uintptr_t);
			return ExternCall<_Self>(TEMP__EXPECTED_MODULE(), "RequestInterface_Impl", out, name, version);
		}

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