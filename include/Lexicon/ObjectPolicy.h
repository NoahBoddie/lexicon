#pragma once

#include "Lexicon/TypeID.h"
#include "Lexicon/ObjectPoolData.h"
#include "Lexicon/ObjectInfo.h"
#include "Lexicon/String.h"
//*src
#include "Lexicon/Interfaces/IdentityManager.h"

namespace LEX
{
	struct ObjectPolicyImpl;


	struct IObjectPolicy
	{

		//Move this please. This is a pure interface. Instead, make a virtual function for it.
		ObjectVTable* base = nullptr;

		
		virtual ~IObjectPolicy() = default;

		//For when the impl version gets moved.
		virtual ObjectPolicyImpl* Promote() { return nullptr; }
		

		//*
		[[nodiscard]] virtual uint32_t ObtainPool() = 0;

		virtual void IncPoolRef(uint32_t i) = 0;

		
		virtual uint32_t DecPoolRef(uint32_t i) = 0;

		//DEPRECATED, use the one that obtains the pool within its use.
		virtual ObjectData InitializePool(uint32_t i, ObjectData data, bool is_ptr) = 0;

		virtual ObjectData InitializePool(ObjectData data, bool is_ptr) = 0;

		virtual ObjectData* RequestPool(uint32_t i) = 0;

		virtual void DestroyPool(uint32_t i) = 0;


		virtual uint32_t GetTypeIDFromOffset(TypeOffset) = 0;

		virtual ObjectData CreateData(uint32_t = 0) = 0;
		
		virtual uint32_t GetPolicyID() = 0;

		virtual TypeIndex GetCategoryIndex() = 0;
		virtual std::string_view GetCategoryName() = 0;
		//*/
	};

	//TODO: Idea on object policies, they derive from object vtable, so they are effective stand-ins for questions around object infos.
	// Largely, they'll just point back to the object info.
	// Problem is, in order to do this I NEED to have it be multiple inheritance, this way newer functions added via more inheritance won't interfer with either
	// side.
	//But the problem is having a return that works between the source and the user. So here's what I do. A type alias, or perhaps a macro instead.
	// Should it be the source, Most uses of OBJECT_POLICY will be ObjectPolicyImpl. But for the user it's ObjectPolicy. This allows me to obscure this connection,
	// but also access the extra vtable without casting, AND the pointers are in the same location for it to continue treating it as an ObjectPolicy.

	//Problem is I'll need to put it on a pivot, mainly so I have 2 different vtables, but still have the object PolicyBase able to be treated like an ObjectVTable.

	//So it might be something like IObjectPolicy, ObjectPolicy(Pivot), ObjectPolicyImpl OBJECT_POLICT(macro between ObjectPolicy and ObjectPolicyImpl).

	struct ObjectPolicy : public IObjectPolicy, public IObjectVTable 
	{
		//Make const plz
		Object CreateObject(uint32_t);

	};





	//Should be final. And should also be outside of this file
	struct ObjectPolicyImpl : public ObjectPolicy
	{

#pragma region ObjectVTable


		ObjectPolicy* GetObjectPolicy() override
		{
			return base->GetObjectPolicy();
		}


		//Used to tell the individual objects version. Useful if the plugin doesn't change, but what's targeted does.
		uintptr_t GetObjectVersion() override
		{
			return base->GetObjectVersion();
		}


		//Used to tell what version the class is. Helps detect when new functions are added.
		uintptr_t GetVTableVersion() override final
		{
			return base->GetVTableVersion();
		}


		//This function is used to check the object versions of this and that to tell if it's valid to use. Most times it will be.
		bool IsCompatible(const IObjectVTable* other) override final
		{
			return base->IsCompatible(other);
		}


		//Determines if it's stored in a value or in a pointer. Do NOT make this dynamic,
		StaticStoreType GetStorageType() override final
		{
			return base->GetStorageType();
		}


		//This will ALWAYS be given the actual objects data. It's then upto get_storage_type to handle it.
		bool IsPooled(ObjectData& self) override final
		{
			return base->IsPooled(self);
		}



		//IMPORTANT.
		//I think either I need a new struct to handle the concept of what this expects. Main problem is I don't want
		// a fake ass object doing things that it's not supposed to do. Maybe I'll make a tag for that.
		//ACTUALLY, just send the object data. It can reliably be turned into whichever by relying on it's assigned constexpr stored type.
		// If the type ends up changing, that's one of the things addressed in the compatibility check function.

		void Destroy(ObjectData& self) override final
		{
			return base->Destroy(self);
		}


		void Copy(ObjectData& self, const ObjectData& other) override final
		{
			return base->Copy(self, other);
		}


		void Move(ObjectData& self, ObjectData& other) override final
		{
			return base->Move(self, other);
		}

		void Initialize(ObjectData& self) override final
		{
			return base->Initialize(self);
		}


		//TODO: Not doing Object::Operate this yet, but figure out what you want to do with it before releasing.
		//virtual void Operator(std::string_view, Object*, Variable&) = 0;



		bool Exists(ObjectData& self) override final
		{
			return base->Exists(self);
		}


		std::partial_ordering Compare(ObjectData& self, ObjectData& other) override final
		{
			return base->Compare(self, other);
		}


		bool RequestDelete(ObjectData& self) override final
		{
			return base->RequestDelete(self);
		}


