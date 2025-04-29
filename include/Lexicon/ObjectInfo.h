#pragma once

#include "ObjectData.h"
#include "TypeID.h"

#include "Lexicon/String.h"
//*src
#include "Lexicon/TypeInfo.h"

#include "Lexicon/Interfaces/IdentityManager.h"

namespace LEX
{
	struct Object;
	struct ObjectPolicy;

	
	struct TypeInfo;

	struct String;

	//Please use ObjectData&. There's never a situation where we won't send it, and ObjectData will never be in need of sending "this".

	//place in detail.
	struct IObjectVTable
	{

		//*



		//The idea of this is that the policy stores it, and is accessed every single time an operator has to be used on an object.
		// So basically, this is the virtual table.

		//If a given ObjectInfo doesn't contain this, it's invalid, and the given object cannot be made into an object
		virtual ~IObjectVTable() = default;

		virtual ObjectPolicy* GetObjectPolicy() = 0;

		//Used to tell the individual objects version. Useful if the plugin doesn't change, but what's targeted does.
		virtual uintptr_t GetObjectVersion() = 0;
		
		//This should exist as a seperation between Vtable version and the other.
		// The info version is what the user implements. The VTable version is what I've added onto the vtables. Basically, VTable
		// will be a final virtual function.
		//virtual uintptr_t GetInfoVersion() = 0;

		//Used to tell what version the class is. Helps detect when new functions are added.
		virtual uintptr_t GetVTableVersion() = 0;


		//TODO: IsCompatible should be using a different struct, specifically the one that tells which item is incompatible with which.
		//This function is used to check the object versions of this and that to tell if it's valid to use. Most times it will be.
		// Also of note, there should be an outer compatibility check, one that calls both versions of IsCompatible. This accounts for updated
		// compatibility rules.
		virtual bool IsCompatible(const IObjectVTable*) = 0;

		//Determines if it's stored in a value or in a pointer. Do NOT make this dynamic,
		virtual StaticStoreType GetStorageType() = 0;

		//This will ALWAYS be given the actual objects data. It's then upto get_storage_type to handle it.
		virtual bool IsPooled(ObjectData&) = 0;

		//Other things that can help with confirming validity, size. if the size is different, definitely incompatible.

		//IMPORTANT.
		//I think either I need a new struct to handle the concept of what this expects. Main problem is I don't want
		// a fake ass object doing things that it's not supposed to do. Maybe I'll make a tag for that.
		//ACTUALLY, just send the object data. It can reliably be turned into whichever by relying on it's assigned constexpr stored type.
		// If the type ends up changing, that's one of the things addressed in the compatibility check function.

		virtual void Destroy(ObjectData&) = 0;

		virtual void Copy(ObjectData&, const ObjectData&) = 0;

		virtual void Move(ObjectData&, ObjectData&) = 0;

		//Launched when the target is first inducted into an object.
		virtual void Initialize(ObjectData&) = 0;

		//TODO: Not doing Object::Operate this yet, but figure out what you want to do with it before releasing.
		//virtual void Operator(std::string_view, ObjectData&, RuntimeVariable&) = 0;



		//Used to invalidate occupying data, possibly requesting destruction.
		virtual bool Exists(ObjectData&) = 0;

		virtual std::partial_ordering Compare(ObjectData&, ObjectData&) = 0;

		virtual bool RequestDelete(ObjectData&) = 0;

		virtual TypeOffset GetTypeOffset(ObjectData&) = 0;

		virtual TypeInfo* SpecializeType(ObjectData&, ITypeInfo*) = 0;

		//This can be defined in a source
		virtual uint32_t GetTypeID(ObjectData&) = 0;

		//Non-virtual
		ITypeInfo* GetTypeInterface(ObjectData& object)
		{
			//Note, not real code (yet)
			auto id = GetTypeID(object);

			return IdentityManager::instance->GetTypeByID(id);
		}

		TypeInfo* GetTypeResolved(ObjectData& object)
		{
			auto type = GetTypeInterface(object);

			return SpecializeType(object, type);
		}

		//Gets the objects print string. Comes with context for types such as bind classes that attach themselves to an object.
		virtual String PrintString(ObjectData& object, std::string_view context) = 0;

		//*/

	};

	struct ObjectVTable : public IObjectVTable
	{
		

		//*
		ObjectPolicy* GetObjectPolicy() override { return _policy; }


		uintptr_t GetObjectVersion() override { return 0; }
		uintptr_t GetVTableVersion() override { return 0; }
		
		virtual void Initialize(ObjectData&) override {}
		
