#pragma once

#include "ObjectPolicyManager.h"

namespace LEX
{
	inline static std::vector<OBJECT_POLICY*> policyList{};

	//I actually may want these storing the pointers. Just need to access the index from there for the handles.
	inline static std::map<std::string_view, uint32_t> aliasList{};


	uint32_t ObjectPolicyManager::GetIndexFromName(std::string_view name)
	{

		auto end = aliasList.end();

		auto it = aliasList.find(name);

		if (it == end) {
			report::compile::critical("AliasNotFound");
			throw nullptr;
		}

		return it->second;
	}


	uint32_t ObjectPolicyManager::GetIndexFromCategory(std::string_view category)
	{
		auto size = policyList.size();

		for (int i = 0; i < size; i++)
		{
			if (policyList[i]->category == category) {
				return i;
			}
		}

		return -1;
	}


	OBJECT_POLICY* ObjectPolicyManager::GetObjectPolicy(uint32_t index)
	{
		assert(policyList.size() >= index);

		if (policyList.size() <= index) {
			return nullptr;
		}

		return policyList[index];
	}


	OBJECT_POLICY* ObjectPolicyManager::RegisterObjectType(std::vector<std::string_view> aliases, std::string_view category, TypeOffset range, DataBuilder builder, HMODULE source)
	{
		//Change parameters of this, include a main name. Maybe a category. But maybe not.

		auto index = IdentityManager::instance->GenerateID(category, range);

		//Crashing is unlikely and most likely will want to be unhandled. So letting it through.
		ObjectPolicyImpl* policy = new ObjectPolicyImpl;

		policy->category = category;
		policy->ctor = builder;
		policy->program = source;
		//policy->offset =

		//static_assert(false, "IdentityManager doesn't have the shit I need.");
		policy->index = index;
		//Right now, and possibly in the future I don't think I actually want to have individual offsets register 
		// themselves as objects. Instead, the entire category is an object. That's better in general.


		auto policyID = policyList.size();

		policy->policyID = policyID;

		auto end = aliasList.end();

		for (auto& alias : aliases)
		{
			auto it = aliasList.find(alias);

			if (it != end) {
				throw nullptr;
			}


			aliasList[alias] = policyID;
		}


		return policyList.emplace_back(policy);
	}

}