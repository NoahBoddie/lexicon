#pragma once


#include "Lexicon/Versioning.h"
//This is ignoring the precompiled header for some fuck all reason.
#include "Lexicon/Interfaces/Interface.h"
#include "Lexicon/Interfaces/InterfaceSingleton.h"

namespace LEX
{

	ENUM(ReportType)
	{
		Main,				//The log goes to the main logger
		Program,			//The log is allowed to given to the program logger
		Script,				//The log allowed to be given to the script logger
		
			
		Return = 1 << 7,	//The log is to return to sender, logging how it sees fit.
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


	ENUM(LogState)
	{
		None	= 0,
		Prep	= 1 << 0,
		Log		= 1 << 1,
		Except	= 1 << 2,
	};

	//Past the prep phase, these are no longer allowed to be edited.
	struct FixedLogData
	{
		//const IssueCode		code;
		IssueType			type;
		IssueLevel			level;
		ReportType			to;
		std::string_view	main;
		std::string_view	trans;
	};

	//Once past the prep state, these are still allowed to be filled and used.
	struct VariedLogData
	{
		api::trans::string		prefix;
		api::trans::string		suffix;
		spdlog::source_loc		loc;


		VariedLogData(std::string& pre, std::string& suf, const spdlog::source_loc& l) : prefix{ pre }, suffix{ suf }, loc{ l }
		{

		}

		VariedLogData(std::string& pre, std::string& suf, spdlog::source_loc&& l) : prefix{ pre }, suffix{ suf }, loc{ l }
		{

		}
	};

	enum struct LogResult
	{
		Show,
		DontShow,
		Stop

	};


	using LogEditor = void(FixedLogData&, VariedLogData&, LogState, LogResult&);

	using thread_hash = uint64_t;

	using LogEditorID = uint32_t;



	namespace detail
	{
		inline static void OutLog(spdlog::source_loc& loc, spdlog::level::level_enum level, std::string_view message)
		{
			//This enables logs to be sent back to where they came from.
			//constexpr fmt::format_string<std::string_view> a_fmt{ "{}" };

			spdlog::log(loc, level, message);

		}
	}
	using Outlogger = decltype(detail::OutLog);


	namespace Version
	{
		namespace _1
		{
			struct INTERFACE_VERSION(ReportManager)
			{
			public:
				
				virtual std::string_view GetIssueMessage(IssueCode code, bool translation) = 0;

				virtual IssueType GetIssueType() = 0;

				virtual void SetIssueType(std::optional<IssueType> type) = 0;

				virtual void SendReport(IssueType type, IssueLevel level, ReportType to, IssueCode code,
					std::string_view main_view, std::string_view trans_view, spdlog::source_loc& loc, Outlogger*) = 0;

			INTERNAL:
				[[nodiscard]] virtual LogEditorID AddEditor(std::function<LogEditor> mutator, LogState state) = 0;
				virtual void RemoveEditor(LogEditorID id) = 0;
			};
		}

		CURRENT_VERSION(ReportManager, 1);
	}

	struct scoped_logger;

	struct IMPL_SINGLETON(ReportManager)
	{
	public:
		friend scoped_logger;//TODO: Scoped logger needs to be moved or handled properly.

		std::string_view GetIssueMessage(IssueCode code, bool translation) override;
		
		IssueType GetIssueType() override;
		void SetIssueType(std::optional<IssueType> type) override;
		void SendReport(IssueType type, IssueLevel level, ReportType to, IssueCode code,
			std::string_view main_view, std::string_view trans_view, spdlog::source_loc& loc, Outlogger* = detail::OutLog) override;
	
	INTERNAL:
		LogEditorID AddEditor(std::function<LogEditor> mutator, LogState state) override;
		void RemoveEditor(LogEditorID id) override;
	};


	struct scoped_logger
	{
		LogEditorID id = 0;


		scoped_logger(std::function<LogEditor> mut, LogState state) : id { ReportManager::instance->AddEditor(mut, state) }
		{
			
		}
		

		~scoped_logger()
		{
			ReportManager::instance->RemoveEditor(id);
		}
	};

}