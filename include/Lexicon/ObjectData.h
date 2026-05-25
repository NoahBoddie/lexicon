#pragma once

#include "Lexicon/ObjectStorage.h"
#include "Lexicon/ObjectSettings.h"

namespace LEX
{
	struct TypeInfo;

	union ObjectData;

	using DataBuilder = ObjectData(*)(TypeInfo*);



	union ObjectData
	{

		template <typename T>
		static ObjectData Build(TypeInfo* type = nullptr)
		{
			T in = [&][[msvc::forceinline]] -> T
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



	public:

		constexpr ObjectData() noexcept = default;
		constexpr ObjectData(ObjectData&) noexcept = default;//I hate that I have to do this
		constexpr ObjectData(ObjectData&&) noexcept = default;
		constexpr ObjectData(const ObjectData&) noexcept = default;
		constexpr ObjectData& operator=(ObjectData&) noexcept = default;
		constexpr ObjectData& operator=(ObjectData&&) noexcept = default;
		constexpr ObjectData& operator=(const ObjectData&) noexcept = default;


		
		template <typename T>
		explicit ObjectData(T&& load)
		{
			LoadData(load);
		}


	private:
		template <typename T>
		void LoadData(T& load)
		{
			using Type = std::remove_cvref_t<T>;


			//I may make this a concept, so it can fail at use and not just within here.
			constexpr ObjectStorage declared_storage = GetObjectStorage<Type>();
			constexpr bool storage_match = detail::default_storage<Type> == declared_storage;


			//QUERY: Can this shit not use the get functions from ObjectData?
			if constexpr (declared_storage == ObjectStorage::Value)
			{
				static_assert(storage_match, "Declared storage is value, but type structure requires pointer.");

				reinterpret_cast<Type&>(fstVal) = std::move(load);
			}
			else
			{
				if constexpr (std::is_pointer<Type>::value) {
					ptrVal = std::move(load);
				}
				else {
					ptrVal = new Type{ std::move(load) };
				}
			}
		}

	public:




		size_t raw = 0;

		mutable uintptr_t fstVal;		// If the value is small enough, the data can be stored directly onto the data without needing to handle a heap pointer.
		mutable void* ptrVal;			// If the object is too large, it will either be carried around by a pointer given, or by a copy made
		uint32_t idxVal;		//smaller fast value, used with references.



		constexpr std::strong_ordering operator <=>(const ObjectData& other) const noexcept// = default;
		{return raw <=> other.raw;}


		template <typename T>
		decltype(auto) get() noexcept
		{
			//This has no safeties. The user should know what they're doing.

			if  constexpr (GetObjectStorage<T>() == ObjectStorage::Value) {	//Value Type
				return reinterpret_cast<T&>(fstVal);
			}
			else {													//Pointer type.
				return *reinterpret_cast<T*>(ptrVal);
			}
		}

		template <typename T>
		decltype(auto) get() const noexcept
		{
			//This has no safeties. The user should know what they're doing.
			//This should probably be making sure not to give someone the const ref of it OR
			// just give them a new object that isn't a const but isn't a ref.
			if  constexpr (GetObjectStorage<T>() == ObjectStorage::Value){	//Value Type
				return reinterpret_cast<T&>(fstVal);
			}
			else {													//Pointer type.
				return *reinterpret_cast<T*>(ptrVal);
			}
		}


		template <typename T>
		auto* ptr() noexcept
		{
			if  constexpr (GetObjectStorage<T>() == ObjectStorage::Value) {	//Value Type
				return reinterpret_cast<T*>(&fstVal);
			}
			else {													//Pointer type.
				return reinterpret_cast<T*>(ptrVal);
			}
		}

		template <typename T>
		const auto* ptr() const noexcept
		{
			if  constexpr (GetObjectStorage<T>() == ObjectStorage::Value) {	//Value Type
				return reinterpret_cast<T*>(&fstVal);
			}
			else {													//Pointer type.
				return reinterpret_cast<T*>(ptrVal);
			}
		}

		void Clear() noexcept
		{
			fstVal = 0;
		}
	};


	enum struct ObjectDataType : int8_t
	{//Want to name storage type. More of my shit getting in the way though.
		//Enforce a default of None please.
		kNone = -1,		//Invalid/uninitialized storage type.
		kValue,			//Storage is kept within the bytes of the object.
		kPointer,		//Storage is kept on a pointer the object holds.
		kReference,		//Storage is pooled, with the object storing a pointer to the data.

		kVal = kValue,
		kPtr = kPointer,
		kRef = kReference,

		kClaimed = kNone,	//When in pool data, this represents that the pool isn't free despite holding no data.
		kFree = kReference,	//When in pool data, this instead represents that a pool as been allocated.
	};



	
}