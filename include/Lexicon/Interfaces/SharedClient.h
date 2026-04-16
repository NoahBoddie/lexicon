#pragma once

#include "Lexicon/Interfaces/ProjectClient.h"

namespace LEX
{

	//TODO: instead of deriving from the default client, I'd like to have the default client be able to register
	// messages and such.


	using MessageFn = bool(*)(
		const std::string_view& msg, 
		std::span<RuntimeVariable> args, 
		ProjectClient* sender);

	//Make this the shared client. Make it an interface that can be used as well. mainly for adding formats and messages


	struct SharedClient : public ProjectClient, public InterfaceSingleton<SharedClient>
	{
		std::string_view GetCompileOptions(size_t index) override;
		bool HandleFormat(IScript* script, const std::string_view& format, const std::string_view& content) override;
		bool AddFormatter(const std::string_view& format, const std::string_view& owner_name, ScriptFormatFn func) override;

		bool RecieveMessage(const std::string_view& msg, std::span<RuntimeVariable> args, ProjectClient* sender) override;


	INTERNAL:

		bool AddFormatter(const std::string_view& format, ScriptFormatFn func);
		void AddCompileOptions(const std::string& option);
		void AddMessenger(MessageFn messenger);
	};
}