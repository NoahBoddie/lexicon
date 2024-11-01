#pragma once

#include "Lexicon/IniHandler.h"

namespace LEX
{




	
	struct SettingManager : protected IniHandler
	{
		//I may expose this outside of here.
	private:

#ifndef SETTING_PATH
		constexpr static std::string_view settingPath = "";
#else
//#define CREATE_SETTING_PATH(...) STRINGIZE(__VA_ARGS__##__VA_OPT__(/)##{})
		//constexpr static std::string_view settingPath = CREATE_SETTING_PATH(SETTING_PATH);
		constexpr static std::string_view settingPath = STRINGIZE(SETTING_PATH);
//#undef SETTING_PATH
//#undef CREATE_SETTING_PATH

#endif


		
	public:

		



		std::string dataDir = "lexicon";
		std::string coreDir = "";
		std::string reportDir = "";
		std::string language = "english";
		spdlog::level::level_enum level = spdlog::level::info;


		//For the logger filter, I'm thinking of making something that can help break apparent what exactly should be taken.
		// Making a version for this that can handle arrays might be helpful for that. Then making something that splits a string up delimited by a character

		//Having a script focused filter might be cool, but definitely not something I should focus on right now.

		bool runtimeBreakpoint = true;

		bool _initialized = false;



		static SettingManager* GetSingleton()
		{
			static SettingManager singleton;
			singleton.Initialize();

			return std::addressof(singleton);
		}

		void Initialize()
		{
			//static std::mutex lock{};

			//std::lock_guard<std::mutex> guard{ lock };

			if (_initialized)
				return;


			//Even if an exception is encountered it only gets one shot to load this sort of stuff. That's it.
			

			CSimpleIniA ini;
			


			if (GetIni(settingPath, "lexicon.ini", ini) == false) {
				logger::info("cannot load lexicon.ini at {}", settingPath);
				return;
			}


			SetValue(dataDir, ini, "General", "sDataDir");
			SetValue(coreDir, ini, "General", "sCoreDir");
			SetValue(reportDir, ini, "Debug", "sReportDir");
			SetValue(language, ini, "Debug", "sLanguage");
			SetValue(runtimeBreakpoint, ini, "Debug", "bRuntimeBreakpoint");
			SetValue(level, ini, "Debug", "sDefaultLogLevel");

			logger::info("somthing {}", magic_enum::enum_name(level));
			
			_initialized = true;
		}
	

		static bool IsEnglish()
		{
			return !stricmp(GetSingleton()->language.c_str(), "english");
		}

	};
}