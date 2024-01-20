/*


#include "IdentityManager.h"

//src
#include "ITypePolicy.h"

namespace LEX
{

	IdentitySet* IdentityManager::_GetIDSet(uint32_t id)
	{
		if (id < nextID)
		{
			auto size = idList.size();

			for (int i = 0; i <= size; i++)
			{
				if (i == size || idList[i].startValue > id)
					return &idList[i - 1];
			}
		}


		return nullptr;
	}
	TypeCode IdentityManager::_GetTypeCode(uint32_t id)
	{
		if (id < nextID)
		{
			auto size = idList.size();

			for (int i = 0; i <= size; i++)
			{
				if (i == size || idList[i].startValue > id) {
					TypeIndex index = i - 1;
					TypeOffset offset = static_cast<TypeOffset>(id - idList[i - 1].startValue);
					RGL_LOG(trace, "{} code offset vs {}", offset, id);

					return TypeCode::Create(index, offset);
				}

			}
		}


		return {};
	}



	TypeID IdentityManager::ClaimID(ITypePolicy* policy, TypeIndex index, TypeOffset offset)
	{
		//Make this one that uses type code.

		//This gets the id set, and then uses the offset

		//I can't use abstract only, because generic stuff shouldn't have those.

		ITypePolicy*& slot = idList[index].GetAtOffset(offset);

		//check here 

		slot = policy;
		RGL_LOG(debug, "offset in claim id {}", offset);
		TypeID id = TypeID::CreateID(idList[index].startValue + offset);

		logger::info("is??? {}", !policy);

		//assign policy id here.
		if (policy)
			policy->SetTypeID(id);

		return id;
	}

	TypeID IdentityManager::ClaimID(ITypePolicy* policy, std::string_view name, TypeOffset offset)
	{
		//This just uses the raw value. Will use the above "ClaimID"

		//Would like to make this better later.
		uint32_t index = GetIndexFromName(name);
		return ClaimID(policy, index, offset);

	}

	TypeID IdentityManager::ClaimID(ITypePolicy* policy, uint32_t id)
	{
		//This just uses the raw value. Will use the above "ClaimID"

		//Would like to make this better later.
		uint32_t index = GetOffsetFromID(id);
		uint32_t offset = id - idList[index].startValue;
		return ClaimID(policy, index, offset);

	}

	//This is for when one needs to manually specify each entry, thus it will not be claiming ids
	uint32_t IdentityManager::GenerateID(std::string_view debug_name, uint16_t range)
	{
		//Generate a type code instead so I can make it a type ID.


		//thread locking to prevent 2 things from getting the same ID.

		std::mutex lock;

		std::lock_guard<std::mutex> guard{ lock };

		uint32_t id = nextID;

		size_t size = idList.size();

		idList.emplace_back(debug_name, nextID, ++range);
		nextID += range;

		return id;
	}

	//This does the deed automatically, should link to each possible itype that inherits this one.
	//Due to the fact it also claims as it generates, a different name may be in order.
	uint32_t IdentityManager::ObtainID(FieldDirectory script_dir, uint16_t range)
	{
		//DEPRECATED, I don't think I want to make this for 


		//This isn't used on interfaces, who largely know who and where they go to, this is used on class types.

		//thread locking to prevent 2 things from getting the same ID.

		std::mutex lock;

		std::lock_guard<std::mutex> guard{ lock };

		uint32_t id = nextID;

		size_t size = idList.size();

		idList.emplace_back(script_dir.GetFullName(), nextID, ++range);
		nextID += range;

		return id;
	}

	uint32_t IdentityManager::ObtainID(ITypePolicy* policy)//, uint16_t range = 0)
	{
		//This isn't used on interfaces, who largely know who and where they go to, this is used on class types.

		//thread locking to prevent 2 things from getting the same ID.

		if (!policy) {
			return 0;
		}
		else if (auto id_query = policy->GetTypeID(); id_query != 0) {
			return id_query;
		}

		std::mutex lock;

		std::lock_guard<std::mutex> guard{ lock };

		TypeID id = TypeID::CreateID(nextID);

		size_t index = idList.size();

		idList.emplace_back("@Loose", nextID, 0);

		RGL_LOG(debug, "offset in obtain id {}", nextID);

		//New stuff
		ITypePolicy*& slot = idList[index].GetAtOffset(0);

		//check here 

		slot = policy;
		//-end new stuff here

		//assign policy id here.
		policy->SetTypeID(id);

		nextID++;

		return id;
	}


	uint32_t IdentityManager::GetIndexFromName(std::string_view name)
	{
		auto size = idList.size();

		for (uint32_t i = 0; i < size; i++)
		{
			if (idList[i].name == name)
				return i;
		}

		return max_value<uint32_t>;
	}



	uint32_t IdentityManager::GetIndexFromID(uint32_t id)
	{
		return _GetTypeCode(id).offset;

		if (id < nextID)
		{
			auto size = idList.size();

			for (int i = 0; i < size; i++)
			{
				if (idList[i].startValue > id)
					return i - 1;
			}
		}


		return max_value<uint32_t>;
	}

	uint32_t IdentityManager::GetOffset(uint32_t id)
	{//this is not right
		return _GetTypeCode(id).offset;

		//This will be useful to move elsewhere.
		if (id < nextID)
		{
			auto size = idList.size();

			for (int i = 0; i < size; i++)
			{
				if (idList[i].startValue > id)
					return i - 1;
			}
		}


		return max_value<uint32_t>;
	}

	uint16_t IdentityManager::GetOffsetFromID(TypeID id)
	{
		return GetOffset(id);
	}



	TypeID IdentityManager::GetIDFromOffset(uint32_t index, uint16_t offset)
	{
		//TODO: GetIDFromOffset needs checks

		return idList[index].startValue + offset;
	}

	TypeID IdentityManager::GetIDFromOffset(std::string_view name, uint16_t offset)
	{
		return GetIDFromOffset(GetIndexFromName(name), offset);
	}

	ITypePolicy* IdentityManager::GetTypeByOffset(std::string_view name, TypeOffset offset)
	{
		//Change this things name so I can include everything in core later.

		TypeCode code = TypeCode::Create(GetIndexFromName(name), offset);
		RGL_LOG(trace, "@offset {}:{}", code.index, code.offset);

		//I don't care about safies right now

		return idList[code.index].GetAtOffset(code.offset);
	}

	ITypePolicy* IdentityManager::GetTypeByID(TypeID id)
	{
		TypeCode code = _GetTypeCode(id);
		RGL_LOG(trace, "@id {}:{}", code.index, code.offset);
		//I don't care about safies right now

		return idList[code.index].GetAtOffset(code.offset);
	}

}

//*/