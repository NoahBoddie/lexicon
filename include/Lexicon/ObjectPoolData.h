#pragma once

#include "ObjectData.h"

namespace LEX
{
	
	enum struct PoolState
	{
		kInert,			//beginning.Has nothing in it, but it's claimed still.
		kActive,			//has active data in it, and is referencing data.
		kReserved,		//The space is no longer used but still is referenced
		kFree,			//Has no references and has no data.

		kInvalid,		//An unexpected combination. May result in bleeding ocular receptacles
		//Alloc for uninitialized
	};

	struct PoolData
	{


		union
		{
			uintptr_t raw = 0;

			ObjectData data;
			struct
			{
				uint32_t right, left;
			};

		};


		//None will serve as the uninitialized tag. If it's set to reference (aliased as free), it's currently empty, and can be 
		// marked as free.
		//If it's marked as none, this means the object hasn't be initialized yet, and only the place that allocated that can use it.
		ObjectDataType type = ObjectDataType::kClaimed;
		
		uint32_t refs = -1;

		//if the object data type is a reference, then the object data isn't


		PoolState GetState() const
		{
			switch (type)
			{
			case ObjectDataType::kClaimed:
				if (refs == -1)
					return PoolState::kInert;
				else
					return PoolState::kReserved;

			case ObjectDataType::kFree:
				if (!refs)
					return PoolState::kFree;

				break;

			case ObjectDataType::kValue:
			case ObjectDataType::kPointer:
				return PoolState::kActive;

			}

			throw nullptr;
			assert(false);

			return PoolState::kInvalid;
		}
	};


	//This is stored within the given object policy for easy access.
	struct ImaginaryPoolManager
	{
		inline static std::vector<PoolData> _objPool;

		//These must both be equal or have an actual value
		inline static uint32_t _left = -1;
		inline static uint32_t _right = -1;

		//All these functions are test functions, they will not represent the final product.

		//NOTE, this severely will need a finding function depending on the situation one finds themselves in within that regard. Just for stuff like ActiveEffects
		// or inventory items being stored. Mainly just so they don't clump up or some shit.
		// compare will help with this.




		//Need pool states
		// Uninit- beginning. Has nothing in it, but it's claimed still.
		// Active- has active data in it, and is referencing data.
		// Reserved- The space is no longer used but still is referenced
		// Free- Has no references and has no data.


		//Note, most functions dealing with free instances will have to thread lock themselves so messing around with it
		// doesn't cause some mismatch.

		//Needed functions

		//FindPool. Should be able to measure object data some how. Low priority.
		// <!> On testing equivalency, it's kinda simply. First, test the value data. If they are equal, that resolves that. But, if they aren't
		//      The function should come with an area to provide the size of the object. This will allow us to know the side of the data.
		//ObtainFreePool
		//IncrementRef
		//DecrementRef(uint32) if decrements to 0, calls destroyPoolData.
		//Check Validation/Free
		//InitializePoolData(uint32, ObjectData, bool) object data to fill, pool data type, true for pointer, false for value.
		//DestroyPoolData(uint32) deletes the information in the pooled object if exists. If it has references, it it becomes claimed. If not, it needs to call free.
		//FreePoolData(uint32) Expects the pooled data to be destroyed. 
		//std::pair<uint32~> GetFreePoolsAtIndex(uint32) Checks left and right of this index for available pools. When something becomes free it will use this
		//	to find it's left and right to adjust what they think is free. Notably, all one has to do is find one side, and the other side is apparent.



		//Note, the lion's share of this has to be virtual, and goes within ObjectPolicy specifically.

		//Virtual functions
		[[nodiscard]] static  uint32_t ObtainPool()
		{//lock

			uint32_t index;

			if (_left != -1)
			{
				index = _left;
			}
			else
			{
				index = _objPool.size();

				_objPool.emplace_back();
			}

			return index;
		}

		static  void IncPoolRef(uint32_t i)
		{
			//verify loc.

			auto& data = _objPool[i];

			//Check states

			data.refs++;

			logger::info("index: {}, inc {}", i, data.refs);
		}

		//No discard?
		static  uint32_t DecPoolRef(uint32_t i)
		{
			//verify loc.

			auto& data = _objPool[i];

			//Check states

			//check that refs isn't so low that it'd wrap around.

			uint32_t result = --data.refs;

			logger::info("index: {}, dec {}", i, result);

			if (!result) {
				DestroyPool(i);
			}

			return result;
		}

		[[nodiscard]] static ObjectData InitializePool(uint32_t i, ObjectData data, bool is_ptr)
		{
			//returns a boolean that says it was created or not.



			_objPool[i].data = data;

			_objPool[i].type = is_ptr ? ObjectDataType::kPtr : ObjectDataType::kVal;



			_objPool[i].refs = 1;

			ObjectData result;

			result.fstVal = i;

			return result;
		}

