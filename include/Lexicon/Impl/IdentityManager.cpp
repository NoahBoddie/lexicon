#pragma once

#include "IdentityManager.h"


//src
#include "Environment.h"
#include "PolicyBase.h"
#include "ITypePolicy.h"
#include "VoidPolicy.h"
namespace LEX
{
	//KILLME
	/*
std::vector<PolicyBase*> Environment::FindTypes(std::string name)
{
	auto end = typeMap.end();

	if (auto it = typeMap.find(name); end != it) {
		return { it->second };
	}
	else {
		return {};
	}
}
	//*/


	inline void test(Record& f)
	{
		PolicyBase* base = nullptr;
		base->FindTypes("");
	}

	struct IdentityData
	{
		std::string_view name;

		uint32_t startID = 0;
		uint32_t range = 0;


	public:
		TypeID GetTypeID(uint16_t offset)
		{
			assert(offset < range);

			if (offset < range)
				return TypeID::invalidValue;

			return startID + offset;
		}

		constexpr IdentityData(std::string_view n, uint32_t val, uint16_t rng) :
			name{ n }, startID{ val }, range{ rng }
		{
		}

	};
	
	struct PolicyEntry
	{
		PolicyBase* policy = nullptr;
		uint32_t index = 0;//Index serves as easy access to a category.

	};






	inline std::mutex _lock;
	inline std::vector<IdentityData> dataList;
	//Policy list starts with an entry immediately. The void policy.
	inline std::vector<PolicyBase*> policyList{ nullptr };

	inline PolicyBase* _voidPolicy = new VoidPolicy{};

	//Not needed now policy list exists.
	inline uint32_t nextID = 1;




	uint32_t IdentityManager::GetIDFromIndex(TypeIndex index)
	{
		assert(dataList.size() > index);

		return dataList[index].startID;
	}

	TypeIndex IdentityManager::GetIndexFromName(std::string_view name)
	{
		auto size = dataList.size();

		for (int i = 0; i < size; i++)
		{
			auto& entry_name = dataList[i].name;

			if (!entry_name.empty() && entry_name == name)
				return i;
		}

		return -1;
	}


	//This feels like it's really only going to be used for the other 2.
	TypeID IdentityManager::ClaimID(PolicyBase* policy, uint32_t id)
	{
		//Fix.
		std::lock_guard<std::mutex> guard{ _lock };

		assert(policyList.size() > id);


		PolicyBase*& slot = policyList[id];

		if (slot) {
			report::compile::critical("Slot for {} already taken.", id);
		}

		//check here 

		slot = policy;

		TypeID ret = TypeID::CreateID(id);

		//assign policy id here.
		policy->SetTypeID(ret);

		return ret;
	}



	//should return the index.
	uint32_t IdentityManager::GenerateID(std::string_view name, uint16_t range)
	{
		//Generates a grouped ID instead of a single one.
		std::lock_guard<std::mutex> guard{ _lock };

		uint32_t nextID = policyList.size();

		TypeIndex index = dataList.size();

		auto size = policyList.size();

		dataList.emplace_back(name, nextID, ++range);



		policyList.resize(size + range);
		//post resize, this should probably fill each entry with an index.

		return index;
	}

	uint32_t IdentityManager::ObtainID(PolicyBase* policy)
	{
		uint32_t nextID = policyList.size();

		if (!policy) {
			return 0;
		}
		else if (auto id_query = policy->GetTypeID(); id_query != 0) {
			return id_query;
		}

		std::lock_guard<std::mutex> guard{ _lock };

		TypeID id = TypeID::CreateID(nextID);


		dataList.emplace_back("", nextID, 1);//shouldn't this be 1 since it's a size?
		policyList.emplace_back(policy);

		//assign policy id here.
		policy->SetTypeID(id);

		return id;
	}



	
	ITypePolicy* IdentityManager::GetTypeByID(TypeID id)
	{
		return GetBaseByID(id);
	}
	//This too is used often.(What does this mean?)
	PolicyBase* IdentityManager::GetBaseByID(TypeID id)
	{
		//TODO: The first type should be the void policy, it should already be generated.
		if (id == 0) {
			logger::info("void {}", !!_voidPolicy);
			return _voidPolicy;
		}

		return policyList[id];
	}

}