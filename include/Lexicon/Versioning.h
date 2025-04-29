


//This header file is used to copy the version namespace where included, as such it comes without a pragma. 
// Simply include it within the scope of a namespace to use it in that location.

//TODO: Some how make it only able to be included once per namespace. Include guards and pragmas are out.

//This space will be repeated over and over again, but the other part will not be.


#pragma region Declaring Version Namespaces

#define DECL_VERSION_SPACE(mc_number, mc_prev) \
			namespace _##mc_number { constexpr uintptr_t version = mc_number; namespace Previous = _##mc_prev; }

namespace Version
{
	DECL_VERSION_SPACE(1, 1);
	DECL_VERSION_SPACE(2, 1);
	DECL_VERSION_SPACE(3, 2);
	DECL_VERSION_SPACE(4, 3);
	DECL_VERSION_SPACE(5, 4);
	DECL_VERSION_SPACE(6, 5);
	DECL_VERSION_SPACE(7, 6);
	DECL_VERSION_SPACE(8, 7);
	DECL_VERSION_SPACE(9, 8);
	DECL_VERSION_SPACE(10, 9);



	//And so on and so on.
	//namespace _...
	//{}


}

#undef DECL_VERSION_SPACE

#pragma endregion



#ifndef VERSIONING_H

#define VERSIONING_H

//Id like to turn this into VersionDeclareSpace. I'd additionally like to turn off the version declaring stuff once it's been declared already
//Also, shorten the fuck out of these macros please.


//TODO: I'd like to move these INTERFACE macros somewhere else if you'd please.

//I would like to merge these, as I feel having the override visible would be helpful. The only difference might be
// text.
#ifdef LEX_SOURCE
	//A macro that pivots between an invalid function call and a forward declared virtual function. Currently forward declared.
#define INTERFACE_METHOD override

//A macro that pivotes between an invalid function call and a forward declared non-virtual function.
#define INTERFACE_FUNCTION 
#else
	//A macro that pivots between an invalid function call and a forward declared virtual function. Currently invalid call.
#define INTERFACE_METHOD override { report::break_critical("Interface not created from lex source. Aborting.");}

//A macro that pivotes between an invalid function call and a forward declared non-virtual function. Currently invalid call.
#define INTERFACE_FUNCTION  { report::break_critical("Interface not created from lex source. Aborting.");} 
#endif

//Make only one interface function and use override where it is.


//TODO: I would like to use novtable on a the interface and current version to prevent it from having more vtables thatn needed.

//INTERFACE_VERSION Doesn't need to do this because it doesn't have a body.

//I previously used this to get the name of the self when using the interface types but screw it.
//private ::RGL_INCLUDE_NAMESPACE::detail::SelfHelper<mc_name>,

//If I can make these the same function some how, that'd be great.
#define INTERFACE_VERSION_DERIVES_NO_SFX(mc_name, mc_base, ...) __declspec(novtable) \
			mc_name : public std::conditional_t<Previous::version != version, Previous::mc_name, mc_base> __VA_OPT__(,) __VA_ARGS__

#define INTERFACE_VERSION_DERIVES(mc_name, mc_base, ...) INTERFACE_VERSION_DERIVES_NO_SFX(CONCAT(IV_,mc_name), mc_base,__VA_ARGS__)


#define INTERFACE_VERSION_NO_SFX(mc_name, ...) INTERFACE_VERSION_DERIVES_NO_SFX(mc_name, UnmovableInterface, __VA_ARGS__)
#define INTERFACE_VERSION(mc_name, ...) INTERFACE_VERSION_DERIVES(mc_name, UnmovableInterface, __VA_ARGS__)

#define CURRENT_VERSION_NO_SFX(mc_type, mc_number, ...)								\
	namespace Current																\
	{																				\
		struct __declspec(novtable)  mc_type :  public _##mc_number::mc_type __VA_OPT__(,) __VA_ARGS__	\
		{																			\
			inline static constexpr uintptr_t version = mc_number;					\
																					\
			uintptr_t Version() const override final								\
			{																		\
				return mc_number;													\
			}																		\
		};																			\
	}

#define CURRENT_VERSION(mc_type, mc_number, ...) CURRENT_VERSION_NO_SFX(CONCAT(IV_,mc_type), mc_number, __VA_ARGS__)

//Short hand if you don't want to define the current version and implementation version differently. Note this could not forward declare, mustn't be abstract.
#define CURRENT_IMPL_VERSION_NO_SFX(mc_type, mc_number, ...) CURRENT_VERSION_NO_SFX(mc_type, mc_number, public InterfaceSingleton<mc_type>, __VA_ARGS__)
#define CURRENT_IMPL_VERSION(mc_type, mc_number, ...) CURRENT_VERSION(mc_type, mc_number, public InterfaceSingleton<mc_type>, __VA_ARGS__)

#define CHECK_INTERFACE_VERSION(...) if (Version() < version) { return __VA_ARGS__; }




#define IMPL_VERSION_DERIVES_NO_SFX(mc_name, mc_type) mc_name : public Version::Current::mc_type
#define IMPL_VERSION_DERIVES(mc_name, mc_type) IMPL_VERSION_DERIVES_NO_SFX(mc_name, CONCAT(IV_,mc_type))

#define IMPL_VERSION_NO_SFX(mc_type) IMPL_VERSION_DERIVES_NO_SFX(mc_type,mc_type)
#define IMPL_VERSION(mc_type) IMPL_VERSION_DERIVES(mc_type,mc_type)



#define IMPL_SINGLETON_DERIVES_NO_SFX(mc_name, mc_type) IMPL_VERSION_DERIVES_NO_SFX(mc_name,mc_type), public InterfaceSingleton<mc_name>
#define IMPL_SINGLETON_DERIVES(mc_name,mc_type) IMPL_VERSION_DERIVES(mc_name,mc_type), public InterfaceSingleton<mc_name>


#define IMPL_SINGLETON_NO_SFX(mc_type) IMPL_SINGLETON_DERIVES_NO_SFX(mc_type,mc_type)
#define IMPL_SINGLETON(mc_type) IMPL_SINGLETON_DERIVES(mc_type,mc_type)





#endif



#ifdef EXAMPLE //This is an example of how the version system is to be used.

namespace LEX
{
	//This is how I will version stuff. The idea is basically the namespaces isolate the version it belongs to easier, allowing for the macro to say who's current.
	// The struct outside of version provides an easy pivot point. This allows me to not use using which in turn allows me to forward declare the interface type.
	// The implementation can then exist elsewhere, possibly within a source file, or not allowed to be compiled if it isn't a source.

	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(Example)
			{
				virtual void IFuncDodad1() = 0;
			};
		}


		namespace _2
		{
			struct INTERFACE_VERSION(Example)
			{
				virtual void IFuncDodad2() = 0;
			};
		}


		namespace _3
		{
			struct INTERFACE_VERSION(Example)
			{
				virtual void IFuncDodad3() = 0;
			};
		}


		CURRENT_VERSION(Example, 3);
	}
	//On inspection, I can make something called IMPL_VERSION, that handles all this shit for me.
	struct Example : public Version::Current::Example, public InterfaceSingleton<Example>
	{
		void IFuncDodad1() override INTERFACE_METHOD;
		void IFuncDodad2() override INTERFACE_METHOD;
		void IFuncDodad3() override INTERFACE_METHOD;
	};
}
#endif