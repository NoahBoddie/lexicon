#pragma once

//TODO: This is in a some what dire need of a redesign I personally believe.


//The name doesn't make as much sense because interfaces are called objects but this works for all types..

//Revision. I want to change the name (again). Maybe TypeInterface, idk

namespace LEX
{
	class ITypePolicy;
	class AbstractTypePolicy;
	

	namespace detail
	{
		//move example
		struct example
		{
			//The storage type function can be defined like this.
			// Function must be static, have no parameters, and return ITypePolicy.
			static AbstractTypePolicy* GetStorageType()
			{
				return {};
			}



			//As the value type function can be defined like this,
			// Requiring the function to be const and membered (virtual allowed), no parameters, and return AbstractTypePolicy.
			AbstractTypePolicy* GetValueType() const
			{
				return {};
			}

			//Additionally, both storage and value can be defined like this. This denotes that the value type does not change based on runtime 
			// value. This will take precedence over other declared variables.
			// It's required the function must be static and return AbstractTypePolicy.
			static AbstractTypePolicy* GetVariableType()
			{
				return {};
			}

			//This is also a viable declaration for variable types that have differences between static and not static
			static AbstractTypePolicy* GetVariableType(const example*)
			{
				return {};
			}
		};
	}


	namespace detail
	{
		//These will need supplemental version
		template<typename T> concept subject_has_var_type_Store = requires()
		{
			{ T::GetVariableType() } -> pointer_derived_from<AbstractTypePolicy*>;
		};

		template<typename T> concept subject_has_var_type_Value = requires(const T * t)
		{
			{ T::GetVariableType(t) } -> pointer_derived_from<AbstractTypePolicy*>;
		};

		template<typename T> concept subject_has_var_type = subject_has_var_type_Value<T> || subject_has_var_type_Store<T>;

	}






	template <typename T, typename = void>
	struct VariableType : public detail::not_implemented
	{

		//This should never be used manually, so private so just in case it actually is used it will throw 
		// a compiler error.

		/*
		AbstractTypePolicy* operator()()
		{
			//static_assert
			return nullptr;
		}

		AbstractTypePolicy* operator()(const T*)
		{
			//static_assert
			return nullptr;
		}
		//*/
	};



	//This is the non implemented version of GetStorageType
	template<typename T>
	detail::not_implemented FetchVariableType(const accepts_all, detail::not_implemented = {})
	{
		static_assert(!std::is_same_v<T, T>, "Unimplemented version of FetchVariableType called.");
		return {};
	}

	//This is the implemented version of these functions. They need to pass a concept that states
	// the function itself implements what's required.


	template<detail::subject_has_var_type T>
	AbstractTypePolicy* FetchVariableType(const T* arg = nullptr)
	{
		if constexpr (detail::subject_has_var_type_Value<T>) {
			return T::GetVariableType(arg);
		}
		else {
			return T::GetVariableType();
		}
	}

	///////////////////////////////////////////////


	//To be defined later, this should be a concept that declares GetStorageType and GetValueType need to have an implementation
	// for each to be used.
	struct TBDL {};


	template<>
	struct VariableType<TBDL>
	{

		AbstractTypePolicy* operator()()
		{

		}

		AbstractTypePolicy* operator()(const TBDL*)
		{

		}
	};

	namespace detail
	{
		template<typename T> concept function_has_var_type_Value = requires(const T* t)
		{
			{ FetchVariableType<T>(t) } -> std::same_as<AbstractTypePolicy*>;
		};
		template<typename T> concept function_has_var_type_Store = requires()
		{
			{ FetchVariableType<T>() } -> std::same_as<AbstractTypePolicy*>;
		};

		template<typename T> concept function_has_var_type = function_has_var_type_Value<T> || function_has_var_type_Store<T>;
	}


	template <detail::function_has_var_type T>
	struct VariableType<T>
	{
		AbstractTypePolicy* operator()(const T* arg)
		{
			if constexpr (detail::function_has_var_type_Value<T>) {
				return FetchVariableType<T>(arg);
			}
			else {
				return FetchVariableType<T>();
			}
		}

		//Can likely remove this
		AbstractTypePolicy* operator()()
		{
			return this->operator()(nullptr);
		}

	};





