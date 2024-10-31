#include "Lexicon/Interfaces/ReportManager.h"

#include "Lexicon/Engine/IssueTable.h"

namespace LEX
{
	static inline thread_local std::optional<IssueType> Type = std::nullopt;


	IssueType ReportManager::GetIssueType() { return Type.value_or(IssueType::Message); }
	void ReportManager::SetIssueType(std::optional<IssueType> type) { Type = type; }


	static std::string HeaderMessage(IssueType type, IssueCode code)
	{
		//If there is no code, we don't need to do this. 

		//might do format instead.
		std::string result = "[";

		bool mark_unk = false;

		switch (type) {
		case IssueType::Compile:
			result += "C";
			break;

		case IssueType::Parse:
			result += "P";
			break;

		case IssueType::Link:
			result += "L";
			break;

		case IssueType::Runtime:
			result += "R";
			break;

		case IssueType::Fault:
			result += "F";
			break;

		case IssueType::Apply:
			result += "A";
			break;

		case IssueType::Message:
			result += "M";
			break;

		default:
			mark_unk = true;
			break;
		}

		uint16_t number = code;

		//"Severity X0000: "

		if (code) {
			char str[5];
			snprintf(str, 5, "%04d", number);
			result += str;
		}

		if (mark_unk) {
			result += "?";
		}

		result += "] ";

		return result;
	}


	static inline void TryThrow(IssueType type, IssueLevel level, std::string_view message)
	{
		//This should through differently depending on what type it is, and if it's fatal it should be
		// a fault regardless.

		//This might take a string as well.


		//TODO: With the errors that are through, I think it would be a nice touch to include the issue code when throwing. The exception.
		// This way, we choose if we're alright with the issue or not.


		if (level == IssueLevel::Error) {
			//specific crash

			switch (type)
			{
			case IssueType::Compile:
				throw CompileError("Not implemented");
			case IssueType::Runtime:
				throw RuntimeError("Not implemented");
			case IssueType::Parse:
				throw ParseError("Not implemented");
			case IssueType::Link:
				throw LinkError("Not implemented");

			default:
				throw Error("Not implemented");
			}
		}
		else if (level == IssueLevel::Critical) {
			//Just the fatal crash, doesn't matter what type it is.
			throw FatalError("Not implemented");
		}
	}


	
	static constexpr spdlog::level::level_enum get_spdlog_level(IssueLevel lvl)
	{
		switch (lvl)
		{
		case IssueLevel::Critical:
			return spdlog::level::level_enum::critical;
		case IssueLevel::Debug:
			return spdlog::level::level_enum::debug;
		case IssueLevel::Error:
		case IssueLevel::Failure:
			return spdlog::level::level_enum::err;
		case IssueLevel::Info:
			return spdlog::level::level_enum::info;
		case IssueLevel::Warning:
			return spdlog::level::level_enum::warn;
		
		//case IssueLevel::Trace:
		default:
			return spdlog::level::level_enum::trace;

		
		}

	}

	std::string_view ReportManager::GetIssueMessage(IssueCode code, bool translation)
	{
		return IssueTable::GetIssueMessage(code.value, translation);
	}


	enum ReportResult
	{
		DontShow,
		Continue,
		Stop,
	};

	
	//I really REALLY want to change how the log mutator works. Namely, I'd like to make it so clients can actually maybe mutate logs?
	// At the very least I'd like to be able to edit the log at different stages, namely, for a certain period you can edit the string,
	// and for some other periods you can only edit it for the message that's going to be used.


	LogEditorID nextId = 0;

