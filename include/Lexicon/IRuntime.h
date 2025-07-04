#pragma once

#include "Lexicon/TypeAliases.h"


namespace LEX
{

	struct Function;
	struct RuntimeVariable;
	class Runtime;
	struct ITemplateBody;


	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(IRuntime)
			{
				virtual void Log(std::source_location loc, IssueLevel level, const std::string_view& message) const = 0;

				virtual IRuntime* GetParent() const = 0;


				virtual const Function* GetFunction() const = 0;

				virtual RuntimeVariable* GetDefault() const = 0;
				
				//virtual void Message(const std::string_view& msg, IssueLevel lvl = IssueLevel::Trace, std::source_location = std::source_location::current()) const = 0;

				//These will honestly do nothing for a long time, but I'll make them at some point.
				virtual Column GetColumn() const = 0;
				virtual Line GetLine() const = 0;
				virtual std::string_view GetFile() const = 0;

				virtual ITemplateBody* AsBody() = 0;
			
			};
		}

		CURRENT_VERSION(IRuntime, 1);
	}

	struct IMPL_VERSION(IRuntime) 
	{

		template<typename... Ts>
		void Report(std::source_location loc, IssueLevel lvl, std::string message, Ts&&... args) const {
			return Log(loc, lvl, fmt::vformat(message, fmt::make_format_args(args...)));
		}

		template<typename... Ts>
		void Report(IssueLevel lvl, SourceAndProxy<std::string> msg, Ts&&... args) const {
			return Log(msg.src, lvl, fmt::vformat(msg.prox, fmt::make_format_args(args...)));
		}


		template<typename... Ts>
		void Report(SourceAndProxy<std::string> msg, Ts&&... args) const {
			return Log(msg.src, IssueLevel::Trace, fmt::vformat(msg.prox, fmt::make_format_args(args...)));
		}


#define DECLARE_REPORT_LEVEL(mc_name, mc_level,...)																	\
	template <typename... Ts>																						\
	__VA_ARGS__ void mc_name(SourceAndProxy<std::string> message, Ts&&... args){									\
		return Log(message.src, IssueLevel::mc_level, fmt::vformat(message.prox, fmt::make_format_args(args...)));	\
	}																												\
	template<typename... Ts>																						\
	__VA_ARGS__ void mc_name(std::source_location loc, std::string message, Ts&&... args) const {					\
		return Log(loc, IssueLevel::mc_level, fmt::vformat(message, fmt::make_format_args(args...)));				\
	}




#define DECLARE_ALL_REPORT_LEVELS							\
	DECLARE_REPORT_LEVEL(Trace, Trace)						\
	DECLARE_REPORT_LEVEL(Info, Info)						\
	DECLARE_REPORT_LEVEL(Debug, Debug)						\
	DECLARE_REPORT_LEVEL(Warn, Warning)						\
	DECLARE_REPORT_LEVEL(Fail, Failure)						\
	DECLARE_REPORT_LEVEL(Error, Error, [[noreturn]])		\
	DECLARE_REPORT_LEVEL(Critical, Critical, [[noreturn]])

		DECLARE_ALL_REPORT_LEVELS;


#undef DECLARE_REPORT_LEVEL
#undef DECLARE_ALL_REPORT_LEVELS


	};

}