	//These are spotty right now. Please redo.
	namespace detail
	{
		template<typename T> concept call_class_has_var_type_Store = !std::is_base_of_v<not_implemented, VariableType<T>>&&
			//!std::is_same_v<StorageType<example>, StorageType<T>> && 
			requires()
		{
			{ VariableType<T>{}.operator()() } -> pointer_derived_from<AbstractTypePolicy*>;
		};

		template<typename T> concept call_class_has_var_type_Value = !std::is_base_of_v<not_implemented, VariableType<T>>&&
			//!std::is_same_v<ValueType<example>, ValueType<T>> && 
			requires(const T* t)
		{
			{ VariableType<T>{}.operator()(t) } -> pointer_derived_from<AbstractTypePolicy*>;
		};


		template<typename T> concept call_class_has_var_type = call_class_has_var_type_Store<T> || call_class_has_var_type_Value<T>;


		template<typename T>
		using custom_decay = T;//std::decay_t<T>;//
	}




	//When using a reference it will only get the value type. When getting it via pointer it may give you the storage type if null.
	//Direct functions can be used if one so chooses.

	//<!> Note, these args shouldn't be using custom T, instead that should onl be used to load the next GetValue/Storage function. 
	// This is so that the explicit specialization doesn't use the wrong type



	template <detail::call_class_has_var_type T>
	AbstractTypePolicy* GetVariableType_()
	{
		using Decay_T = detail::custom_decay<T>;

		if constexpr (detail::call_class_has_var_type_Value<T>) {
			return VariableType<Decay_T>{}(nullptr);
		}
		else {
			return VariableType<Decay_T>{}();
		}

		

	}

	template <detail::call_class_has_var_type T>
	AbstractTypePolicy* GetVariableType_(detail::custom_decay<T>* arg)
	{
		using Decay_T = detail::custom_decay<T>;


		AbstractTypePolicy* type = nullptr;

		//I actually think this might be a little off, cause it's double dipping or some shit. But fuck it we ball I guess.

		if constexpr (detail::call_class_has_var_type_Value<T>) {
			if (arg) {
				return VariableType<Decay_T>{}(arg);
			}
		}

		if constexpr (detail::call_class_has_var_type_Store<T>) {
			if (!type) {
				type = GetVariableType_<Decay_T>();
			}
		}

		return type;

	}


	template <detail::call_class_has_var_type T>
	AbstractTypePolicy* GetVariableType_(detail::custom_decay<T>& arg)
	{
		using _T = detail::custom_decay<T>;


		return GetVariableType_<_T>(&arg);
	}









	//At a later point a single function and probably handle all of these.

	class Number;
	struct Delegate;
	struct Object;
	struct FunctionHandle;
	struct Array;
	class Variable;
	struct String;
	
	

	template <>
	struct VariableType<Void>
	{
		AbstractTypePolicy* operator()(const Void*);
	};


	

	template <>
	struct VariableType<double>
	{

		AbstractTypePolicy* operator()();
	};

	//This is how I'll prefer to get variable types.
	// GetVariableType becomes FetchVariableType, vice versa
	// Newly named GetVariableType will instead use FetchVariableType, which will either want a VariableType struct
	// or a GetVariableType function within the type itself.
	/*
	template <std::integral T>
	struct VariableType<T>
	{

		AbstractTypePolicy* operator()() { return nullptr; }
	};
	//*/
	
	//template class VariableType<Number>;
	
	//template AbstractTypePolicy* VariableType<Number>::operator()(const Number*);
	
	/*
	 
	//x
	template <>
	struct VariableType<Number>
	{
		inline AbstractTypePolicy* operator()(const Number* it);
	};
	
	//x
	template <>
	struct VariableType<String>
	{
		AbstractTypePolicy* operator()(const String*);
	};

	//no
	template <>
	struct VariableType<Delegate>
	{
		AbstractTypePolicy* operator()(const Delegate*);
	};

	//x
	template <>
	struct VariableType<Object>
	{
		AbstractTypePolicy* operator()(const Object*);
	};


	//no
	template <>
	struct VariableType<FunctionHandle>
	{
		AbstractTypePolicy* operator()(const FunctionHandle*);
	};
	//x
	template <>
	struct VariableType<Array>
	{
		AbstractTypePolicy* operator()(const Array*);
	};
	//x
	template <>
	struct VariableType<Variable>
	{
		AbstractTypePolicy* operator()(const Variable*);
	};


	void TestFunc();
	//*/
}