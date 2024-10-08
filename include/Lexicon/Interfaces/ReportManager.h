#pragma once


#include "Lexicon/Versioning.h"
//This is ignoring the precompiled header for some fuck all reason.
#include "Lexicon/Interfaces/Interface.h"
#include "Lexicon/Interfaces/InterfaceSingleton.h"

namespace LEX
{

	enum struct ReportType
	{
		None,			//The logger is undecided, default is plugin.
		Main,			//The log goes to the main logger
		General,		//The log goes to the general scripts area
		Script,			//The log goes to the script owners area
		Plugin,			//Log goes to the owning plugin
	};

	//TODO: Turn log mutator parameters into a struct, this is pretty annoying to remember.
	
	
	using LogMutator_V1 = bool(IssueType&, IssueLevel&, std::string&, ReportType&, spdlog::source_loc&);

	//This will need to be changed. String cannot work.
	struct LogData
	{
		IssueType			type;
		IssueLevel			level;
		std::string			message;
		ReportType			to;
		spdlog::source_loc	loc;
	};

	using LogMutator = bool(LogData&);



	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(ReportManager)
			{

			public:
				static void OutLog(spdlog::source_loc& loc, spdlog::level::level_enum level, std::string_view message)
				{
					//This enables logs to be sent back to where they came from.
					constexpr fmt::format_string<std::string_view> a_fmt{ "{}" };

					spdlog::log(loc, level, a_fmt, std::forward<std::string_view>(message));

				}

				
			public:
				using Outlogger = decltype(OutLog)*;

				virtual std::string_view GetIssueMessage(IssueCode code) = 0;

				virtual IssueType GetIssueType() = 0;

				virtual void SetIssueType(std::optional<IssueType> type) = 0;

				virtual void SendReport(IssueType type, IssueLevel level, ReportType to, IssueCode code, std::string_view message, spdlog::source_loc& loc, Outlogger = OutLog) = 0;

			INTERNAL:
				[[nodiscard]] virtual size_t AddMutator(std::function<LogMutator> mutator, bool preheader) = 0;
				virtual void RemoveMutator(size_t id) = 0;
			};
		}

		CURRENT_VERSION(ReportManager, 1);
	}

	struct ScopedLogger;

	struct IMPL_SINGLETON(ReportManager)
	{
	public:
		friend ScopedLogger;//TODO: Scoped logger needs to be moved or handled properly.

		std::string_view GetIssueMessage(IssueCode code) override;
		
		IssueType GetIssueType() override;
		void SetIssueType(std::optional<IssueType> type) override;
		void SendReport(IssueType type, IssueLevel level, ReportType to, IssueCode code, std::string_view message, spdlog::source_loc& loc, Outlogger = OutLog) override;
	
	INTERNAL:
		size_t AddMutator(std::function<LogMutator> mutator, bool preheader) override;
		void RemoveMutator(size_t id) override;
	};


	struct ScopedLogger
	{
		size_t id = 0;

		ScopedLogger(std::function<LogMutator> mut, bool preheader) : id { ReportManager::instance->AddMutator(mut, preheader) }
		{
			return;
		}

		~ScopedLogger()
		{
			ReportManager::instance->RemoveMutator(id);
		}
	};

}