#pragma once

namespace LEX
{
	//TODO: Clean the ref_wrapper pls, thanks

	struct Dispatcher;

	template <class R, class T, class... Args>
	struct BasicDispatcher;

	
	struct TEST_REF;
	//*
	namespace detail
	{
		enum ref_type
		{
			kNoRef,
			kMaybeRef,
			kLocalRef,
			kScopeRef,//TODO: SCOPED not scope
			kGlobalRef,
		};

		template <ref_type T>
		struct passable_ref : public std::bool_constant<true> {};

		template <>
		struct passable_ref<kScopeRef> : public std::bool_constant<false> {};



		template <ref_type T>
		constexpr bool passable_ref_v = passable_ref<T>::value;

		template <typename T>
		concept primitive = std::is_scalar_v<T> || std::is_reference_v<T>;

		//struct noone {};

		//struct nothing : public noone {};

		struct wrapper_settings// : public nothing
		{
		protected:
			struct jank_ass_idea
			{
				jank_ass_idea(wrapper_settings* a_self)
				{
					a_self->refState = kActive;
				}
			};


			//using nothing::noone;

			enum State
			{
				kImmutable, //Created when a reference is transfered. Means it is undoubtedly a reference. Exists for maybe's benefit
				kInactive,	//Cannot create active wrappers or set the current value of refs.
				kActive,	//While active it can set its reference to its current value but it cannot make more active wrappers
				kTransmit,	//While transmitting, it makes active wrappers, but cannot be active itself.
			};

		public:
			wrapper_settings() = default;
			wrapper_settings(const wrapper_settings& other)
			{
				refState = other.refState;
				Demote();
			}
			wrapper_settings(wrapper_settings&& other)
			{
				refState = other.refState;
				Demote();
			}

		protected:
			void Demote()
			{
				switch (refState)
				{
				case kTransmit:
					refState = kActive;
					break;
				case kActive:
					refState = kInactive;
				}
			}


			State refState = kInactive;

		};

		template <typename T, typename = void>
		struct basic_wrapper : public T, public wrapper_settings
		{
		public:
			using Elem = T;

		public:
			using T::T;

			//Break this down into 2 parts, the basic wrapper will be both primitive and complex. This one with the auto template param
			// will be what handles different reference types.

			basic_wrapper(const T& other) : T{ other } {}
			basic_wrapper(T&& other) : T{ other } {}

			/*
			basic_wrapper& operator= (const basic_wrapper& other)
			{
				return T::operator=(other);
			}
			basic_wrapper& operator= (const T& other)
			{
				return T::operator=(other);
			}
			//*/

			//This system is how I could possibly handle the situation of preventing certain types fall into certain hands.
			basic_wrapper() = default;//This is required if I do the below
		};


		template <primitive T>
		struct basic_wrapper <T> : public wrapper_settings
		{
		public:
			using Elem = std::remove_reference_t<T>;

			static constexpr bool k_isReference = std::is_reference_v<T>;

		private:
			T _value{};

		public:
			basic_wrapper() requires(!k_isReference) = default;
			basic_wrapper(const Elem& val) requires(!k_isReference) : _value(val) {}
			basic_wrapper(Elem&& val) requires(!k_isReference) : _value(val) {}

			basic_wrapper(Elem& val) requires(k_isReference) : _value{ val } {}



			template<std::convertible_to<T> Tx>
			basic_wrapper& operator=(const basic_wrapper<Tx>& other)
			{
				_value = other;
				return *this;
			}
			//*
			basic_wrapper& operator= (const basic_wrapper& other)
			{
				//This and the thing above should be the exact same fucking thing. No idea why it isn't.
				_value = other;
				return *this;
			}
			//*/
			basic_wrapper& operator= (const T& other)
			{
				_value = other;
				return *this;
			}

			/*
			basic_wrapper& operator= (const basic_wrapper& other)
			{
				_value = other;
				return *this;
			}

			//*/

			constexpr operator Elem& () { return _value; }
			constexpr Elem* operator &() { return &_value; }


			constexpr operator const Elem& () const { return _value; }
			constexpr const Elem* operator &() const { return &_value; }
		};



		template <typename T1, ref_type T2>
		struct ref_wrapper : public basic_wrapper<T1>
		{
			using Elem = std::remove_reference_t<T1>;

			static constexpr bool k_isReference = std::is_reference_v<T1>;



