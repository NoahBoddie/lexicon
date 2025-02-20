#pragma once
#define __STOP_USING__ 1
#ifdef __STOP_USING__
//Need a way to turn this shit off.
//*
#include <cassert>
#include <cctype>
#include <cerrno>
#include <cfenv>
#include <cfloat>
#include <cinttypes>
#include <climits>
#include <clocale>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <cuchar>
#include <cwchar>
#include <cwctype>

#include <algorithm>
#include <any>
#include <array>
#include <atomic>
#include <barrier>
#include <bit>
#include <bitset>
#include <charconv>
#include <chrono>
#include <compare>
#include <complex>
#include <concepts>
#include <condition_variable>
#include <deque>
#include <exception>
#include <execution>
#include <filesystem>
#include <format>
#include <forward_list>
#include <fstream>
#include <functional>
#include <future>
#include <initializer_list>
#include <iomanip>
#include <iosfwd>
#include <ios>
#include <iostream>
#include <istream>
#include <iterator>
#include <latch>
#include <limits>
#include <locale>
#include <map>
#include <memory>
#include <memory_resource>
#include <mutex>
#include <new>
#include <numbers>
#include <numeric>
#include <optional>
#include <ostream>
#include <queue>
#include <random>
#include <ranges>
#include <regex>
#include <ratio>
#include <scoped_allocator>
#include <semaphore>
#include <set>
#include <shared_mutex>
#include <source_location>
#include <span>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <string_view>
#include <syncstream>
#include <system_error>
#include <thread>
#include <tuple>
#include <typeindex>
#include <typeinfo>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <valarray>
#include <variant>
#include <vector>
#include <version>


#include <ShlObj_core.h>
#include <Windows.h>
#include <Psapi.h>
//*/
#undef cdecl // Workaround for Clang 14 CMake configure error.


#define RGL_LOG(mc_level, mc_text, ...) logger::mc_level(mc_text __VA_OPT__(,)__VA_ARGS__)


#pragma warning(push)
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#pragma warning(pop)

//I don't care who uses them right now.
#undef min
#undef max

//This logger is meant to only be used in this project, not to leak this definition into another.
// Move this bit into a src file.


//move this please.
inline HMODULE GetCurrentModule()
{
    //Make this static in that it stores teh result it gets the first time so it never needs to search it again.
    // NB: XP+ solution!
    static HMODULE hModule = NULL;

    if (hModule == NULL) {
        GetModuleHandleEx(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
            (LPCTSTR)GetCurrentModule,
            &hModule);
    }

    return hModule;
}

inline std::string GetModuleName(HMODULE mdl)
{
    //Make this static in that it stores teh result it gets the first time so it never needs to search it again.
    // NB: XP+ solution!
    static HMODULE hModule = NULL;

    std::vector<char> buffer;
    buffer.resize(MAX_PATH);

    GetModuleFileNameA(mdl, &buffer.at(0), MAX_PATH);

    return std::string{ buffer.data() };
}



//>src
#include "spdlog/spdlog.h"
//#include <spdlog/sinks/basic_file_sink.h>
//#include <spdlog/sinks/msvc_sink.h>
//#include <spdlog/sinks/stdout_sinks.h>
#include "spdlog/sinks/stdout_color_sinks.h"
#include "magic_enum.hpp"
namespace logger
{
	//Apparently this has issues with clang, but I don't use clang (atm) so I'm free (for now)
	using source_location = std::source_location;
#define SOURCE_LOGGER(a_func, a_type)                                                 \
                                                                                      \
	template <class... Args>                                                          \
	struct [[maybe_unused]] a_func                                                    \
	{                                                                                 \
		a_func() = delete;                                                            \
                                                                                      \
		explicit a_func(                                                              \
			fmt::format_string<Args...> a_fmt,                                        \
			Args&&... a_args,                                                         \
			source_location a_loc = source_location::current())                       \
		{                                                                             \
            InitializeLogging();                                                      \
			spdlog::log(                                                              \
				spdlog::source_loc{                                                   \
					a_loc.file_name(),                                                \
					static_cast<int>(a_loc.line()),                                   \
					a_loc.function_name() },                                          \
				spdlog::level::a_type,                                                \
				a_fmt,                                                                \
				std::forward<Args>(a_args)...);                                       \
		}                                                                             \
	};                                                                                \
                                                                                      \
	template <class... Args>                                                          \
	a_func(fmt::format_string<Args...>, Args&&...) -> a_func<Args...>;


#define DEFAULT_LOGGER() extern "C" __declspec(dllexport) void SetDefaultLogger()

