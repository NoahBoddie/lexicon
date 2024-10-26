#include "Lexicon/Interfaces/ReportManager.h"

#include "Lexicon/Engine/IssueTable.h"

namespace LEX
{
	static inline thread_local std::optional<IssueType> Type = std::nullopt;

	IssueType ReportManager::GetIssueType() { return Type.value_or(IssueType::Message); }
	void ReportManager::SetIssueType(std::optional<IssueType> type) { Type = type; }


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

	std::string_view ReportManager::GetIssueMessage(IssueCode code)
	{
		return IssueTable::GetIssueMessage(code.value);
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


	size_t nextId = 0;
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


/*
	bool MutateReport(IssueType& type, IssueLevel& level, std::string& message, ReportType& to, spdlog::source_loc& loc, bool preheader)
	{

		std::hash<std::thread::id> hasher{};

		size_t hash = hasher(std::this_thread::get_id());


		IssueType _type = type;

		if (auto it = checkList.find(hash); checkList.end() != it) {
			for (auto& [id, before, func] : it->second) {

				if (preheader == before && func(preheader ? _type : type, level, message, to, loc) == false) {
					return false;
				}
			}
		}

		return true;
	}
//*/



	bool MutateReport(LogData& data, bool preheader)
	{
		//*

		std::hash<std::thread::id> hasher{};

		size_t hash = hasher(std::this_thread::get_id());


		if (auto it = checkList.find(hash); checkList.end() != it) {
			for (auto& [id, before, func] : it->second) {

				if (preheader == before && func(data) == false) {
					return false;
				}
			}
		}
		//*/
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

}