			using basic_wrapper<T1>::basic_wrapper;
			using basic_wrapper<T1>::operator=;


			template<typename Tx, ref_type Ty>
			friend struct ref_wrapper;

			static constexpr bool k_maybeRef = T2 == ref_type::kMaybeRef;
			using Refr = std::reference_wrapper<Elem>;

			//The idea of this object is effectively unifying the ValueTypes into one, as well as having the function that pulls them
			// united into one. As well as the function that moves them uniting into one.


			//So small ish problem, I want this to be able to take raw values, rather, I want the instantiable to be able to take raw
			// values. So the problem is moving the constructor to instantiable, rn.



			ref_wrapper() = default;// : _reference {std::ref<T1>(*this)}{}

			ref_wrapper(const std::reference_wrapper<Elem>& other) requires(!k_isReference) : _reference{ other }
			{
				this->refState = wrapper_settings::kImmutable;
			}


			/*
			ref_wrapper(const ref_wrapper& other) requires(!k_maybeRef) = default;
			ref_wrapper(ref_wrapper&& other) requires(!k_maybeRef) = default;

			ref_wrapper(const ref_wrapper& other) requires(k_maybeRef) : basic_wrapper{ other }, _reference{ std::nullopt } {}
			ref_wrapper(ref_wrapper&& other) requires(k_maybeRef) : basic_wrapper{ other }, _reference{ std::nullopt } {}
			/*/
			template <ref_type Ty>
			ref_wrapper(const ref_wrapper<T1, Ty>& other) : basic_wrapper<T1>{ other }, _reference{ other._reference } {}

			template <ref_type Ty>
			ref_wrapper(ref_wrapper<T1, Ty>&& other) : basic_wrapper<T1>{ other }, _reference{ other._reference } {}
			//*/


			~ref_wrapper()
			{
				if (this->refState == wrapper_settings::kActive) {
					if (auto ref = reference())
					{
						*ref = *this;
					}

				}
			}
		protected:
			Elem* reference()
			{
				if constexpr (k_maybeRef) {
					if (!this->refState != wrapper_settings::kImmutable)
						return nullptr;
				}
				return std::addressof(_reference.get());
			}


			//ref_wrapper(ref_wrapper&& other){}

			std::reference_wrapper<Elem> _reference = std::ref<Elem>(*this);

			//jank_ass_idea _jank = this;

		};


		template <typename T1, ref_type T2>
		struct complex_wrapper : public ref_wrapper<T1, T2>
		{
			using Elem = std::remove_reference_t<T1>;

		private:
			using Base = ref_wrapper<T1, T2>;
			//using ref_wrapper<T1, T2>::ref_wrapper;
			//using ref_wrapper<T1, T2>::basic_wrapper;
		public:
			//Make it so I don't need 2 versions for this pls.
			friend struct LEX::TEST_REF;
				
			template <class R, class T, class... Args>
			friend struct LEX::BasicDispatcher;

			friend struct LEX::Dispatcher;


			using Self = complex_wrapper<T1, T2>;

		public:
			constexpr static auto Type = T2;

		public:

			//This solely can be created by a reference of the type T

			//This is what is supposed to handle the reference hand off I believe.
			complex_wrapper(Elem& other) requires(T2 != ref_type::kMaybeRef) : Base{ std::ref(other) } { logger::info("test"); }//Enable this only if it's not maybe ref

			//*
		protected:
			template <typename... TArgs>
				requires (std::is_constructible_v<Base, TArgs...> && !any_true<std::is_base_of_v<wrapper_settings, std::remove_cvref_t<TArgs>>...>::value)
			constexpr complex_wrapper(TArgs&&... args)
				noexcept(noexcept(Base(std::forward<TArgs...>(args...))))
				requires (T2 != ref_type::kMaybeRef) : Base(std::forward<TArgs...>(args...))
			{
			}
			//*/

			//*
		public:
			template <typename... TArgs>
				requires (std::is_constructible_v<Base, TArgs...> && !any_true<std::is_base_of_v<wrapper_settings, std::remove_cvref_t<TArgs>>...>::value)
			constexpr complex_wrapper(TArgs&&... args) noexcept(noexcept(Base(std::forward<TArgs...>(args...))))
				requires (T2 == ref_type::kMaybeRef) : Base(std::forward<TArgs...>(args...))
			{
			}
			//*/