		virtual bool IsCompatible(const IObjectVTable*) override { return true; }
		
		virtual bool Exists(ObjectData&) override
		{
			//By default, this will be true most times, this is u
			return true;
		}

		//by default most objects aren't going to be pooled
		virtual bool IsPooled(ObjectData&) override
		{
			return false;
		}

		virtual bool RequestDelete(ObjectData& self) override
		{
			//by default, most are objects are expected trivally deletable. This calls destroy, and then leaves.

			Destroy(self);
			return true;
		}

		uint32_t GetTypeID(ObjectData& data) override API_FINAL;

		TypeInfo* SpecializeType(ObjectData&, ITypeInfo* type) override
		{
			//By default a specialized class will be as normal. No additional work will need to be used.

			if (!type)
				report::critical("Cannot specialize null type");

			return type->GetTypePolicy((ITemplateBody*)nullptr);
		}

		


		String PrintString(ObjectData& self, std::string_view context) override
		{


			//Later I'd like this to be able to get the name. Saving that for later though.
			return std::format("{}::({:X})", context.empty() ? "Object" : context, self.fstVal);
		}


		//*/
	INTERNAL:

		void SetPolicy(ObjectPolicy* policy)
		{
			if (!_policy)
				_policy = policy;
		}

		//This policy is the only 
		mutable ObjectPolicy* _policy = nullptr;
	};
	
	//IOVT is the version that's used on policies, and the above is the version that's found in object info.



	template <typename T>
	struct ObjectInfo : LEX::detail::not_implemented
	{

	};

	template <typename T>
	struct QualifiedObjectInfo : public ObjectVTable
	{
		using Type = T;

		//A qualified version of object info for your object info to derive from. Autofill some type based virtual functions, making it easier to deal with.

		//A few of these should be final
		
		//make const
		StaticStoreType GetStorageType() override final
		{
			return object_storage_v<T>;
		}

		void Destroy(ObjectData& self) override
		{
			if constexpr (std::is_pointer_v<T>)
			{
				logger::debug("destroying {} pointer", GetTypeName<T>());
			}
			else if constexpr (object_storage_v<T> == value_storage) {
				//Value types simply need to unhandle their values.
				logger::debug("destroying {} value", GetTypeName<T>());
				self.get<T>().~T();
			}
			else{
				//while pointer types will need to deallocate
				logger::debug("destroying {} reference", GetTypeName<T>());
				delete self.ptr<T>();
			}
		}

		//Use other things.
		//Copy and this are basically exactly the same.
		void Copy(ObjectData& self, const ObjectData& other) override
		{
			self.get<T>() = other.get<T>();
		}

		void Move(ObjectData& self, ObjectData& other) override
		{
			self.get<T>() = std::move(other.get<T>());
		}


		//I'll save this for later, but I'd like it to be made
		std::partial_ordering Compare(ObjectData& self, ObjectData& other) override
		{
			if constexpr (std::three_way_comparable<T>)
			{
				return self.get<T>() <=> other.get<T>();
			}
			else
			{
				if constexpr (std::totally_ordered<T>)
				{
					if (self.get<T>() < other.get<T>())
						return std::partial_ordering::greater;

					if (self.get<T>() > other.get<T>())
						return std::partial_ordering::lesser;
				}

				if constexpr (std::equality_comparable<T>) {
					if (self.get<T>() == other.get<T>())
						return std::partial_ordering::equivalent;

				}

				return std::partial_ordering::unordered;
			}
		}


		String PrintString(ObjectData& self, std::string_view context) override
		{
			constexpr bool has_func = requires(const T& t, std::string_view s)
			{
				{ t.PrintString(s) } -> std::convertible_to<String>;
			};

			if constexpr (has_func)
			{
				return self.get<T>().PrintString(context);
			}
			else
			{
				return __super::PrintString(self, context);
			}
		}


		//it would be neat if this would make it so object data no longer had to be used by obscuring the old versions of the functions
		// and calling new ones. 
	};


	


	template <typename T>
	concept has_object_info = std::is_base_of_v<ObjectVTable, ObjectInfo<std::remove_cvref_t<T>>> &&
		!std::is_abstract_v<ObjectInfo<std::remove_cvref_t<T>>>;


	template <has_object_info T>
	ObjectVTable* GetObjectInfo()
	{
		//Having this not be loose is a good sign of warning.
		//using PureT = std::remove_cvref_t<T>;

		static ObjectVTable* vtable = new ObjectInfo<T>{};

		return vtable;
	}

}