	/*
	
	static void HeaderMessage(std::string& message, IssueType type, IssueCode code)
	{
		//If there is no code, we don't need to do this. 
		
		//might do format instead.
		LString result = "[";

		bool mark_unk = false;

		switch (type) {
		case IssueType::Compile:
			result += "C";
			break;

		case IssueType::Parse:
			result += "P";
			break;

		case IssueType::Link:
			result += "L";
			break;

		case IssueType::Runtime:
			result += "R";
			break;

		case IssueType::Fault:
			result += "F";
			break;

		case IssueType::Apply:
			result += "A";
			break;

		case IssueType::Message:
			result += "M";
			break;

		default:
			mark_unk = true;
			break;
		}

		uint16_t number = code;

		//"Severity X0000: "

		if (code) {
			LChar str[5];
			snprintf(str, 5, "%04d", number);
			result += str;
		}

		if (mark_unk) {
			result += "?";
		}

		result += "] ";

		message.insert(0, result);
	}



	//straighten this out holy mother of god.
	std::unordered_map<size_t, std::vector<std::tuple<size_t, bool, std::function<LogMutator>>>> checkList;

	std::vector<std::function<LogMutator>> _checkList;

	//I'd lock these but they don't seem to be a hot spot.
	size_t ReportManager::AddMutator(std::function<LogMutator> mutator, bool preheader)
	{
		auto curr_id = nextId++;
		
		std::hash<std::thread::id> hasher{};

		size_t hash = hasher(std::this_thread::get_id());

		checkList[hash].emplace_back(curr_id, preheader, std::move(mutator));

		return curr_id;
	}
	
	void ReportManager::RemoveMutator(size_t id)
	{

		std::hash<std::thread::id> hasher{};

		size_t hash = hasher(std::this_thread::get_id());

		if (auto _it = checkList.find(hash); checkList.end() != _it) {
			auto& vector = _it->second;

			auto end = vector.rend();
			
			//We look backwards because that is the thing that is most likely to request this, due to it primarily being for 
			// when self managing objects close
			auto it =  std::find_if(vector.rbegin(), vector.rend(), [id](auto& it) { return std::get<0>(it) == id; });
			
			if (it != end) {
				if (vector.size() == 1) {
					//If this is the last one, remove entry
					checkList.erase(hash);
				}
				else{
					//If this isn't remove the function.
					vector.erase(std::next(it).base());
					//remove entry
				}

			}
		}
	}





	bool MutateReport(LogData& data, bool preheader)
	{
		

		std::hash<std::thread::id> hasher{};

		size_t hash = hasher(std::this_thread::get_id());


		if (auto it = checkList.find(hash); checkList.end() != it) {
			for (auto& [id, before, func] : it->second) {

				if (preheader == before && func(data) == false) {
					return false;
				}
			}
		}
		
		return true;
	}
	

	void ReportManager::SendReport(IssueType type, IssueLevel level, ReportType to, IssueCode code, std::string_view view, spdlog::source_loc& loc, Outlogger logger)
	{
		//By having this in interface, I can obscure spdlog, enabling spdlog not being a requirement. But in the future. I won't try too hard to do that now.

		constexpr fmt::format_string<std::string_view> a_fmt{ "{}" };
		
		logger::InitializeLogging();

		//std::string message{ view };

		bool log = true;

		LogData data{ type, level, std::string { view }, to, loc };

		//TODO: Around here it would be nice to note when a message has been promoted from it's natural state.
		
		//This doesn't print logs, fix pls.
		log = MutateReport(data, true);
		type = data.type;
		
		if (log || level == IssueLevel::Critical) {
			HeaderMessage(data.message, type, code);
		}

		if (log) {
			log = MutateReport(data, false);
		}

		bool return_back = to & ReportType::Return;

		to &= ~ReportType::Return;

		
		
		if (log || level == IssueLevel::Critical)
		{
			auto spd_lvl = get_spdlog_level(level);

			std::shared_ptr<spdlog::logger> log_ptr;

			switch (to)
			{
			case ReportType::Script:
				if (log_ptr = spdlog::get("script"))
					log_ptr->log(loc, spd_lvl, data.message);

				[[fallthrough]];

			case ReportType::Program:
				if (log_ptr = spdlog::get("program"))
					log_ptr->log(loc, spd_lvl, data.message);

				[[fallthrough]];
				
			case ReportType::Main:
				spdlog::log(loc, spd_lvl, data.message);
				break;

			//default:
				//if (log_ptr = spdlog::get("console"))
				//	log_ptr->log(loc, spd_lvl, data.message);
			}
			
			if (return_back)
				logger(loc, spd_lvl, data.message);
		}

		TryThrow(type, level, data.message);
	}
	//*/
/////////////////////////////