		public:


			complex_wrapper(const std::reference_wrapper<Elem>& other) :
				Base{ other }
			{}

			/*
			template<ref_type T> requires(T > Type)
				complex_wrapper(const complex_wrapper<T1, T>& other) : Base{ other }
			{

			}
			//*/

			//*
			template<std::convertible_to<T1> Tx, ref_type Ty> requires(Ty > Type)
				complex_wrapper(const complex_wrapper<Tx, Ty>& other) : Base{ other }
			{

			}
			//*/


			//Basically prohibits scoped from passing to itself, forcing it to be demoted.
			complex_wrapper(const complex_wrapper& other) requires (passable_ref_v<Type>) = default;
			complex_wrapper(complex_wrapper&& other) requires (passable_ref_v<Type>) = default;

			//*
			template<std::convertible_to<T1> Tx>
			complex_wrapper(const complex_wrapper<Tx, T2>& other) requires (passable_ref_v<Type>) : Base{ other } {};

			template<std::convertible_to<T1> Tx>
			complex_wrapper(complex_wrapper<Tx, T2>&& other) requires (passable_ref_v<Type>) : Base{ other } {}
			//*/

			//NOTE: Want to see if the looks fit
			complex_wrapper(const complex_wrapper& other) = delete;
			complex_wrapper(complex_wrapper&& other) = delete;


			using ref_wrapper<T1, T2>::operator=;

			/*
			template <typename TArg>
			//requires (std::is_constructible_v<Base, TArgs...>)
			constexpr complex_wrapper& operator=(TArg&& arg)// noexcept(noexcept(Base(std::forward<TArgs...>(args...))))
				//requires (T2 == ref_type::kMaybeRef) : Base(std::forward<TArgs...>(args...))
			{
				return Base::operator=(arg);
			}
			//*/





			//This should be literal and pulls the entire value.
			//template<ref_type RT>
			//complex_wrapper(const complex_wrapper<T1, RT>& other) : complex_wrapper{ other.pull_ref() } {}


			//complex_wrapper(const std::reference_wrapper<T1>& other) :
			//	value{ other }
			//{}
			//Note, ONLY construct. Nothing else.






			//I think this remains deleted
			//template<int _D>requires(_D > D)
			//	ComplexWrapper(complex_wrapper<T, _D>&&) {}

		//protected:
			~complex_wrapper() = default;
		};

		template<typename T>
		struct complex_wrapper<T, kNoRef>;


		//These complex wrappers when recieving input mustn't use the value. that much is off limits.

		//Both of these should have some derived from business going on here where it can accept reference_wrappers that derive
		// from the target type.

		template <std::derived_from<wrapper_settings> Base_Wrapper>
		struct instantiable_wrapper : public Base_Wrapper
		{
			//The instantiable wrapper should probably take non-reference values.
		public:
			using Base = Base_Wrapper;
			using Self = instantiable_wrapper<Base_Wrapper>;
			using Base_Wrapper::Base_Wrapper;

			//Make this a macro I can design
			template <typename... TArgs>
				requires (std::is_constructible_v<Base, TArgs...> && !any_true<std::is_base_of_v<wrapper_settings, std::remove_cvref_t<TArgs>>...>::value)
			constexpr instantiable_wrapper(TArgs&&... args) noexcept(noexcept(Base(std::forward<TArgs...>(args...)))) :
				Base(std::forward<TArgs...>(args...))
			{
			}

		protected:
			//TODO: Put this is wrapper settings, and make it take a setting. I do not want or need multiple versions of this

			wrapper_settings::jank_ass_idea jankness = this;

		public:
			//This gives us the ability to create. I unfortunately need to do it like this.
			~instantiable_wrapper() {}
		};

		struct base {};
		struct derived : public base {};


