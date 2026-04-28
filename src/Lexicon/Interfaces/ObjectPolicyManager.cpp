#include "Lexicon/Interfaces/ObjectPolicyManager.h"

namespace LEX
{
	//inline static std::vector<ObjectPolicyImpl*> _policyList{};

	//I actually may want these storing the pointers. Just need to access the index from there for the handles.
	std::map<std::string_view, uint32_t>& aliasList = make_singleton();

	std::vector<ObjectPolicy*>& _policyList = make_singleton();

	//std::vector<ObjectPolicy*> ObjectPolicyManager::_policyList{};


	uint32_t ObjectPolicyManager::GetIndexFromName(std::string_view name)
	{

		auto end = aliasList.end();

		auto it = aliasList.find(name);

		if (it == end) {
			return -1;
		}

		return it->second;
	}


	uint32_t ObjectPolicyManager::GetIndexFromCategory(std::string_view category)
	{
		//auto policyList = GetPolicyList();
		auto policyList = &_policyList;

		if (policyList)
		{
			auto size = policyList->size();

			for (int i = 0; i < size; i++)
			{
				if (policyList->at(i)->category == category) {
					return i;
				}
			}
		}

		return -1;
	}


	ObjectPolicy* ObjectPolicyManager::GetObjectPolicy(uint32_t index)
	{
		if (index == -1) {
			return nullptr;
		}

		//auto policyList = GetPolicyList();
		auto policyList = &_policyList;

		if (policyList)
		{
			assert_if_not (policyList->size() > index) {
				return policyList->at(index);
			}

			
		}

		return nullptr;
	}


	ObjectPolicy* ObjectPolicyManager::RegisterObjectType(ObjectVTable* vtable, const std::span<std::string_view>& aliases, std::string_view category, TypeOffset range, DataBuilder builder, HMODULE source)
	{
		//Change parameters of this, include a main name. Maybe a category. But maybe not.
		report::message::info("adding category {}", category);
		auto index = IdentityManager::instance->GenerateID(category, range, 
			[](const std::string_view& category, const std::span<std::string_view>& args)-> TypeOffset
			{
				ObjectPolicy* policy = ObjectPolicyManager::instance->GetObjectPolicyFromName(category);
				
				assert_if(!policy) {
					return -1;
				}

				return policy->GetOffsetFromArgs(category, args);
			});

		//Crashing is unlikely and most likely will want to be unhandled. So letting it through.
		ObjectPolicy* policy = new ObjectPolicy;

		//auto policyList = GetPolicyList();
		auto policyList = &_policyList;
		
		auto policyID = policyList->size();
		



		policyList->emplace_back(policy);

		vtable->SetPolicy(policy);
		policy->base = vtable;
		policy->category = category;
		policy->ctor = builder;
		policy->program = source;
		//policy->offset =

		//static_assert(false, "IdentityManager doesn't have the shit I need.");
		policy->index = index;
		//Right now, and possibly in the future I don't think I actually want to have individual offsets register 
		// themselves as objects. Instead, the entire category is an object. That's better in general.

		

		

		policy->policyID = policyID;

		auto end = aliasList.end();

		for (auto& alias : aliases)
		{
			auto it = aliasList.find(alias);

			if (it != end) {
				report::fault::critical("alias {} taken", alias);
			}


			aliasList[alias] = policyID;
		}


		return policy;
	}

}