	struct LogEditorEntry
	{
		LogEditorID id{};
		LogState state;
		std::function<LogEditor> muter;

	};


	std::unordered_map<thread_hash, std::vector<LogEditorEntry>> newList;

	LogEditorID ReportManager::AddEditor(std::function<LogEditor> mutator, LogState state)
	{
		if (state == LogState::None)
			return -1;

		auto curr_id = nextId++;

		std::hash<std::thread::id> hasher{};

		thread_hash hash = hasher(std::this_thread::get_id());

		newList[hash].emplace_back(curr_id, state, std::move(mutator));

		return curr_id;
	}

	void ReportManager::RemoveEditor(LogEditorID id)
	{

		std::hash<std::thread::id> hasher{};

		size_t hash = hasher(std::this_thread::get_id());

		if (auto _it = newList.find(hash); newList.end() != _it) {
			auto& vector = _it->second;

			auto end = vector.rend();

			//We look backwards because that is the thing that is most likely to request this, due to it primarily being for 
			// when self managing objects close
			auto it = std::find_if(vector.rbegin(), vector.rend(), [id](auto& it) { return it.id == id; });

			if (it != end) {
				if (vector.size() == 1) {
					//If this is the last one, remove entry
					newList.erase(hash);
				}
				else {
					//If this isn't remove the function.
					vector.erase(std::next(it).base());
					//remove entry
				}

			}
		}
	}


	

	bool EditReport(FixedLogData& fixed, VariedLogData& varied, LogState state)
	{

		//Look, I'm keeping it a buck fifty. This shit is terrible and it does not fucking work right. Specifically when it comes to appending stuff.
		// It things can override each other, but for now, I'm just going to say I don't care.
		
		std::hash<std::thread::id> hasher{};

		thread_hash hash = hasher(std::this_thread::get_id());

		LogResult out = LogResult::Show;

		if (auto it = newList.find(hash); newList.end() != it) {
			for (auto& [id, state_flags, func] : it->second) {
				if (state_flags | state){
					func(fixed, varied, state, out);
					
					//Will only care to stop if it's not an exception.


#pragma warning(suppress: 26813)
					if (out == LogResult::Stop && state != LogState::Except)
						return false;
				}
				
				

			}
		}
	
		return out == LogResult::Show;
	}


	void SendLog(FixedLogData fixed, VariedLogData& varied, ReportType to, bool use_main, std::function<Outlogger> logger)
	{
		constexpr fmt::format_string<std::string_view> a_fmt{ "{}" };

		fixed.to = to;


		std::string prefix;
		std::string suffix;


		//This is what gets applied to the mutate function, with VariedLogData providing its input first, then this one applying on top of that.
		VariedLogData data{ prefix, suffix, varied.loc };


		bool log = EditReport(fixed, data, LogState::Log);

		auto spd_lvl = get_spdlog_level(fixed.level);

		
		if (!log && ReportType::Main != to)
			return;

		std::shared_ptr<spdlog::logger> log_ptr;

		std::string_view message;

		if (use_main) {
			message = fixed.main;
		}
		else {
			message = fixed.trans;
		}

		if (prefix.empty() && suffix.empty())
			logger(data.loc, spd_lvl, message);
		else
			logger(data.loc, spd_lvl, prefix + message.data() + suffix);

	}

