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
}

inline int __init = 0;