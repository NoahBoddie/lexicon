#pragma once

#include "ObjectData.h"
#include "TypeID.h"

#include "Lexicon/String.h"
#include "Lexicon/ObjectSettings.h"
//*src
#include "Lexicon/TypeInfo.h"
#include "Lexicon/ObjectParams.h"

#include "Lexicon/Interfaces/IdentityManager.h"

namespace LEX
{
	struct Object;
	struct ObjectPolicy;

	
	struct TypeInfo;

	struct String;

	struct IObjectInfo;

	struct ObjectParams;


	using ObjLitCtor = Object(*)(std::string_view);


	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(IObjectInfo)
			{
				//The idea of this is that the policy stores it, and is accessed every single time an operator has to be used on an object.
				// So basically, this is the virtual table.

				//If a given ObjectInfo doesn't contain this, it's invalid, and the given object cannot be made into an object

				virtual ObjectPolicy* GetObjectPolicy() = 0;

				//Used to tell the individual objects version. Useful if the plugin doesn't change, but what's targeted does.
				virtual uintptr_t GetObjectVersion() = 0;


				//TODO: IsCompatible should be using a different struct, specifically the one that tells which item is incompatible with which.
				//This function is used to check the object versions of this and that to tell if it's valid to use. Most times it will be.
				// Also of note, there should be an outer compatibility check, one that calls both versions of IsCompatible. This accounts for updated
				// compatibility rules.
				virtual bool IsCompatible(uintptr_t) = 0;

				//Determines if it's stored in a value or in a pointer. Do NOT make this dynamic,
				virtual ObjectStorage GetStorage() = 0;

				//This will ALWAYS be given the actual objects data. It's then upto get_storage_type to handle it.
				virtual bool IsPooled(TypeInfo*) = 0;

				//Other things that can help with confirming validity, size. if the size is different, definitely incompatible.

				//IMPORTANT.
				//I think either I need a new struct to handle the concept of what this expects. Main problem is I don't want
				// a fake ass object doing things that it's not supposed to do. Maybe I'll make a tag for that.
				//ACTUALLY, just send the object data. It can reliably be turned into whichever by relying on it's assigned constexpr stored type.
				// If the type ends up changing, that's one of the things addressed in the compatibility check function.

				virtual void Destroy(ObjectData&) = 0;

				virtual void Copy(ObjectData&, const ObjectData&) = 0;

				virtual void Move(ObjectData&, ObjectData&) = 0;

				virtual ObjectData Build(TypeInfo* type) = 0;
				//Launched when the target is first inducted into an object.
				virtual void Initialize(ObjectData&, TypeInfo*) = 0;

				//TODO: Not doing Object::Operate this yet, but figure out what you want to do with it before releasing.
				//virtual void Operator(std::string_view, ObjectData&, RuntimeVariable&) = 0;



				//Used to invalidate occupying data, possibly requesting destruction.
				virtual bool Exists(ObjectData&) = 0;

				virtual std::partial_ordering Compare(ObjectData&, ObjectData&) = 0;

				virtual bool RequestDelete(ObjectData&) = 0;

				virtual TypeOffset GetTypeOffset(const ObjectParams&) = 0;

				virtual TypeInfo* SpecializeType(const ObjectParams&, ITypeInfo*) = 0;

				//This can be defined in a source
				virtual uint32_t GetTypeID(const ObjectParams&) = 0;

				virtual TypeInfo* GetOverrideType(const ObjectParams&) = 0;

				//Gets the objects print string. Comes with context for types such as bind classes that attach themselves to an object.
				virtual String PrintString(const ObjectParams& object, std::string_view context) = 0;

				virtual bool CreateLiteralData(std::string_view literal, uintptr_t& hash, ObjLitCtor& ctor) = 0;

				virtual TypeOffset GetOffsetFromArgs(const std::string_view& category, const std::span<std::string_view>& args) = 0;
				//*/

			};
		}


		CURRENT_VERSION(IObjectInfo, 1);
	}

	struct IMPL_VERSION(IObjectInfo)
	{

		uintptr_t GetObjectVersion() override { return 0; }
		uintptr_t GetInfoVersion() { return Version(); }
		
		void Initialize(ObjectData&, TypeInfo*) override {}
		
		virtual bool IsCompatible(uintptr_t) override { return true; }
		
		virtual bool Exists(ObjectData&) override
		{
			//By default, this will be true most times, this is u
			return true;
		}

		//by default most objects aren't going to be pooled
		virtual bool IsPooled(TypeInfo*) override
		{
			return false;
		}

		virtual bool RequestDelete(ObjectData& self) override
		{
			//by default, most are objects are expected trivally deletable. This calls destroy, and then leaves.

			Destroy(self);
			return true;
		}

		uint32_t GetTypeID(const ObjectParams& data) override API_FINAL;

		TypeInfo* SpecializeType(const ObjectParams&, ITypeInfo* type) override
		{
			//By default a specialized class will be as normal. No additional work will need to be used.

			if (!type)
				report::critical("Cannot specialize null type");

			return type->GetTypeInfo(nullptr);
		}

		


		String PrintString(const ObjectParams& self, std::string_view context) override
		{


			//Later I'd like this to be able to get the name. Saving that for later though.
			return std::format("{}::({:X})", context.empty() ? "Object" : context, self.data.fstVal);
		}

		bool CreateLiteralData(std::string_view literal, uintptr_t& hash, ObjLitCtor& ctor) override
		{
			return false;
		}

		//Very ill-advised you use this, primarily only exists for ScriptObjects and attributes
		TypeInfo* GetOverrideType(const ObjectParams&) override
		{
			return nullptr;
		}

		ITypeInfo* GetTypeInterface(const ObjectParams& object)
		{
			if (auto type = GetOverrideType(object)) {
				return type;
			}

			//Note, not real code (yet)
			auto id = GetTypeID(object);

			return IdentityManager::instance->GetTypeByID(id);
		}


		ObjectData CreateData(TypeInfo* type = nullptr)
		{
			ObjectData data = Build(type);
			Initialize(data, type);

			return data;
		}


		TypeInfo* GetTypeResolved(const ObjectParams& object)
		{
			if (auto type = GetOverrideType(object)) {
				return type;
			}

			auto type = GetTypeInterface(object);

			return SpecializeType(object, type);
		}

		TypeOffset GetOffsetFromArgs(const std::string_view& category, const std::span<std::string_view>& args) override
		{
			//This remains unused unless someone manually overrides it.
			return -1;
		}

		//*/

	};
	



	//IOVT is the version that's used on policies, and the below is the version that's found in object info.
	struct ObjectInfoBase : public IObjectInfo
	{
		ObjectPolicy* GetObjectPolicy() override { return _policy; }


	INTERNAL:

		void SetPolicy(ObjectPolicy* policy)
		{
			assert_if_not(!_policy)
				_policy = policy;
		}

		//This policy is the only 
		mutable ObjectPolicy* _policy = nullptr;
	};


	template <typename T>
	struct ObjectInfo : public ObjectInfoBase
	{
		using Type = T;

		//A qualified version of object info for your object info to derive from. Autofill some type based virtual functions, making it easier to deal with.

		//A few of these should be final
		

		static const T& get(const ObjectData& self)
		{
			return self.get<T>();
		}


		static T& get(ObjectData& self)
		{
			return unconst(get(make_const(self)));
		}


		static T* ptr(ObjectData& self)
		{
			return self.ptr<T>();
		}

		static const T* ptr(const ObjectData& self)
		{
			return ptr(unconst(self));
		}


		
		ObjectStorage GetStorage() override final
		{
			return GetObjectStorage<T>();
		}
		

		ObjectData Build(TypeInfo* type) override final
		{
			T in = [&] [[msvc::forceinline]] -> T
			{
				if constexpr (std::is_default_constructible_v<T>) {
					return T{};
				}
				else if constexpr (std::is_constructible_v<T, TypeInfo*>) {
					T{ type };
				}
				else {
					static_assert(!std::is_same_v<T, T>, "Object type T cannot be constructed by default or Type.");
					std::unreachable();
				}

			}();

			ObjectData data{ in };

			return data;
		}


		void Destroy(ObjectData& self) override
		{
			
			if constexpr (GetObjectStorage<T>() == ObjectStorage::Value) {
				//Value types simply need to unhandle their values.
				if constexpr (!std::is_pointer_v<T>){
					get(self).~T();
				}
			}
			else{
				//while pointer types will need to deallocate
				delete ptr(self);
			}
		}

		//Use other things.
		//Copy and this are basically exactly the same.
		void Copy(ObjectData& self, const ObjectData& other) override
		{
			get(self) = get(other);
		}

		void Move(ObjectData& self, ObjectData& other) override
		{
			get(self) = std::move(get(other));
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


		String PrintString(const ObjectParams& self, std::string_view context) override
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

		bool Exists(ObjectData& self) override
		{
			if constexpr (requires(ObjectData& it) { { it.get<T>() } ->std::convertible_to<bool>; }) {
				return self.get<T>();
			}
			else {
				return IObjectInfo::Exists(self);
			}
		}


		//it would be neat if this would make it so object data no longer had to be used by obscuring the old versions of the functions
		// and calling new ones. 
	};



	template <typename T>
	concept setting_is_object_info = has_object_info<T> &&
		std::derived_from<ObjectSettings<std::remove_cvref_t<T>>, ObjectInfo<std::remove_cvref_t<T>>> &&
		!std::is_abstract_v<ObjectSettings<std::remove_cvref_t<T>>>;
}

