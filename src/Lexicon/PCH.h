#pragma once


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
#undef cdecl // Workaround for Clang 14 CMake configure error.


#define RGL_LOG(mc_level, mc_text, ...) logger::mc_level(mc_text __VA_OPT__(,)__VA_ARGS__)


#pragma warning(push)
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#pragma warning(pop)


//This logger is meant to only be used in this project, not to leak this definition into another.
// Move this bit into a src file.


//>src
#include "spdlog/spdlog.h"
//#include <spdlog/sinks/basic_file_sink.h>
//#include <spdlog/sinks/msvc_sink.h>
//#include <spdlog/sinks/stdout_sinks.h>
#include "spdlog/sinks/stdout_color_sinks.h"

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


    inline void InitializeLogging()
    {
        static bool _initialized = false;

        if (_initialized)
            return;

        _initialized = true;
        /*
        auto log =
        auto path = log_directory();
        if (!path) {
            report_and_fail("Unable to lookup SKSE logs directory.");
        }
        *path /= PluginDeclaration::GetSingleton()->GetName();
        *path += L".log";


        std::shared_ptr<spdlog::logger> log;


        if (IsDebuggerPresent()) {
            log = std::make_shared<spdlog::logger>(
                "Global", std::make_shared<spdlog::sinks::msvc_sink_mt>());
        }
        else {
            log = std::make_shared<spdlog::logger>(
                "Global", std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true));
        }

        //*/

        std::shared_ptr<spdlog::logger> log = spdlog::stdout_color_mt("console");

#ifndef NDEBUG
        const auto level = spdlog::level::trace;
#else
        //Use right alt for just debug logging, control to allow debugger to attach.
        const auto level = GetKeyState(VK_RCONTROL) & 0x800 || GetKeyState(VK_RMENU) & 0x800 ?
            spdlog::level::debug : spdlog::level::info;
#endif


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
    }


	SOURCE_LOGGER(trace, trace);
	SOURCE_LOGGER(debug, debug);
	SOURCE_LOGGER(info, info);
	SOURCE_LOGGER(warn, warn);
	SOURCE_LOGGER(error, err);
	SOURCE_LOGGER(critical, critical);
}


#include "RoguesGallery.hpp"




using namespace std::literals;

#include "Lexicon/Impl/Report.h"



//This shit is supposed to be in src, it's not making it into the next project stupid.
namespace LEX
{
	using namespace RGL_NAMESPACE;
	using namespace RGL_INCLUDE_NAMESPACE;

    using String = std::string;
    using _String = std::string;
}

inline int __init = 0;




//SEND TO RGL PLZ

//Move to RGL ig
struct Initializer
{
    using def = void(*)();

    inline static std::vector<def> executeList;

    static void Execute()
    {
        //Should be run when there's nothing more to initialize.
        if (executeList.empty() == true)
            return;

        for (auto func : executeList) {
            func();
        }

        executeList.clear();
    }


    template <std::convertible_to<def> T>
    Initializer(T func)
    {
        if (func)
            //func();
            executeList.push_back(func);
    }
};
inline static int test = 0;
//Initializes something on the spot.
#define INITIALIZE() inline static Initializer CONCAT(__init_,__COUNTER__) = []() -> void
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

namespace RGL
{
    
    template<typename Test, template<typename...> class Ref>
    struct is_specialization : std::false_type {};

    template<template<typename...> class Ref, typename... Args>
    struct is_specialization<Ref<Args...>, Ref> : std::true_type {};

    template<typename Test, template<typename...> class Ref>
    concept specialization_of = is_specialization<Test, Ref>::value;



    template <typename T>
    using remove_ref_const = std::remove_reference_t<std::remove_const_t<T>>;

}

namespace LEX::detail
{
    struct not_implemented {};
}


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
std::invoke_result_t<T, Args...> ExternCall(LPCSTR module_name, LPCSTR func_name, Args&&... args)
{
	using _Ret = std::invoke_result_t<T, Args...>;

    using _Func = T*;//_Ret(*)(Args...);


	static _Func func = nullptr;

    if (!func)
    {
        HINSTANCE API = GetModuleHandle(module_name);

        if (API == nullptr) {
            static bool once = false;

            if (!once) {
                RGL_LOG(debug, "Extern Call failed. Module {} not found.", module_name);
                once = true;
            }

            return ReturnDefaultOrFailure<_Ret>();
        }

        func = (_Func)GetProcAddress(API, func_name);


        if (func) {
            static bool once = false;

            if (!once) {
                RGL_LOG(debug, "Found {} in {}.", func_name, module_name);
                once = true;
            }
        }
        else {
            static bool once = false;

            if (!once) {
                RGL_LOG(critical, "Extern call failed. Function {} not found in {}.", func_name, module_name);
                once = true;
            }

            return ReturnDefaultOrFailure<_Ret>();
        }
    }

	return func(std::forward<Args>(args)...);

}

template <auto T, typename... Args> requires(std::is_function_v<std::remove_pointer_t<decltype(T)>>)
std::invoke_result_t<decltype(T), Args...> ExternCall(LPCSTR module_name, LPCSTR func_name, Args&&... args)
{
	return ExternCall<decltype(T), Args...>(module_name, func_name, std::forward<Args>(args)...);
}






//I need to undeclare these macros when I get the chance.
//OBJECT_POLICY

#define LEX_SOURCE 1

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

#else
#define INTERNAL private
#define INTERNAL_(mc_access) private
#define INTERNAL_PROTECTED private
#define INTERNAL_PUBLIC private

#define INTERNAL_DEFAULT delete

#endif // LEX_SOURCE





#pragma endregion