#pragma once

#include "Lexicon/TypeID.h"
#include "Lexicon/Interfaces/ProjectClient.h"

#include "Lexicon/Number.h"

namespace LEX
{

	struct DefaultClient : public ProjectClient
	{
		//Currently has an issue where failure is not descriptive if the location it failed in.

		void RecieveMessage(uint64_t severity, std::string_view message, ProjectClient* sender) override
		{
			//no reciever on this end.
		}

		bool HandleFormat(Script* script, std::string_view format, std::string_view name, std::string_view content) override
		{
			//By default has no formatter.
			return false;
		}


		virtual TypeOffset HandleExtraOffsetArgs(std::string_view category, std::string_view* data, size_t length) { return -1; }

		TypeOffset GetOffsetFromArgs(std::string_view category, std::string_view* data, size_t length) override final
		{
			switch (Hash(category))
			{
				case "NUMBER"_h:
				{
					if (length != 4)
						report::compile::critical("NUMBER requires 4 entries");
				
					Number::Settings setting;
				
					for (int i = 0; i < length; i++)
					{
						switch (Hash(data[i]))
						{
						case "Overflow"_h:
							if (setting.limit != Limit::Invalid)
								report::compile::critical("NUMBER limit already set to {}.", magic_enum::enum_name(setting.limit));
							setting.limit = Limit::Overflow;
							break;
						case "Bound"_h:
							if (setting.limit != Limit::Invalid)
								report::compile::critical("NUMBER limit already set to {}.", magic_enum::enum_name(setting.limit));
							setting.limit = Limit::Bound;
							break;
						case "Infinite"_h:
							if (setting.limit != Limit::Invalid)
								report::compile::critical("NUMBER limit already set to {}.", magic_enum::enum_name(setting.limit));
							setting.limit = Limit::Infinite;
							break;

						case "Bit"_h :
							if (setting.size != Size::Invalid)
								report::compile::critical("NUMBER size already set to {}.", magic_enum::enum_name(setting.size));
							setting.size = Size::Bit;
							break;
						case "Byte"_h:
							if (setting.size != Size::Invalid)
								report::compile::critical("NUMBER size already set to {}.", magic_enum::enum_name(setting.size));
							setting.size = Size::Byte;
							break;
						case "Word"_h:
							if (setting.size != Size::Invalid)
								report::compile::critical("NUMBER size already set to {}.", magic_enum::enum_name(setting.size));
							setting.size = Size::Word;
							break;
						case "DWord"_h:
							if (setting.size != Size::Invalid)
								report::compile::critical("NUMBER size already set to {}.", magic_enum::enum_name(setting.size));
							setting.size = Size::DWord;
							break;
						case "QWord"_h:
							if (setting.size != Size::Invalid)
								report::compile::critical("NUMBER size already set to {}.", magic_enum::enum_name(setting.size));
							setting.size = Size::QWord;
							break;

						case "Signed"_h:
							if (setting.size != Size::Invalid)
								report::compile::critical("NUMBER sign already set to {}.", magic_enum::enum_name(setting.sign));
							setting.sign = Signage::Signed;
							break;

						case "Unsigned"_h:
							if (setting.size != Size::Invalid)
								report::compile::critical("NUMBER size already set to {}.", magic_enum::enum_name(setting.sign));
							setting.sign = Signage::Unsigned;
							break;


						case "Integral"_h:
							if (setting.type != NumeralType::Invalid)
								report::compile::critical("NUMBER type already set to {}.", magic_enum::enum_name(setting.type));
							setting.type = NumeralType::Integral;
							break;

						case "Floating"_h:
							if (setting.type != NumeralType::Invalid)
								report::compile::critical("NUMBER size already set to {}.", magic_enum::enum_name(setting.type));
							setting.type = NumeralType::Floating;
							break;


						default:
							report::compile::critical("NUMBER unknown number entry '{}' found.", data[i]);
						};
					}

					return setting.GetOffset();
				}
				



				default: break;
			}

			return HandleExtraOffsetArgs(category, data, length);
		}


		static DefaultClient* GetInstance()
		{
			//This should be ensured not to be called before initialize is called.
			
			if (Initializer::Finished() && !_client)
				_client = new DefaultClient;

			return _client;

		}

		static void SetInstance(DefaultClient* client)
		{
			if (!_client)
				_client = client;
		}

	private:
		inline static DefaultClient* _client = nullptr;
	};

}