		//The idea I'll use is likely that the basic includes, the ref_wrap includes, the complex has a privated variable constructor,
		// and the instantiable has a 2 way as well.
		inline void TestThis()
		{
			constexpr bool cont = std::convertible_to<int, int>;


			using Self = complex_wrapper<int, kLocalRef>;
			int testIn = 1;

			complex_wrapper<int, kLocalRef> test1 = testIn;

			complex_wrapper<int, kMaybeRef> test2 = 1;//constructor destroyed?

			complex_wrapper<int, kLocalRef> test3a = test1;//constructor destroyed?
			ref_wrapper<int, kLocalRef> test4{ test1 };
			//complex_wrapper<int, kLocalRef> test5 = test2;

			derived* _derived;
			complex_wrapper<derived*, kLocalRef> base1 = _derived;
			base1 = _derived;
			//complex_wrapper<base*, kLocalRef> base2 = _derived;//This should be allowed but it isn't
			complex_wrapper<base*, kLocalRef> base3 = base1;//This should also be allowed.


			base3 = base1;
			base3 = _derived;

			basic_wrapper<int> test5 = 1;
			test5 = 1;

			test4 = 1;
			int out1 = test2;
			test1 = test2;
			//Next I need to set up operator usage

			int& out2 = test2;


			complex_wrapper<int&, kLocalRef> test6 = testIn;

			complex_wrapper<int, kLocalRef> test7 = test6;
			constexpr auto test = !any_true<std::is_base_of_v<wrapper_settings, decltype(test3a)>>::value;
			//Now that I think of it, this should be allowed no? It would either give you the reference of the maybe ref slot, or it would
			// give the reference it has stored. But it wouldn't give it to a local.
			//instantiable_wrapper<complex_wrapper<int, kLocalRef>> test4 = test2;
		}










		template<typename T>
		struct try_wrap_param
		{
			using type = T;
		};

		template<std::derived_from<wrapper_settings> T>
		struct try_wrap_param<T>
		{
			using type = instantiable_wrapper<T>;
		};

		template<typename T>
		struct try_wrap_param<T&> : public try_wrap_param<T> {};

		template<typename T>
		struct try_wrap_param<T&&> : public try_wrap_param<T> {};

		template<typename T>
		using try_wrap_param_t = try_wrap_param<T>::type;




		template<typename T>
		struct simplify_wrapper
		{
			using type = T;
		};

		template<std::derived_from<wrapper_settings> T>
		struct simplify_wrapper<T>
		{
			using type = T::Elem;
		};



		template<typename T>
		using simplify_wrapper_t = simplify_wrapper<T>::type;


		template<typename T, bool Ret>
		struct reference_type
		{
			static constexpr auto value = kNoRef;
		};


		template<std::derived_from<wrapper_settings> T, bool Ret>
		struct reference_type<T, Ret>
		{
			static constexpr auto value = T::Type;
		};

		//Here's what I could do instead. Remove the reference and try to get the reference type, then comparre
		template<typename T, bool Ret>
		struct reference_type<T&, Ret>
		{
		private:
			using Type = std::remove_reference_t<T>;
			static constexpr auto _underlying = reference_type<Type, Ret>::value;

		public:

			static constexpr auto value = _underlying != kNoRef ?
				_underlying : Ret ?
				kLocalRef : kScopeRef;
		};

		//RValue references are basically treated like regular functions from the grand scheme of things.
		template<typename T, bool Ret>
		struct reference_type<T&&, Ret> : public reference_type<T, Ret> {};

		//template<typename T, bool Ret>
		//struct reference_type<const T, Ret> : reference_type<T> {};


		template<typename T, bool Ret>
		constexpr auto reference_type_v = reference_type<T, Ret>::value;

		constexpr auto TVal = reference_type_v<complex_wrapper<int, kGlobalRef>&&, false>;
	}
	//*/

	template <typename T>
	using maybe_ref = detail::complex_wrapper<T, detail::ref_type::kMaybeRef>;

	template <typename T>
	using local_ref = detail::complex_wrapper<T, detail::ref_type::kLocalRef>;

	template <typename T>
	using scoped_ref = detail::complex_wrapper<T, detail::ref_type::kScopeRef>;

	template <typename T>
	using global_ref = detail::complex_wrapper<T, detail::ref_type::kGlobalRef>;

}

namespace std
{
	//Thinking of specialing this in such a way that I redirect what would be returned from this.
	//_EXPORT_STD template <class _Ty>
	//_NODISCARD _CONSTEXPR20 std::reference_wrapper<int> ref(MOV& _Val) noexcept {
	//	return std::reference_wrapper<int>(_Val.a);
	//}

	//This may only be required for maybe ref
	template <typename T1, LEX::detail::ref_type T2>
	std::reference_wrapper<T1> ref(LEX::detail::complex_wrapper<T1, T2>& _Val) noexcept {
		return std::reference_wrapper<T1>(_Val);
	}
}