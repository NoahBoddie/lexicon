#pragma once

#include "Lexicon/IniHandler.h"
#include "Lexicon/IssueCode.h"


#include "Lexicon/Engine/SettingManager.h"


//Might go in impl, a user has no need to access this really.

namespace LEX
{
	struct IssueTable : public IniHandler
	{
		enum LanguageType
		{
			kOriginal,
			kTranslation,
			kMatch,
		};

		static void SetValue(IssueCode& to, CSimpleIniA& ini, std::string_view category, std::string_view key, bool translation)
		{
			try
			{
				to.code = std::stoll(key.data());
			}
			catch (std::invalid_argument& error)
			{
				report::apply::warn("Value for {}::{} is invalid.", category, key);
				return;
			}

			to.SetProjectID(category.data(), category.size());


			if (to.value) {

				auto& str = _table[to.value][translation];

				if (str.empty() == false) {
					report::apply::warn("Issue at {}::{} already taken.", category, to.code);
					return;
				}

				__super::SetValue(str, ini, category, key);
			}

		}

		using _Table = std::unordered_map<uint64_t, std::array<std::string, 2>>;



		static std::string_view GetIssueMessage(uint64_t code, bool translation)
		{
			auto it = _table.find(code);
			return it != _table.end() ? it->second[translation].c_str() : "";
		}
		
	private:
		static void LoadIssueStrings(std::string_view folders, std::string_view project, std::string_view language, LanguageType lang_type)
		{
			CSimpleIniA ini;

			std::string path = project.empty() ? std::format("{}/report/{}.ini", folders, language) : std::format("{}/report/{}/{}.ini", folders, project, language);

			if (GetIni(path, ini) == false) {
				logger::warn("Cannot load {} strings for {} at {}", project, language, folders);
				return;
			}
		
			std::list<CSimpleIniA::Entry> lister;

			ini.GetAllSections(lister);

			
			//should check below or over and readjust what it says based on that.

			if (lister.size() < 1)
			{
				if (project.empty() == false) {
					logger::warn("Project info tables require a section name with 4 characters or less. Invalid issue table at {}", path);
					return;
				}
			}
			else if (lister.size() > 1)
			{
				logger::warn("More than one section detected. Invalid issue table at {}", path);
				return;
			}


			std::string_view section = lister.size() ? lister.front().pItem : "";

			

			ini.GetAllKeys(section.data(), lister);


			for (auto& entry : lister)
			{
				IssueCode input;

				SetValue(input, ini, section, entry.pItem, lang_type != kOriginal);

				if (lang_type == kMatch)
				{
					_table[input][false] = _table[input][true];
				}

			}

		}
	
public:
		static void LoadStrings(std::string_view folders, std::string_view project = "")
		{
			std::string_view language = SettingManager::GetSingleton()->language;


			bool is_english = !_stricmp(language.data(), "english");


			LoadIssueStrings(folders, project, language, is_english ? kMatch : kTranslation);

			if (!is_english){
				LoadIssueStrings(folders, project, "english", kOriginal);
			}
		}
		

	private:
		inline static _Table _table;

	};

	INITIALIZE_NOW()
	{
		//I may reinvent the IssueTable into a string table, something to store translated strings for people to use that vary based on some kind of base string given.

		//Note, I can use the issue table as a method of telling what exactly a 

		auto singleton = SettingManager::GetSingleton();

		IssueTable::LoadStrings(singleton->reportDir);
	}
}