		TypeOffset GetTypeOffset(ObjectData& self) override final
		{
			return base->GetTypeOffset(self);
		}


		AbstractTypePolicy* SpecializeType(ObjectData& self, ITypePolicy* type) override final
		{
			return base->SpecializeType(self, type);
		}
		
		uint32_t GetTypeID(ObjectData& self) override
		{
			return base->GetTypeID(self);
		}


		
		String PrintString(ObjectData& self, std::string_view context) override
		{
			return base->PrintString(self, context);
		}

#pragma endregion


#pragma region ObjectPool functions


		void FreePool(uint32_t i)
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



			logger::info("free index: {}, g_ls {}, g_rs {}, l_ls {}, l_rs {}", i,
				_left, _right, siblings.first, siblings.second);
		}


		uint32_t GetClosestFreePool(uint32_t i, bool is_right)
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

		std::pair<uint32_t, uint32_t> GetNearestFreePools(uint32_t i)
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


		[[nodiscard]] uint32_t ObtainPool() override
		{//lock
			//Obtain pool is being removed from being virtual. Instead, one can do it all at once. No discard on the index.


			uint32_t index;

			if (_left != -1)
			{
				//TODO: Need to prevent this from being claimed
				index = _left;
			}
			else
			{
				index = _objPool.size();

				_objPool.emplace_back();
			}

			return index;
		}

		void IncPoolRef(uint32_t i) override 
		{
			//verify loc.

			auto& data = _objPool[i];

			//Check states

			data.refs++;

			logger::info("index: {}, inc {}", i, data.refs);
		}


		uint32_t DecPoolRef(uint32_t i) override
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

		//return on this are gonna be a changing. But this point, the id's already obtained. No reason to have this.
		ObjectData InitializePool(uint32_t i, ObjectData data, bool is_val) override
		{
			//returns a boolean that says it was created or not.


			if (_objPool[i].type == ObjectDataType::kFree)
			{
				auto next_free = _objPool[i].right;

				if (_left == _right)
					_right = next_free;

				logger::info("next free {}", next_free);
				_left = next_free;
			}
			//TODO: Use a reference here, this constant accessing is annoying looking.
			_objPool[i].data = data;

			_objPool[i].type = is_val ? ObjectDataType::kVal : ObjectDataType::kPtr;



			_objPool[i].refs = 1;

			ObjectData result;

			result.fstVal = i;
			logger::info("index: {}, init {}", i, 1);
			return result;
		}


		ObjectData InitializePool(ObjectData data, bool is_val) override
		{
			auto i = ObtainPool();
			return InitializePool(i, data, is_val);
		}

		ObjectData* RequestPool(uint32_t i) override
		{
			//For now, just give them the index. Later? Make sure it's valid if you're going to give it.

			if (_objPool.size() <= i)
				return nullptr;
			
			auto& entry = _objPool[i];

			return &entry.data;
		}

		void DestroyPool(uint32_t i) override
		{
			//It's well possible the info will want to use this so this should be virtual

			//DestroyPoolData(uint32) deletes the information in the pooled object if exists.If it has references, it it becomes claimed.If not, it needs to call free.

			//lock

			logger::debug("Attempting to destroy pool index {}", i);


			PoolData& data = _objPool[i];

			auto state = data.GetState();

			get_switch(state)
			{
			case PoolState::kActive:
			{
				//Turn into an object and then unhandle it.

				//Clear the owning data out of it.
				Destroy(data.data);

				logger::info("destroying index: {}, raw value: {}", i, data.raw);

				data.raw = 0;

				//data.type = data.refs ? ObjectDataType::kClaimed : ObjectDataType::kFree;

				//If it has references it's claimed (reserved), if not it's free.
				if (data.refs) {
					data.type = ObjectDataType::kClaimed;
				}
			}
			[[fallthrough]];
			case PoolState::kReserved:
				if (switch_value == PoolState::kReserved) {
					logger::info("idx{} was already destroyed.", i);
				}

				if (!data.refs) {
					FreePool(i);
				}
				break;

			default:
				logger::info("idx {} failed to do anything with {}", i, (int)switch_value);
			}
		}

#pragma endregion

		uint32_t GetTypeIDFromOffset(TypeOffset offset) override
		{
			return IdentityManager::instance->GetIDFromIndex(index) + offset;
		}

		ObjectData CreateData(uint32_t id = 0) override
		{
			//Currently, no data used. Later? Either Instance ID, or Type ID. One should know which they use.
			return ctor(id);
		}

		uint32_t GetPolicyID() override
		{
			return policyID;
		}

		TypeIndex GetCategoryIndex() override
		{
			return index;
		}
		std::string_view GetCategoryName() override
		{
			return category;
		}


		struct
		{
			//When you can, make pooling handling a structure that needs to be created before being used. That way every registered object 
			// doesn't use it.
			std::mutex _lock;


			std::vector<PoolData> _objPool;

			//These must both be equal or have an actual value
			uint32_t _left = -1;
			uint32_t _right = -1;

		};
		


		//Revision. Put it on the policy instead.


		//This is to be the true version of object policy basically. The above is a versioned interface.

		const std::type_info* type;//This is made with create, to denote source.
		HMODULE program;



		DataBuilder ctor;

		std::string_view category;
		//All ideas from the claimed id to  the end of the offset are a part of the registered type.
		// So think of from form all the way
		TypeIndex index;


		uint32_t policyID = -1;


	};






}