    void InitializeLogging();


	SOURCE_LOGGER(trace, trace);
	SOURCE_LOGGER(debug, debug);
	SOURCE_LOGGER(info, info);
	SOURCE_LOGGER(warn, warn);
	SOURCE_LOGGER(error, err);
	SOURCE_LOGGER(critical, critical);
    /*
    inline void InitializeLogging()
    {

        static bool _initialized = false;

        if (_initialized)
            return;

        _initialized = true;

        //_logger<void>{}();
        auto default_logger = GetProcAddress(GetCurrentModule(), "SetDefaultLogger");

        if (auto default_logger = GetProcAddress(GetCurrentModule(), "SetDefaultLogger"); default_logger) {
            using func_t = void(*)();
            func_t func = (func_t)default_logger;
            return func();
        }


        std::shared_ptr<spdlog::logger> log = spdlog::stdout_color_mt("console");

#ifndef NDEBUG
        auto level = spdlog::level::trace;
#else
        //Use right alt for just debug logging, control to allow debugger to attach.
        auto level = GetKeyState(VK_RCONTROL) & 0x800 || GetKeyState(VK_RMENU) & 0x800 ?
            spdlog::level::debug : spdlog::level::info;
#endif

        if (level >= spdlog::level::info) {
            level = LEX::SettingManager::GetSingleton()->level;
        }

        log->set_level(level);
        log->flush_on(level);

        spdlog::set_default_logger(std::move(log));
        //spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%t] [%s:%#] %v");
        spdlog::set_pattern("%s(%#): [%^%l%$] %v");


#ifdef NDEBUG
        if (spdlog::level::debug == level) {
            logger::debug("debug logger in release enabled.");
        }
#endif
        return;

        auto color_log = dynamic_cast<spdlog::sinks::stdout_color_sink_mt*>(log.get());

        if (color_log)
        {
            //color_log->set_color(spdlog::level::level_enum::trace, spdlog::color)
        }

        //void set_color(spdlog::level::level_enum::trace, spdlog::yellow);

        logger::info("No default logger set, using standard.");
    }
    //*/
}



#include "RoguesGallery.hpp"


#include "boost/regex.hpp"




using namespace std::literals;

//*


//This shit is supposed to be in src, it's not making it into the next project stupid.
namespace LEX
{
	using namespace RGL_NAMESPACE;
	using namespace RGL_INCLUDE_NAMESPACE;
}

#include "Lexicon/Report.h"

#ifdef LEX_SOURCE
#include "Lexicon/Engine/Impl/EngineEnum.h"
#endif




//SEND TO RGL PLZ

//Move to RGL ig
struct Initializer
{
    using def = void(*)();
    using now = void(*)(size_t);


    inline static std::vector<std::pair<def, std::string_view>> executeList;

    inline static std::set<std::string_view> executed;

    inline static bool Finished(std::string_view key = "")
    {
        return executed.contains(key);
    }

    static void Execute(std::string_view key = "")
    {
        //Should be run when there's nothing more to initialize.
        if (executeList.empty() == true)
            return;

        logger::debug("executing {}", key);


        executeList.erase(std::remove_if(executeList.begin(), executeList.end(),
            [&](auto&& pair) 
            { 
                if (key == pair.second) {
                    pair.first();
                    return true;
                }

                return false; 
            }),
            executeList.end());
        
        executed.emplace(key);

        //for (auto func : executeList) {
        //    func();
        //}
        //executeList.clear();
        //_done = true;
    }


    
    Initializer(def func, std::string_view key = "")
    {
        if (func)
            //func();
            executeList.push_back(std::make_pair(func, key));
    }

    
    Initializer(def func, int)
    {
        if (func)
            func();
    }

};


