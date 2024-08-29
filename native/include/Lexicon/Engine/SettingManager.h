#pragma once

#include "SimpleIni.h"

namespace LEX
{
	struct SettingManager
	{
		//I may expose this outside of here.

#ifndef SETTING_PATH
		constexpr static std::string_view settingPath = "{}";
#else
#define CREATE_SETTING_PATH(...) STRINGIZE(__VA_ARGS__##__VA_OPT__(/)##{})
		constexpr static std::string_view settingPath = CREATE_SETTING_PATH(SETTING_PATH);

#undef SETTING_PATH
#undef CREATE_SETTING_PATH

#endif
		

		std::string scriptDir = "lexicon/scripts";
		std::string language = "english";
		spdlog::level::level_enum level = spdlog::level::info;

		bool runtimeBreakpoint = true;

		bool _initialized = false;

		template<typename T> requires (std::integral<T> || std::is_enum_v<T>)
		void SetValue(T& to, CSimpleIniA& ini, std::string_view category, std::string_view key)
		{
			constexpr bool k_is_enum = std::is_enum_v<T>;

			const char* value;

			value = ini.GetValue(category.data(), key.data(), nullptr);

			if (!value)
			{
				report::apply::warn("cannot find value for {}::{}. Using default.", category, key);
				return;
			}
			
			if constexpr (k_is_enum)
			{
				std::optional<T> opt_enum = magic_enum::enum_cast<T>(value);

				if (opt_enum.has_value() == false)
				{
					to = opt_enum.value();
				}
			}
			else
			{
				long long raw;

				switch (Hash<HashFlags::Insensitive>(value))
				{
				case "true"_ih:
					raw = true;
					break;
				case "false"_ih:
					raw = false;
					break;

				default:

					try
					{
						raw = std::stoll(value);
					}
					catch (std::invalid_argument& error)
					{
						report::apply::warn("Value for {}::{} is invalid.", category, key);
						return;
					}
					break;
				}

				to = static_cast<T>(raw);
			}
			
			
		}

		
		void SetValue(std::string& to, CSimpleIniA& ini, std::string_view category, std::string_view key)
		{
			const char* value;

			value = ini.GetValue(category.data(), key.data(), nullptr);

			if (!value)
			{
				report::apply::warn("cannot find value for {}::{}. Using default.", category, key);
				return;
			}

			to = value;
		}


		static SettingManager* GetSingleton()
		{
			static SettingManager singleton;
			singleton.Initialize();

			return std::addressof(singleton);
		}

		void Initialize()
		{
			if (_initialized)
				return;

			//Even if an exception is encountered it only gets one shot to load this sort of stuff. That's it.
			_initialized = true;

			CSimpleIniA ini;
			ini.SetUnicode();

			
			std::string path = std::format(settingPath, "lexicon.ini");
			SI_Error rc = ini.LoadFile(path.c_str());
			if (rc < 0) 
			{ 
				logger::info("cannot load lexicon.ini");
				return;
			};
			
			assert(rc == SI_OK);


			SetValue(scriptDir, ini, "General", "sScriptDir");
			SetValue(language, ini, "Debug", "sLanguage");
			SetValue(runtimeBreakpoint, ini, "Debug", "bRuntimeBreakpoint");
			SetValue(level, ini, "Debug", "sDefaultLogLevel");

		}
	
	};
}