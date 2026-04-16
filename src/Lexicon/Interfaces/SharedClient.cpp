#include "Lexicon/Interfaces/SharedClient.h"

#include "Lexicon/RuntimeVariable.h"
#include "Lexicon/Engine/Repository.h"
namespace LEX
{
	std::vector<std::string> options;
	std::vector<MessageFn> messengers;
	std::unordered_map<std::string, ScriptFormatFn> formatters;



	bool SharedClient::RecieveMessage(const std::string_view& msg, std::span<RuntimeVariable> args, ProjectClient* sender)
	{
		return false;
	}


	bool SharedClient::HandleFormat(IScript* script, const std::string_view& format, const std::string_view& content)
	{
		auto it = formatters.find(std::string{ format });

		if (formatters.end() == it) {
			report::compile::failure("Failed to find format for '{}'", format);
			return false;
		}

		auto func = it->second;

		return func(script, format, content);
	}

	bool SharedClient::AddFormatter(const std::string_view& format, ScriptFormatFn func)
	{
		auto& entry = formatters[std::string{ format }];

		assert_if(entry) {
			report::compile::warn("'{}' already filled.", format);
			return false;
		}

		entry = func;

		return true;
	}


	bool SharedClient::AddFormatter(const std::string_view& format, const std::string_view& owner_name, ScriptFormatFn func)
	{
		constexpr std::string_view shared_str = "Shared";

		if (!owner_name.empty() == true) {
			report::compile::failure("Owner name of shared format '{}' is empty.", format);
			return false;
		}

		
		if (owner_name == shared_str) {
			report::compile::failure("Shared format '{}' cannot be owned directly by the shared repository.", format);
			return false;
		}


		std::string full_name = std::format("{}::{}", owner_name, format);

		return AddFormatter(full_name, func);
		
	}



	void SharedClient::AddCompileOptions(const std::string& option)
	{
		options.push_back(option);
	}

	void SharedClient::AddMessenger(MessageFn messenger)
	{
		messengers.push_back(messenger);
	}

	std::string_view SharedClient::GetCompileOptions(size_t index)
	{
		if (options.size() > index)
			return options[index];

		return {};
	}
}