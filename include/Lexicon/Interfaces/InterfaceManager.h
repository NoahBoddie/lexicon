#pragma once

#include "Interface.h"

namespace LEX
{
	struct Interface;


#ifdef LEX_SOURCE
#define LEX_API extern "C" [[maybe_unused]] __declspec(dllexport)
#else
#define LEX_API
#endif

	//Both of these should be in a source file some where.
	LEX_API bool RegisterInterface_Impl(Interface& ifc, std::string_view name);
	
	//This should be an external function.
	LEX_API Update RequestInterface_Impl(Interface*& out, std::string_view name, uintptr_t version);
	
	



	struct InterfaceManager
	{

		static bool RegisterInterface(Interface& ifc, std::string_view name)
		{
#ifdef LEX_SOURCE
			return RegisterInterface_Impl(ifc, name);
#else
			using Self = decltype(RegisterInterface_Impl);
			return ExternCall<Self>(LEX_BINARY_MODULE, "RegisterInterface_Impl", ifc, name);
#endif

		//If this is the source, it should just directly use the implemetation.
			
		}

		static Update RequestInterface(Interface*& out, std::string_view name, uintptr_t version)
		{
#ifdef LEX_SOURCE
			return RequestInterface_Impl(out, name, version);
#else
			using Self = decltype(RequestInterface_Impl);
			return ExternCall<Self>(LEX_BINARY_MODULE, "RequestInterface_Impl", out, name, version);
#endif

			
		}


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
	};

}