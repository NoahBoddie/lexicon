#include "Lexicon/Interfaces/InterfaceManager.h"


namespace LEX
{
	std::unordered_map<std::string_view, Interface*>& GetInterfaceList()
	{
		//I handle this through due to the fact that register interface is handled along with the rest of the construction
		// objects (honestly a dumb idea). This ensures no matter how early a singleton is constructed, the interface list is
		// constructed before something tries to add to it.
		static std::unordered_map<std::string_view, Interface*> interfaceList{};
		
		return interfaceList;
	}
	

	bool RegisterInterface_Impl(Interface& ifc, std::string_view name)
	{
		auto& interfaceList = GetInterfaceList();
		
		if (auto& entry = interfaceList[name])
			return false;
		else
			entry = &ifc;

		
		return true;
	}

	//This should be an external function.
	Update RequestInterface_Impl(Interface*& out, std::string_view name, uintptr_t version)
	{
		auto& interfaceList = GetInterfaceList();

		auto it = interfaceList.find(name);

		if (interfaceList.end() == it)
			return Update::Invalid;

		Interface* result = it->second;

		//If the query is larger than what we can give, engine is out of date.
		if (result->Version() < version)
			return Update::Engine;

		//If what we can give cannot handle the version queried
		if (result->CanHandle(version) == false)
			return Update::Library;

		out = result;

		return Update::Match;
	}


	/*
	bool InterfaceManager::RegisterInterface(Interface& ifc, std::string_view name)
	{
#ifdef LEX_SOURCE
		//return RegisterInterface_Impl(ifc, name);
#endif

		//If this is the source, it should just directly use the implemetation.
		using _Self = bool(Interface&, std::string_view);
		return ExternCall<_Self>(LEX_BINARY_MODULE, "RegisterInterface_Impl", ifc, name);
	}

	Update InterfaceManager::RequestInterface(Interface*& out, std::string_view name, uintptr_t version)
	{
#ifdef LEX_SOURCE
		//return RequestInterface_Impl(out, name, version);
#endif

		using _Self = Update(Interface*&, std::string_view, uintptr_t);
		return ExternCall<_Self>(LEX_BINARY_MODULE, "RequestInterface_Impl", out, name, version);
	}
	//*/
}