	void SendLog(FixedLogData& fixed, VariedLogData& varied, ReportType to, bool use_main, const std::string& logger_name)
	{

		SendLog(fixed, varied, to, use_main, [&](spdlog::source_loc& loc, spdlog::level::level_enum level, std::string_view message)
			{
				std::shared_ptr<spdlog::logger> log_ptr{};

				auto def_log = spdlog::default_logger();

				if (logger_name.empty() == true) {
					log_ptr = def_log;
				}
				else {
					log_ptr = spdlog::get(logger_name);

					if (log_ptr == def_log) {
						return;
					}
				}



				if (log_ptr) {
					log_ptr->log(loc, level, message);
				}
			});

		/*
		constexpr fmt::format_string<std::string_view> a_fmt{ "{}" };

		fixed.to = to;

		auto spd_lvl = get_spdlog_level(fixed.level);


		//This is what gets applied to the mutate function, with VariedLogData providing its input first, then this one applying on top of that.
		VariedLogData data{ varied.loc };

		std::shared_ptr<spdlog::logger> log_ptr;

		std::string_view message;

		if (logger_name.empty() == true) {
			log_ptr = spdlog::default_logger();
		}
		else {
			log_ptr = spdlog::get(logger_name);
		}
		
		if (use_main) {
			message = fixed.main;
		}
		else {
			message = fixed.trans;
		}


		if (log_ptr) {
			log_ptr->log(data.loc, spd_lvl, "{}{}{}{}{}", varied.prefix, data.prefix, message, data.suffix, varied.suffix);
		}
		//*/

	}


	//Look, I'm keeping it a buck fifty. This shit is terrible and it does not fucking work right. Specifically when it comes to appending stuff.
	// It things can override each other, but for now, I'm just going to say I don't care.

	void ReportManager::SendReport(IssueType type, IssueLevel level, ReportType to, IssueCode code,
		std::string_view main_view, std::string_view trans_view, spdlog::source_loc& loc, Outlogger* logger)
	{
		//Making this function more time efficient is an on going goal.

		logger::InitializeLogging();

		

		bool is_critical = level == IssueLevel::Critical;


		std::string prefix;
		std::string suffix;

		FixedLogData fixed{ type, level, to, main_view, trans_view };

		VariedLogData varied{ prefix, suffix, std::move(loc) };

		bool log = EditReport(fixed, varied, LogState::Prep);

		std::string header = HeaderMessage(fixed.type, code);

		//I might have to apply this later, due to a new header having to be put on what with each logger. Sounds like torture.
		std::string main = header + prefix + main_view.data() + suffix;
		std::string	trans = SettingManager::IsEnglish() ? main : header + prefix + trans_view.data() + suffix;
		
		fixed.main = main;
		fixed.trans = trans;

		

		if (is_critical) {
			fixed.level = level;//An issue level of critical cannot be prevented nor should it be avoided. Which is why it should be sparing.
		}
			

		bool return_back = to & ReportType::Return;

		to &= ~ReportType::Return;

		//Around here fixed should be changed to use the new prefixes
		

		switch (fixed.to)
		{
		case ReportType::Script:
			if (log) {
				SendLog(fixed, varied, ReportType::Script, false, "script");
			}
			[[fallthrough]];

		case ReportType::Program:
			if (log) {
				SendLog(fixed, varied, ReportType::Program, false, "program");
			}
			[[fallthrough]];

		default:
			if (!is_critical)//Once more, if critical, we must use the main logger.
				break;
				
			[[fallthrough]];

		case ReportType::Main:
			//Note, that the main logger cannot be stopped if the issue level is large enough to send a crash.
			if (log || is_critical) {
				//TODO: As is, these do not fucking work, by extension all translations. Please address.
				
				SendLog(fixed, varied, ReportType::Main, true, "");

				//I would like to have this report to the console if it's open, or if a debugger is attached.
				if (GetConsoleWindow() != NULL)
					SendLog(fixed, varied, ReportType::Main, false, "console");

			}
			break;
		}


			

		if (return_back)
			//Im unsure if I want to use translated errors when sending stuff back, as these are for 
			SendLog(fixed, varied, ReportType::Return, true, logger);
			
		prefix.clear();
		suffix.clear();

		EditReport(fixed, varied, LogState::Except);

		//Once past the prep state, these are still allowed to be filled and used.
		
		if (prefix.empty() && suffix.empty())
			TryThrow(type, level, trans_view);
		else
			TryThrow(type, level, prefix + trans_view.data() + suffix);
	}


}