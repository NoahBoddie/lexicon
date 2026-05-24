#pragma once

namespace LEX
{
	struct TypeInfo;

	//Object storage slated for relocation
	namespace detail
	{
		struct inherent {};


		template <typename T>
		struct object_storage : public inherent
		{
			//This is the purest version of this, rather the raw one. Used within make object to find if a forced storage is antithetical to 
			// it's design.

			//may want to decay this shit btw.
			//Ensures it's the same size as a pointer basically.
			static constexpr bool value = !std::is_polymorphic_v<T> && std::is_trivially_copyable_v<T> && sizeof(T) <= (sizeof(void*));// && not_polymorphic?

			//The detection of this will help determine if the object storage was choosen manually or not. Lacking this will mean it's been manually choosen.
			// ACTUALLY, make this something that it derives from instead. Easier to deal with it not existing.
			static constexpr bool inherent = true;
		};


		template <typename T>
		constexpr bool object_storage_v = object_storage<T>::value;
	};

	
	template <typename T>
	struct object_storage : detail::object_storage<T> {};

	//name pending
	struct StaticStoreType
	{
		const bool value;

		constexpr operator bool() const noexcept
		{
			return value;
		}

		template<typename T>
		constexpr StaticStoreType(object_storage<T>) noexcept : value{ object_storage<T>::value }
		{

		}
	};


	//template <typename T>
	//constexpr bool object_storage_v = object_storage<T>::value;

	template <typename T>
	constexpr StaticStoreType object_storage_v = StaticStoreType(object_storage<T>{});

	constexpr bool value_storage = true;
	constexpr bool pointer_storage = false;

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

		template <typename T>
		explicit ObjectData(T& load)// : ObjectData{ std::addressof(load) }
		{
			LoadData(std::addressof(load));
		}

		template <typename T>
		explicit ObjectData(T&& load)// : ObjectData{ std::addressof(load) }
		{
			LoadData(std::addressof(load));
		}


	private:
		template <typename T>
		void LoadData(T* load)
		{
			using _Type = std::remove_cvref_t<T>;


			//I may make this a concept, so it can fail at use and not just within here.
			constexpr bool declared_storage = object_storage_v<_Type>;
			constexpr bool storage_match = detail::object_storage_v<_Type> == declared_storage;


			//QUERY: Can this shit not use the get functions from ObjectData?
			if constexpr (declared_storage == value_storage)
			{
				static_assert(storage_match, "Declared storage is value, but type structure requires pointer.");

				reinterpret_cast<_Type&>(*this) = std::move(*load);
			}
			else
			{
				if constexpr (std::is_pointer<_Type>::value)
				{
					//If data returned as a pointer
					if (load) {
						ptrVal = *load;
					}
					else {
						ptrVal = _Type{};
					}
				}
				else
				{
					//This is basically assuming it's not a pointer already. Deal with that.

					if (load) {
						ptrVal = new _Type{ *load };
					}
					else {
						ptrVal = new _Type{};
					}
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

			if  constexpr (object_storage_v<T> == value_storage) {	//Value Type
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
			if  constexpr (object_storage_v<T> == value_storage){	//Value Type
				return reinterpret_cast<T&>(fstVal);
			}
			else {													//Pointer type.
				return *reinterpret_cast<T*>(ptrVal);
			}
		}


		template <typename T>
		auto* ptr() noexcept
		{
			if  constexpr (object_storage_v<T> == value_storage) {	//Value Type
				return reinterpret_cast<T*>(&fstVal);
			}
			else {													//Pointer type.
				return reinterpret_cast<T*>(ptrVal);
			}
		}

		template <typename T>
		const auto* ptr() const noexcept
		{
			if  constexpr (object_storage_v<T> == value_storage) {	//Value Type
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