//Im thinking of adding keywords to initialize, allowing it to have keywords.

//Initializes something on the spot.
#define INITIALIZE__COUNTED(mc_counter,...) inline extern void CONCAT(__init_func_,mc_counter)();\
volatile inline extern Initializer CONCAT(__init_var_,mc_counter) = {CONCAT(__init_func_,mc_counter) __VA_OPT__(,) __VA_ARGS__};\
inline extern void CONCAT(__init_func_,mc_counter)()

#define INITIALIZE(...) INITIALIZE__COUNTED(__COUNTER__,__VA_ARGS__)

#define INITIALIZE_NOW(...) INITIALIZE__COUNTED(__COUNTER__,0,__VA_ARGS__)


//inline static Initializer CONCAT(__init_,__LINE__) = []() -> void


//Revised to need to be executed at a particular time. This prevents issues with it going off too early.
//Revise this in a manner that it doesn't have to have a semi-colon at the end. maybe forward declare a function rather than a lambda.
#pragma region RGL_SPACE

#define REQ_CONSTRAINT(mc_type, mc_size, mc_cmp, mc_phrase) static_assert(sizeof(mc_type) mc_cmp mc_size, STRINGIZE(sizeof mc_type mc_phrase mc_size.))

#define REQUIRED_SIZE(mc_type, mc_size) REQ_CONSTRAINT(mc_type, mc_size, ==, is not)
#define REQ_LESS_SIZE(mc_type, mc_size) REQ_CONSTRAINT(mc_type, mc_size, <, is not less than)
#define REQ_OR_LESS_SIZE(mc_type, mc_size) REQ_CONSTRAINT(mc_type, mc_size, <=, is greater than)


//Can instantiate should have template versions so it will basically handle construction in however it's best handled.
// It only cares about basic instantiation, not quite if it's abstract or not.
#define CAN_INSTANTIATE(mc_type) void CONCAT(__INSTANT_TEST__,__COUNTER__)() { mc_type a_test{}; }

#define UNCONST(mc_expression) const_cast<std::remove_const_t<decltype(mc_expression)>>(mc_expression)


//#define DEFINE_SELF_IMPL(mc_counter) auto CONCAT(__self_,mc_counter)() -> decltype(*this); using Self = std::invoke_result_t<decltype(CONCAT(__self_,mc_counter))>
//#define DEFINE_SELF DEFINE_SELF_IMPL(__COUNTER__)

/*
//This is how define self should be implemented.
template<typename T>
struct class_t;

template<typename T, typename R>
struct class_t< R(T::*)() > { using type = T; };
void self_f(); using Self = typename class_t<decltype(&self_f)>::type;
//*/

namespace RGL
{


    template<typename Test, template<typename...> class Ref>
    struct is_specialization : std::false_type {};

    template<template<typename...> class Ref, typename... Args>
    struct is_specialization<Ref<Args...>, Ref> : std::true_type {};//I'd like to make a different one of these where it accepts derived types too.

    template<typename Test, template<typename...> class Ref>
    concept specialization_of = is_specialization<Test, Ref>::value;



    template <typename T>
    using remove_ref_const = std::remove_reference_t<std::remove_const_t<T>>;


    
	template <typename Test, typename T, typename... More>
	struct is_any_convertible : public std::conditional_t<std::convertible_to<T, Test>, std::true_type, std::conditional_t<!!sizeof...(More), is_any_convertible<Test, More...>, std::false_type>>
	{
		is_any_convertible() = delete;
	};

	template <typename Test, typename T, typename... More>
	constexpr bool is_any_convertible_v = is_any_convertible<Test, T, More...>::value;



