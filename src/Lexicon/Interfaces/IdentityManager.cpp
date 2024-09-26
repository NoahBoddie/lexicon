#pragma once

#include "Lexicon/Interfaces/IdentityManager.h"


//src
#include "Lexicon/Engine/Environment.h"
#include "Lexicon/Engine/PolicyBase.h"
#include "Lexicon/ITypePolicy.h"
#include "Lexicon/Engine/VoidPolicy.h"

#include "Lexicon/InherentType.h"

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


	std::vector<IdentityData>& GetDataList()
	{
		//I handle this through due to the fact that register interface is handled along with the rest of the construction
		// objects (honestly a dumb idea). This ensures no matter how early a singleton is constructed, the interface list is
		// constructed before something tries to add to it.
		static std::vector<IdentityData> dataList{};
		//logger::info("dataList.size() => {}", dataList.size());
		return dataList;
	}



	inline std::mutex _lock;
	//inline std::vector<IdentityData> dataList;
	//Policy list starts with an entry immediately. The void policy.
	inline std::vector<PolicyBase*> policyList{ nullptr };

	//TODO: Instead of having to search every identity list, I think I may make a second list that basically points to what index it goes to. 
	// This is easier to do, but also with the advent of so many different generated object types, it will become more performant to just do this.


	inline std::array<PolicyBase*, InherentType::kTotal> inherentTypes
	{
		new VoidPolicy{}, 
	};



	inline PolicyBase* _voidPolicy = new VoidPolicy{};

	//Not needed now policy list exists.
	inline uint32_t nextID = InherentType::kTotal;


	
	PolicyBase* IdentityManager::GetInherentBase(InherentType type)
	{
		return inherentTypes[type];
	}
	
	ITypePolicy* IdentityManager::GetInherentType(InherentType type)
	{
		return GetInherentBase(type);
	}


	uint32_t IdentityManager::GetIDFromIndex(TypeIndex index)
	{
		auto& dataList = GetDataList();

		assert(dataList.size() > index);

		//logger::info("dataList.size() ({}) > index ({})", dataList.size(), index);

		return dataList[index].startID;
	}


	TypeIndex IdentityManager::GetIndexFromName(std::string_view name)
	{
		auto& dataList = GetDataList();

		auto size = dataList.size();

		for (int i = 0; i < size; i++)
		{
			auto& entry_name = dataList[i].name;
			
			if (!entry_name.empty() && entry_name == name)
				return i;
		}

		return -1;
	}


	TypeIdentity IdentityManager::GetIdentityFromID(TypeID id)
	{
		auto& dataList = GetDataList();

		auto size = dataList.size();

		uint32_t res = -1;

		for (uint32_t i = 0; i < size; i++)
		{
			if (dataList[i].startID < id)
				continue;

			if (!i)
				break;

			auto& data = dataList[--i];

			if (data.startID <= id)
				return TypeIdentity{ data.startID, i, id - data.startID };
		}


		return {};
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

		//uint32_t nextID = policyList.size();

		auto& dataList = GetDataList();

		TypeIndex index = dataList.size();

		auto size = policyList.size();

		dataList.emplace_back(name, nextID, ++range);

		nextID += range;

		policyList.resize(size + range);
		//post resize, this should probably fill each entry with an index.

		return index;
	}

	uint32_t IdentityManager::ObtainID(PolicyBase* policy)
	{
		//uint32_t nextID = policyList.size();

		if (!policy) {
			return 0;
		}
		else if (auto id_query = policy->GetTypeID(); id_query != 0) {
			return id_query;
		}

		std::lock_guard<std::mutex> guard{ _lock };

		TypeID id = TypeID::CreateID(nextID);

		auto& dataList = GetDataList();

		dataList.emplace_back("", nextID, 1);//shouldn't this be 1 since it's a size?
		policyList.emplace_back(policy);

		nextID++;
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
		if (id < InherentType::kTotal) {
			return GetInherentBase((InherentType)id.value());
		}

		return policyList[id];
	}

}