		static void DestroyPool(uint32_t i)
		{
			//It's well possible the info will want to use this so this should be virtual

			//DestroyPoolData(uint32) deletes the information in the pooled object if exists.If it has references, it it becomes claimed.If not, it needs to call free.

			//lock




			PoolData& data = _objPool[i];

			auto state = data.GetState();

			get_switch(state)
			{
			case PoolState::kActive:
			{

				//Turn into an object and then unhandle it.

				//Clear the owning data out of it.

				logger::info("index: {}, destroying value {}", i, data.raw);

				data.raw = 0;

				//data.type = data.refs ? ObjectDataType::kClaimed : ObjectDataType::kFree;

				//If it has references it's claimed (reserved), if not it's free.
				if (data.refs) {
					data.type = ObjectDataType::kClaimed;
				}
			}
			case PoolState::kReserved:
				if (switch_value == PoolState::kReserved)
					logger::info("idx{} was already destroyed.", i);

				if (!data.refs) {
					FreePool(i);
				}
				break;
			}

			return;

			if (state == PoolState::kActive) {

				//Turn into an object and then unhandle it.

				//Clear the owning data out of it.

				logger::info("index: {}, destroying value {}", i, data.raw);

				data.raw = 0;

				//data.type = data.refs ? ObjectDataType::kClaimed : ObjectDataType::kFree;

				//If it has references it's claimed (reserved), if not it's free.

			}



			if (data.refs) {
				data.type = ObjectDataType::kClaimed;
			}
			else {
				FreePool(i);
			}


		}


		//Non-virtual

		static void FreePool(uint32_t i)
		{
			//Makes assumptions that it's in a state to be freed. Shouldn't be called by users.
			PoolData& data = _objPool[i];

			data.type = ObjectDataType::kFree;

			assert(!data.refs);

			auto siblings = GetNearestFreePools(i);

			//TODO: Change the object pool setting into a function. With safeties.
			if (siblings.first != -1) {
				_objPool[siblings.first].right = i;
				_objPool[i].left = siblings.first;
			}
			else {
				_left = i;
				_objPool[i].left = -1;
			}

			if (siblings.second != -1) {
				_objPool[siblings.second].left = i;
				_objPool[i].right = siblings.second;
			}
			else {
				_right = i;
				_objPool[i].right = -1;
			}



			logger::info("free index: {}, g_ls {}, g_rs {}, l_ls {}, l_rs", i,
				_left, _right, siblings.first, siblings.second);
		}


		static uint32_t GetClosestFreePool(uint32_t i, bool is_right)
		{
			uint32_t result = is_right ? _right : _left;
			//This is as close as we're getting to good.
			while (true)
			{
				PoolData& data = _objPool[result];

				if (is_right) {
					if (i > result)
						return result;
				}
				else
				{
					if (result > i)//left > i
						return result;
				}
				//not needed, but a precaution.
				auto buffer = is_right ? data.right : data.left;

				if (buffer == -1) {
					logger::critical("it actually happened :skull:");
					return result;
				}

				result = buffer;
			}

			return 0;
		}

		static std::pair<uint32_t, uint32_t> GetNearestFreePools(uint32_t i)
		{
			std::pair<uint32_t, uint32_t> result{ -1 , -1 };

			assert(i != _left && i != _right);

			if (_left == -1)
			{
				return result;
			}

			if (_left == _right)
			{
				if (_left < i)
					return { _left, -1 };
				else
					return { -1, _right };
			}

			result.first = GetClosestFreePool(i, false);//_left;
			result.second = GetClosestFreePool(i, false);//_right;
			/*
			while (true)
			{
				PoolData& data = _objPool[result.first];

				if (data.right > i)
					break;

				result.first = data.left
			}
			//*/

			return result;
		}

	};

	struct FakeObject
	{
		static constexpr uint32_t invalid = -1;

		uint32_t index = invalid;

		FakeObject(ObjectData stuff)
		{
			index = ImaginaryPoolManager::ObtainPool();
			auto toss = ImaginaryPoolManager::InitializePool(index, stuff, false);

		}

		FakeObject() = default;


		//A big ol note for these, it should not allow invalid reference data to be copied, so instead, it should give it an empty object data.


		//I have no fucking clue how to do the originals (though I'm sure it's move, copy, swap) so I'm doing it manually for right now.
		FakeObject(const FakeObject& other)
		{
			index = other.index;
			ImaginaryPoolManager::IncPoolRef(other.index);

		}

		FakeObject(FakeObject&& other)
		{
			index = other.index;
			ImaginaryPoolManager::IncPoolRef(other.index);
		}

		FakeObject& operator=(const FakeObject& other)
		{
			index = other.index;
			ImaginaryPoolManager::IncPoolRef(other.index);
			return *this;
		}


		FakeObject& operator=(FakeObject&& other)
		{
			index = other.index;
			ImaginaryPoolManager::IncPoolRef(other.index);
			return *this;
		}

		~FakeObject()
		{
			ImaginaryPoolManager::DecPoolRef(index);
		}

	};


}