#pragma once

namespace LEX
{


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




	union ObjectData
	{
		size_t raw = 0;

		uintptr_t fstVal;		// If the value is small enough, the data can be stored directly onto the data without needing to handle a heap pointer.
		void* ptrVal;			// If the object is too large, it will either be carried around by a pointer given, or by a copy made
		uint32_t idxVal;		//smaller fast value, used with references.



		constexpr std::strong_ordering operator <=>(const ObjectData& other) const noexcept// = default;
		{return raw <=> other.raw;}


		template <typename T>
		decltype(auto) get()
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
		decltype(auto) get() const
		{
			//This has no safeties. The user should know what they're doing.

			if  constexpr (object_storage_v<T> == value_storage){	//Value Type
				return reinterpret_cast<T&>(fstVal);
			}
			else {													//Pointer type.
				return *reinterpret_cast<T*>(ptrVal);
			}
		}


		template <typename T>
		auto* ptr()
		{
			if  constexpr (object_storage_v<T> == value_storage) {	//Value Type
				return reinterpret_cast<T*>(&fstVal);
			}
			else {													//Pointer type.
				return reinterpret_cast<T*>(ptrVal);
			}
		}

		template <typename T>
		const auto* ptr() const
		{
			if  constexpr (object_storage_v<T> == value_storage) {	//Value Type
				return reinterpret_cast<T*>(&fstVal);
			}
			else {													//Pointer type.
				return reinterpret_cast<T*>(ptrVal);
			}
		}

		void Clear()
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



	using DataBuilder = ObjectData(*)(uint32_t);


	//Type will be used later to control whether something is pooled or not.	
	template <typename T>
	void FillObjectData(ObjectData& data, T* load)
	{

		using _Type = std::remove_reference_t<std::remove_const_t<T>>;



		//I may make this a concept, so it can fail at use and not just within here.
		constexpr bool declared_storage = object_storage_v<_Type>;
		constexpr bool storage_match = detail::object_storage_v<_Type> == declared_storage;



		if constexpr (declared_storage == value_storage)
		{
			static_assert(storage_match, "Declared storage is value, but type structure requires pointer.");

			reinterpret_cast<_Type&>(data) = std::move(*load);
		}
		else
		{
			if constexpr (std::is_pointer<_Type>::value)
			{
				//If data returned as a pointer
				if (load) {
					logger::debug("Load by value");
					data.ptrVal = *load;
				}
				else {
					logger::debug("Init by value");
					data.ptrVal = _Type{};
				}
			}
			else
			{
				//This is basically assuming it's not a pointer already. Deal with that.

				if (load) {
					logger::debug("Load by ptr");
					data.ptrVal = new _Type{ *load };
				}
				else {
					logger::debug("Init by ptr");
					data.ptrVal = new _Type{};
				}
			}
		}
	}

	template <typename T>
	void FillObjectData(ObjectData& data, T& load)
	{
		return FillObjectData<T>(data, &load);
	}

	//This function takes the type id or instance id of the created object as information how to qualify it's given type.
	// This can be useful for data that is sometimes, but not always pooled. If it's a template function it will be sending the instance
	// id. From there type type id is something on can claim for themselves.
	// One will simply have to know which it is.
	template <typename T>
	void BuildQualifier(ObjectData&, uint32_t) {}

	template <typename T>
	ObjectData GenericDataBuilder(uint32_t id = 0)
	{
		ObjectData data;


		//For starters, you won't want to use create type. 


		//But the problem is, how would I submit the type form and have it make Form*? I won't. Just make everything for Form*. if it's not Form*, it will
		// assume it needs to make Form. Which it isn't allowed to.

		//ALSO

		//Take this section. I think this is how I'll fill data, least mess this way.	
		T in;

		FillObjectData<T>(data, std::addressof(in));
		
		BuildQualifier<T>(data, id);

		return data;
	}





}