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


	enum struct LogState
	{
		None,
		Prep,
		Log,
		Except,
	};



	struct LogParams
	{	
		IssueType				type;		//Issue type. Can only be changed during prep
		IssueLevel				level;		//Issue level. Can only be changed during prep
		ReportType				to;			//what we're going to log to. Can only be changed during prep
		std::string_view		message;	//The primary message to show
		std::string_view		note;		//The message to be shown.
		std::ostringstream		prefix;
		std::ostringstream		suffix;
		spdlog::source_loc		loc;

		LogParams() = default;

		LogParams(const LogParams& other)
		{
			type = other.type;
			level = other.level;
			to = other.to;
			message = other.message;
			note = other.note;
			prefix << other.prefix.view();
			suffix << other.suffix.view();
			loc = other.loc;
		}
	};
	REQUIRED_SIZE(LogParams, 528);

	enum struct LogResult
	{
		Show,
		DontShow,
		Stop
	};


	using LogEditor = void(LogParams&, LogState, LogResult&);

	//I would like to have an object be a handler for this.
	//TODO: Put inside cpp
	using thread_hash = uint64_t;
	using LogEditorID = uint32_t;


	struct EditorHandle;


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
				[[nodiscard]] virtual EditorHandle AddEditor(std::function<LogEditor> editor) = 0;
				virtual void RemoveEditor(EditorHandle& id) = 0;
			};
		}

		CURRENT_VERSION(ReportManager, 1);
	}

	struct IMPL_SINGLETON(ReportManager)
	{
	public:
		friend EditorHandle;//TODO: EditorHandle needs to be moved or handled properly.

		std::string_view GetIssueMessage(IssueCode code, bool translation) override;
		
		IssueType GetIssueType() override;
		void SetIssueType(std::optional<IssueType> type) override;
		void SendReport(IssueType type, IssueLevel level, ReportType to, IssueCode code,
			std::string_view main_view, std::string_view trans_view, spdlog::source_loc& loc, Outlogger* = detail::OutLog) override;
	
	INTERNAL:
		//I would like some versions of these that target specific functions, as a form of ease of use.
		EditorHandle AddEditor(std::function<LogEditor> editor) override;
		void RemoveEditor(EditorHandle& id) override;
	};



	
	struct EditorHandle
	{
	INTERNAL:
		size_t _id = 0;//Might use uint and an optional value.
	
		void Release()
		{
			if (_id) {
				ReportManager::instance->RemoveEditor(*this);
			}

			_id = 0;
		}

	public:
		auto id() const
		{
			return _id;
		}

		constexpr EditorHandle(size_t i) : _id {i}{}
		EditorHandle(const EditorHandle&) = delete;
		EditorHandle& operator=(const EditorHandle&) = delete;
		
		EditorHandle(EditorHandle&& other)
		{
			Release();
			std::swap(_id, other._id);
		}
		EditorHandle& operator=(EditorHandle&& other)
		{
			Release();
			std::swap(_id, other._id);
			return *this;
		}

		~EditorHandle()
		{
			Release();
		}
	};
	REQUIRED_SIZE(EditorHandle, 0x8);
}