	template <typename Test, typename T, size_t I = 0>
	struct convertible_variant_index : public
		std::conditional_t<
			specialization_of<Test, std::variant>,                              //Condition
			std::conditional_t<
				std::convertible_to<T, std::variant_alternative_t<I, Test>>,    //Condition
				std::integral_constant<size_t, I>,         //Result A
				std::conditional_t<     //Result B
					I + 1 < std::variant_size_v<Test>,                         //Condition
					convertible_variant_index<Test, T, I + 1>, //Result A
					std::integral_constant<size_t, -1>                         //Result B
				>
			>,
			std::integral_constant<size_t, -1>
		>
	{
        static constexpr size_t no_pos = -1;

        convertible_variant_index() = delete;
	};


    template <typename Test, typename T, size_t I = 0>
    constexpr size_t convertible_variant_index_v = convertible_variant_index<Test, T, I>::value;

    
    /*
	template <typename Test, typename T, size_t I = 0>
	struct is_variant_convertible : public 
		std::conditional_t<
			specialization_of<Test, std::variant>,                              //Condition
			std::conditional_t<
				std::convertible_to<T, std::variant_alternative_t<I, Test>>,    //Condition
				std::true_type,         //Result A
				std::conditional_t<     //Result B
					I + 1 < std::variant_size_v<Test>,                         //Condition
					is_variant_convertible<Test, T, I + 1>, //Result A
					std::false_type                         //Result B
				>
			>,
			std::false_type
		>
	{
		is_variant_convertible() = delete;
	};
    /*/

    template <typename Test, typename T, size_t I = 0>
    struct is_variant_convertible : public std::bool_constant<convertible_variant_index_v<Test, T, I> != -1> 
    {
        is_variant_convertible() = delete;
    };
    //*/
	template <typename Test, typename T>
	constexpr bool is_variant_convertible_v = is_variant_convertible<Test, T>::value;

	template <typename _From, typename _To>
	concept variant_convertible_to = is_variant_convertible<_From, _To>::value;
	
    template <typename _To, typename _From>
    concept variant_convertible_from = variant_convertible_to<_From, _To>;



    template <class Derived, class Base>
    concept pointer_derived_from = std::derived_from<std::remove_pointer_t<Derived>, std::remove_pointer_t<Base>>;


    struct accepts_all
    {
        //This type helps resolve some ambiguity between 2 like functions by making the invalid one convert while the actual on takes a reference.
        constexpr accepts_all() = default;

        template <typename T>
        constexpr accepts_all(T) {}


        template <typename T>
        constexpr accepts_all(const T&) {}

        template <typename T>
        constexpr accepts_all(T&&) {}
    };

}

namespace LEX::detail
{
    struct not_implemented {};
}




template <typename T>
T ReturnDefaultOrFailure()
{
	if constexpr (std::is_same_v<T, void>) {
		return;
	}
	else if constexpr (std::is_reference_v<T>) {
		//TODO: Please change this to not just throw some random ass nullptr.
		throw nullptr;
	}
	else {
		return {};
	}
}



template<typename T, typename... Args>
std::invoke_result_t<T, Args...> ExternCall(HINSTANCE a_module, LPCSTR func_name, Args&&... args)
{
    using _Ret = std::invoke_result_t<T, Args...>;

    using _Func = T*;//_Ret(*)(Args...);


    _Func func = (_Func)GetProcAddress(a_module, func_name);

    

    if (func) {
        static bool once = false;

        if (!once) {
            //RGL_LOG(debug, "Found {} in {}.", func_name, GetModuleName(a_module));
            RGL_LOG(debug, "Found {}.", func_name);
            once = true;
        }
    }
    else {
        static bool once = false;

        if (!once) {
            //RGL_LOG(critical, "Extern call failed. Function {} not found in {}.", func_name, GetModuleName(a_module));
            RGL_LOG(critical, "Extern call failed. Function {}.", func_name);
            once = true;
        }

        return ReturnDefaultOrFailure<_Ret>();
    }
    

    return func(std::forward<Args>(args)...);

}




