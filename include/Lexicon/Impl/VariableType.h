#pragma once


//The name doesn't make as much sense because interfaces are called objects but this works for all types..

//Revision. I want to change the name (again). Maybe TypeInterface, idk

namespace LEX
{
	class ITypePolicy;
	class AbstractTypePolicy;
	
	//tag to tell tha this is being used by a single type.
	struct single_type {};

	namespace detail
	{
		struct not_implemented {};
		
		
		//move example
		struct example {};
	}

	
	//This is storageType
	template <typename T>
	struct StorageType : public detail::not_implemented
	{
		//instead of void, I can perhaps use a different type.
		//No operator as we can see.

		//I still don't like this a ton because you need to implement the function still

	private:
		ITypePolicy* operator()()
		{

		}
	};

	//And this is value type.
	template <typename T>
	struct ValueType : public detail::not_implemented
	{
#pragma region a spiel
		//this one is a bit special dig? See I want to decay whatever T is so there aren't any references, pointers
		// of the type there, though arrays are allowed. For this I think I'll make my own type trait.

		//A key thing of note is this is manually implemented, so one can say, have the T be TESForm, but the
		// operator will use TESForm*.
		// So when searching for the variable type class to use, what it should be done is these things should be decayed
		// away. 
		//Specifically as a reminder, pointer types (not just c pointers), constness etc.

		//Within the concept, it's required that the argument that carries the evaluation either be a pointer or reference,
		// as the creation of new objects shouldn't be allowed here. (unless by chance the object is small/trivally copiable
		// which would be a slight optimization.

		//Additionally, it's notable that the operator but either be a const pointer or const reference of the type
		// it's trying to use. Just to make sure that no setting ever happens.



		//More notable settings. If something is considered nullable that's when return type is needed.
		// If one never encounters a nullable one you may never need to define the return type. However,
		// slight issue with this is that what if something is nullable

		//SO maybe to control this is a possible boolean comparison to std::nullptr_t? And that will reenable the
		// ReturnType query.


		//Also if something has a ReturnType but no VariableType that should be able to run that, but perhaps
		// with a warning. However if you have some tag on the class some how you can prevent it from doing that.


		//Maybe for both concepts you can have it only use a base class to signify you will not need the other one.
#pragma endregion
	private:
		//This should never be used manually, so private so just in case it actually is used it will throw 
		// a compiler error.
		AbstractTypePolicy* operator()(const T*)
		{
			//static_assert
			return nullptr;
		}
	};


	//Examples
#pragma region Examples
	//Note: Remove static_assert and "using T" in implementation.
	
	template <>
	struct StorageType<detail::example>
	{
		using T = detail::example;

		//Getting the ReturnType returns a ITypePolicy, an interface of a type, as a single C++ type could possibly
		// be able to represent a generic imaginary object.

		ITypePolicy* operator()()
		{
			static_assert(std::is_same_v<T, T>, "Example ReturnType<detail::example> cannot be used.");
		}
		
	};
	
	template <>
	struct ValueType<detail::example>
	{
		using T = detail::example;
		
		//Getting the VariableType must return an AbstractTypePolicy, meaning that it's connected to a real
		// instantiable type. As it's arguments it takes either a type that passes the "pointer_type" constraints
		// or is a reference and must always be passed const to prevent data manipulation or access data creation.


		AbstractTypePolicy* operator()(const detail::example*)
		{
			static_assert(std::is_same_v<T, T>, "Example VariableType<detail::example> cannot be used.");
		}

		AbstractTypePolicy* operator()(const detail::example&)
		{
			static_assert(std::is_same_v<T, T>, "Example VariableType<detail::example> cannot be used.");
		}
	};

#pragma endregion

	namespace detail
	{
		template<typename T> concept ret_impl = !std::is_base_of_v<detail::not_implemented, StorageType<T>> &&
			!std::is_same_v<StorageType<example>, StorageType<T>>;

		template<typename T> concept var_impl = !std::is_base_of_v<detail::not_implemented, ValueType<T>> &&
			!std::is_same_v<ValueType<example>, ValueType<T>>;
		
		//
		template<typename T> concept obj_impl = ret_impl<T> && var_impl<T>;



		//I will specify how this works later. Right now? Direct as shit.


		//About this decay, I'm thinking that primarily you'll see a wrapper function used, a detail function
		// that can handle the decay somehow.
		//using Type = 


		//the idea of this function is that it needs explicit specialization, or the existence of a certain function within the type to be used.
		// Otherwise, it will return null, or perhaps the void type.

		template<typename T>
		using custom_decay = T;//std::decay_t<T>;//
	}
	

	template <detail::var_impl T>
	AbstractTypePolicy* GetVariableType(detail::custom_decay<T>& arg)
	{
		using _T = detail::custom_decay<T>;

		
		if constexpr (false)
		{
			//If type specified internal function (not implemented yet)
			return nullptr;
		}
		else
		{
			return ValueType<_T>{}(arg);
		}
		
	}

	template <detail::ret_impl T>
	ITypePolicy* GetReturnType(detail::custom_decay<T>& arg)
	{
		using _T = detail::custom_decay<T>;


		if constexpr (false)
		{
			//If type specified internal function (not implemented yet)
			return nullptr;
		}
		else
		{
			return StorageType<_T>{}(arg);
		}

	}

	//This should be called GetVariablePolicy or GetVariableType
	template <detail::obj_impl T>
	ITypePolicy* GetTypePolicy(detail::custom_decay<T>& arg)
	{
		using _T = detail::custom_decay<T>;


		//Unsure if I will actually use this, or in what way.

		if constexpr (false)
		{
			//If type specified internal function (not implemented yet)
			return nullptr;
		}
		else
		{
			if (auto policy = GetVariableType<_T>(arg); !policy)//or if the policy is the policy of none
				return GetReturnType<_T>(arg);
		}

	}
}