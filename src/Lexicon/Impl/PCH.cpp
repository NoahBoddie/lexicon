#include "Lexicon/Impl/PCH.h"


#ifdef LEX_SOURCE
#include "Lexicon/Engine/SettingManager.h"
#endif



std::optional<std::filesystem::path> log_directory()
{
    TCHAR buff[MAX_PATH];

    const auto result = SHGetFolderPath(NULL, CSIDL_MYDOCUMENTS, NULL, SHGFP_TYPE_CURRENT, buff);

    std::filesystem::path path = &buff[0];

    if (path.empty() || result != 0) {
        logger::info("failed to get known folder path");
        
        return std::nullopt;
    }

    
    path /= "Lexicon"sv;
    /*
    path /= "My Games"sv;
    if SKYRIM_REL_VR_CONSTEXPR(REL::Module::IsVR()) {
        path /= "Skyrim VR";
    }
    else {
        path /= std::filesystem::exists("steam_api64.dll") ?
            (std::filesystem::exists("openvr_api.dll") ? "Skyrim VR" : "Skyrim Special Edition") :
            "Skyrim Special Edition GOG";
    }
    path /= "SKSE"sv;
    //*/
    return path;
}


void SetLogger(std::string_view name, std::filesystem::path path, std::string_view pattern) {


    path /= name;
    path += L".log";


    std::shared_ptr<spdlog::logger> log;

    if (IsDebuggerPresent()) {
        log = std::make_shared<spdlog::logger>(
            name.data(), std::make_shared<spdlog::sinks::msvc_sink_mt>());
    }
    else {
        log = std::make_shared<spdlog::logger>(
            name.data(), std::make_shared<spdlog::sinks::basic_file_sink_mt>(path.string(), true));
    }

   
#ifndef NDEBUG
    const auto level = spdlog::level::trace;
#else
    const auto level = spdlog::level::info;
#endif


    log->set_level(level);
    log->flush_on(level);

    spdlog::register_logger(log);
    

    //spdlog::set_level(level);
    //spdlog::flush_on(level);
    
    
    //spdlog::set_pattern("%s(%#): [%^%l%$] %v"s);
    log->set_pattern(pattern.data());

    log->info("Logger '{}' set.", name);
}



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

    //Only setting up the console log here.

    std::shared_ptr<spdlog::logger> log = spdlog::stdout_color_mt("console");



    log->set_level(level);
    log->flush_on(level);

    spdlog::set_default_logger(std::move(log));
    //spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%n] [%l] [%t] [%s:%#] %v");
    spdlog::set_pattern("%s(%#): [%^%l%$] %v");
    

    //LEX::SettingManager::GetSingleton()->level;
    
    auto directory = log_directory();



    //SetLogger("program", *directory, "%s(%#): [%^%l%$] %v");
    //SetLogger("script", *directory, "%s(%#): [%^%l%$] %v");



#ifdef NDEBUG
    if (spdlog::level::debug == level) {
        logger::debug("debug logger in release enabled.");
    }
#endif
    logger::info("No default logger set, using standard.");
    return;
}