template<typename T, typename... Args>
std::invoke_result_t<T, Args...> ExternCall(LPCSTR module_name, LPCSTR func_name, Args&&... args)
{
	using _Ret = std::invoke_result_t<T, Args...>;

    using _Func = T*;//_Ret(*)(Args...);

    HINSTANCE a_module = GetModuleHandleA(module_name);

    if (a_module == nullptr) {
        static bool once = false;

        if (!once) {
            RGL_LOG(debug, "Extern Call failed. Module {} not found.", module_name);
            once = true;
        }

        return ReturnDefaultOrFailure<_Ret>();
    }


    return ExternCall<T, Args...>(a_module, func_name, std::forward<Args>(args)...);


}

template <auto T, typename... Args> requires(std::is_function_v<std::remove_pointer_t<decltype(T)>>)
std::invoke_result_t<decltype(T), Args...> ExternCall(LPCSTR module_name, LPCSTR func_name, Args&&... args)
{
	return ExternCall<decltype(T), Args...>(module_name, func_name, std::forward<Args>(args)...);
}

namespace LEX
{
    namespace stl
    {
        template< class _From, class _To >
        concept convertible_to = std::convertible_to<_From, _To>;


        template< class To, class From >
        concept convertible_from = convertible_to<From, To>;



        template< class _From, class _To >
        concept castable_to = requires { static_cast<_To>(::std::declval<_From>()); };;

        template< class _To, class _From >
        concept castable_from = castable_to<_From, _To>;

    }
}



//I need to undeclare these macros when I get the chance.
//OBJECT_POLICY

//#define LEX_SOURCE 1

//Makes virtual functions final outside of the lexicon source executable
#ifdef LEX_SOURCE
//Makes virtual functions final outside of the lexicon source executable. Currently is inactive.
#define API_FINAL
#else
//Makes virtual functions final outside of the lexicon source executable. Currently is active.
#define API_FINAL final 
#endif




#ifdef LEX_SOURCE
#define OBJECT_POLICY ObjectPolicyImpl
#define FRWD_DECL_OBJECT_POLICY class OBJECT_POLICY
#else
#define OBJECT_POLICY ObjectPolicy
#define FRWD_DECL_OBJECT_POLICY struct OBJECT_POLICY
#endif // LEX_SOURCE



//Internal macro
#ifdef LEX_SOURCE
#define INTERNAL public
#define INTERNAL_(mc_access) mc_access
#define INTERNAL_PROTECTED INTERNAL_(protected)
#define INTERNAL_PUBLIC INTERNAL_(public)

#define INTERNAL_DEFAULT default


#define INTERN_TYPE int8_t
#define EXTERN_TYPE uint8_t

#define INTERN_NAME int8_t = {}
#define EXTERN_NAME uint8_t

//Access space is only supposed to be used by external binaries. Currently set to private.
#define EXTERNAL private

#define ACCESS_OR(mc_l, mc_r) mc_l
#else
#define INTERNAL private
#define INTERNAL_(mc_access) private
#define INTERNAL_PROTECTED private
#define INTERNAL_PUBLIC private

#define INTERNAL_DEFAULT delete

//Access space is only supposed to be used by external binaries. Currently set to public.
#define EXTERNAL public

#define ACCESS_OR(mc_l, mc_r) mc_r


#define INTERN_TYPE uint8_t
#define EXTERN_TYPE int8_t

#define INTERN_NAME uint8_t
#define EXTERN_NAME int8_t = {}

#endif // LEX_SOURCE

#if !defined(__INTELLISENSE__)
#define NEED_SOURCE static_assert(false, STRINGIZE(CONCAT(__FILE__, needs a source file.)));
#else
#define NEED_SOURCE static_assert(true);

#endif





#pragma endregion

//Move me please.
#include "Lexicon/Versioning.h"

#include "Lexicon/Impl/CustomTraits.h"
#include "Lexicon/Interfaces/Interface.h"
#include "Lexicon/Interfaces/InterfaceSingleton.h"
//*/

#endif

#define NULLCHECK(mc_condition) if (!mc_condition) report::critical("Condition '{}' is invalid, throwing fatal exception.", STRINGIZE(mc_condition))


