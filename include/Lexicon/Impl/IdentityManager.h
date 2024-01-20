#pragma once

//cut
#include "TypeID.h"
#include "FieldDirectory.h"

//src
#include "TypePolicy.h"
#include "ITypePolicy.h"

namespace LEX
{
	struct ITypePolicy;

	

	struct IdentitySet
	{
		//This is the object that handles the management of the ranges of objects

		//Might be the name that is given to the set, like core types or SkyrimObjects. Mainly used as a
		// debug thing.
		std::string_view name;

		//This is the name of the 
		uint32_t startValue = 0;
		uint16_t offSize = 0;
		

	private:
		//Set and truncated to the offset value.
		//I'm thinking the range must be 16 bit.
		std::vector<PolicyBase*> _range{};

	public:
		TypeID GetTypeID(uint16_t offset)
		{
			return startValue + offset;
		}

		PolicyBase*& GetAtOffset(uint16_t offset)
		{
			assert(offset < offSize);
			
			if (auto size = _range.size(); size < offset && offSize > size){
				_range.resize(min(offset + 1, offSize));
			}
			//RGL_LOG(trace, " size end:{}", _range.size());
			return _range[offset];
		}

		IdentitySet(std::string_view n, uint32_t val, uint16_t rng) :
			name{ n }, startValue{ val }, offSize{ rng }
		{
			//IDEA, if the name is empty, it's determined to be a vacant lot basically. 
			// I'll want to use this for 
		}

	};

	//The class that manages the base TypeID system for all objects, classes
	class IdentityManager
	{
		inline static std::vector<IdentitySet> idList;//This ID list should start with the null type.

		//Zero is a claimed id, I'll make it likely an actual value.
		inline static uint32_t nextID = 1;


	protected:
		static IdentitySet* _GetIDSet(uint32_t id)
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
		static TypeCode _GetTypeCode(uint32_t id)
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
	public:


		static TypeID ClaimID(PolicyBase* policy, TypeIndex index, TypeOffset offset)
		{
			//Make this one that uses type code.
			
			//This gets the id set, and then uses the offset

			//I can't use abstract only, because generic stuff shouldn't have those.
			
			PolicyBase*& slot = idList[index].GetAtOffset(offset);
			
			//check here 
			
			slot = policy;
			RGL_LOG(debug, "offset in claim id {}", offset);
			TypeID id = TypeID::CreateID(idList[index].startValue + offset);

			logger::info("is??? {}", !policy);

			//assign policy id here.
			policy->SetTypeID(id);
			
			return id;
		}

		static TypeID ClaimID(PolicyBase* policy, std::string_view name, TypeOffset offset)
		{
			//This just uses the raw value. Will use the above "ClaimID"

			//Would like to make this better later.
			uint32_t index = GetIndexFromName(name);
			return ClaimID(policy, index, offset);

		}

		static TypeID ClaimID(PolicyBase* policy, uint32_t id)
		{
			//This just uses the raw value. Will use the above "ClaimID"

			//Would like to make this better later.
			uint32_t index = GetOffsetFromID(id);
			uint32_t offset = id - idList[index].startValue;
			return ClaimID(policy, index, offset);

		}

		//This is for when one needs to manually specify each entry, thus it will not be claiming ids
		static uint32_t GenerateID(std::string_view debug_name = "", uint16_t range = 0)
		{
			//Generate a type code instead so I can make it a type ID.


			//thread locking to prevent 2 things from getting the same ID.

			static std::mutex lock;

			std::lock_guard<std::mutex> guard{ lock };

			uint32_t id = nextID;

			size_t size = idList.size();

			idList.emplace_back(debug_name, nextID, ++range);
			nextID += range;

			return id;
		}

		//This does the deed automatically, should link to each possible itype that inherits this one.
		//Due to the fact it also claims as it generates, a different name may be in order.
		static uint32_t ObtainID(FieldDirectory script_dir, uint16_t range = 0)
		{
			//DEPRECATED, I don't think I want to make this for 


			//This isn't used on interfaces, who largely know who and where they go to, this is used on class types.

			//thread locking to prevent 2 things from getting the same ID.

			static std::mutex lock;

			std::lock_guard<std::mutex> guard{ lock };

			uint32_t id = nextID;

			size_t size = idList.size();

			idList.emplace_back(script_dir.GetFullName(), nextID, ++range);
			nextID += range;

			return id;
		}

		static uint32_t ObtainID(PolicyBase* policy)//, uint16_t range = 0)
		{
			//This isn't used on interfaces, who largely know who and where they go to, this is used on class types.

			//thread locking to prevent 2 things from getting the same ID.

			if (!policy) {
				return 0;
			}
			else if (auto id_query = policy->GetTypeID(); id_query != 0) {
				return id_query;
			}

			static std::mutex lock;

			std::lock_guard<std::mutex> guard{ lock };

			TypeID id = TypeID::CreateID(nextID);

			size_t index = idList.size();

			idList.emplace_back("@Loose", nextID, 0);

			RGL_LOG(debug, "offset in obtain id {}", nextID);

			//New stuff
			PolicyBase*& slot = idList[index].GetAtOffset(0);

			//check here 

			slot = policy;
			//-end new stuff here
			
			//assign policy id here.
			policy->SetTypeID(id);

			nextID++;

			return id;
		}


		static uint32_t GetIndexFromName(std::string_view name)
		{
			auto size = idList.size();

			for (uint32_t i = 0; i < size; i++)
			{
				if (idList[i].name == name)
					return i;
			}

			return max_value<uint32_t>;
		}



		static uint32_t GetIndexFromID(uint32_t id)
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

		static uint32_t GetOffset(uint32_t id)
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

		static uint16_t GetOffsetFromID(TypeID id)
		{
			return GetOffset(id);
		}



		static TypeID GetIDFromOffset(uint32_t index, uint16_t offset)
		{
			//TODO: GetIDFromOffset needs checks

			return idList[index].startValue + offset;
		}

		static TypeID GetIDFromOffset(std::string_view name, uint16_t offset)
		{
			GetIDFromOffset(GetIndexFromName(name), offset);
		}

		static PolicyBase* GetTypeByOffset(std::string_view name, TypeOffset offset)
		{
			//Change this things name so I can include everything in core later.

			TypeCode code = TypeCode::Create(GetIndexFromName(name), offset);
			RGL_LOG(trace, "@offset {}:{}", code.index, code.offset);

			//TODO: I don't care about safies right now  but this will cause issues.

			return idList[code.index].GetAtOffset(code.offset);
		}

		static PolicyBase* GetTypeByID(TypeID id)
		{
			TypeCode code = _GetTypeCode(id);
			RGL_LOG(trace, "@id {}:{}", code.index, code.offset);
			//I don't care about safies right now
			
			return idList[code.index].GetAtOffset(code.offset);
		}

		//TODO: The next concept for IdenMan is figuring out how to make use of the given range
	};

}