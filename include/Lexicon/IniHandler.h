#pragma once

#include "SimpleIni.h"



namespace LEX
{

	struct IniHandler
	{
		template<typename T> requires (std::integral<T> || std::is_enum_v<T>)
			static void SetValue(T& to, CSimpleIniA& ini, std::string_view category, std::string_view key)
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
					report::warn("Value for {}::{} is invalid. No enum under '{}'", category, key, value);
				}

				to = opt_enum.value();
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



		static void SetValue(std::string& to, CSimpleIniA& ini, std::string_view category, std::string_view key)
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


		static bool GetIni(std::string_view path, CSimpleIniA& ini)
		{
			ini.SetUnicode();

			SI_Error rc = ini.LoadFile(path.data());

			assert(rc == SI_OK);

			return rc >= 0;

		}

		static bool GetIni(std::string_view folders, std::string_view file, CSimpleIniA& ini)
		{			
			ini.SetUnicode();

			std::string path = std::format("{}/{}", folders, file);

			auto result = GetIni(path, ini);

			return result;

		}

	};


}