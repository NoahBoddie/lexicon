#include "Lexicon/Impl/PCH.h"


#ifdef LEX_SOURCE
#include "Lexicon/Engine/SettingManager.h"
#endif

void logger::InitializeLogging()
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
#ifdef LEX_SOURCE

    if (level >= spdlog::level::info) {
        level = LEX::SettingManager::GetSingleton()->level;
    }

#else

    if (auto def = spdlog::default_logger(); def)
    {
        return;
    }

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
    logger::info("No default logger set, using standard.");
    return;

    auto color_log = dynamic_cast<spdlog::sinks::stdout_color_sink_mt*>(log.get());

    if (color_log)
    {
        //color_log->set_color(spdlog::level::level_enum::trace, spdlog::color)
    }

    //void set_color(spdlog::level::level_enum::trace, spdlog::yellow);

    
}
