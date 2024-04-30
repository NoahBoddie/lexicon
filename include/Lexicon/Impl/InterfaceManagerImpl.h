#pragma once

#include "Interface.h"

//This isn't the real name of this file, instead, this is the Impl/ version of interface manager.
// thus meaning it isn't compiled into someone's system.

namespace LEX
{

	inline static std::unordered_map<std::string_view, Interface*> interfaceList;


	extern "C" __declspec(dllexport) inline bool RegisterInterface_Impl(Interface& ifc, std::string_view name)
	{
		logger::info("registering. . .");
		interfaceList[name] = &ifc;

		logger::info("Registered interface");
		return true;
	}


	//This should be an external function.
	extern "C" __declspec(dllexport) inline Update RequestInterface_Impl(Interface*& out, std::string_view name, uintptr_t version)
	{
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

}