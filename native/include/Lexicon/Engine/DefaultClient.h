#pragma once

#include "Lexicon/TypeID.h"
#include "Lexicon/Interfaces/ProjectClient.h"

#include "Lexicon/Number.h"

namespace LEX
{

	//TODO: instead of deriving from the default client, I'd like to have the default client be able to register
	// messages and such.


	using MessageFn = void(*)(uint64_t severity, std::string_view message, ProjectClient* sender);
	using ScriptFormatFn = bool(*)(Script* script, std::string_view format, std::string_view name, std::string_view content);

	struct DefaultClient final : public ProjectClient
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



		static DefaultClient* GetInstance()
		{
			//This should be ensured not to be called before initialize is called.
			
			//Make synchorized client.

			static DefaultClient singleton;

			return &singleton;

			if (Initializer::Finished() && !_client)
				_client = new DefaultClient;

			return _client;

		}

		static void SetInstance(DefaultClient* client)
		{
			if (!_client)
				_client = client;
		}

		std::string_view GetCompileOptions(size_t index) override
		{
			if (_options.size() > index)
				return _options[index];

			return {};
		}

		void AddCompileOptions(const std::string& option)
		{
			_options.push_back(option);
		}

	private:
		std::vector<std::string> _options;
		std::vector<MessageFn> _messengers;
		std::vector<ScriptFormatFn> _formatterss;

		inline static DefaultClient* _client = nullptr;
	};

}