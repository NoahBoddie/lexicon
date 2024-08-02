#include "Lexicon/Interfaces/ReportManager.h"

namespace LEX
{
	inline std::unordered_map<size_t, std::string_view> issueTable;

	static inline thread_local std::optional<IssueType> Type = std::nullopt;

	IssueType ReportManager::GetIssueType() { return Type.value_or(IssueType::Message); }


	static void HeaderMessage(std::string& message, IssueType type, IssueCode code)
	{
		//might do format instead.
		LString result;

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
			result += "U";
			break;
		}

		uint16_t number = code;

		//"Severity X0000: "

		if (!code) {
			result += "0000";

		}
		else {
			LChar str[5];
			snprintf(str, 5, "%04d", number);
			result += str;
		}
		result += ": ";

		message.insert(0, result);
	}


	static inline void TryThrow(IssueType type, IssueLevel level, std::string_view message)
	{
		//This should through differently depending on what type it is, and if it's fatal it should be
		// a fault regardless.

		//This might take a string as well.


		


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
		auto it = issueTable.find(code);
		return it != issueTable.end() ? it->second : std::string_view{};
	}


	enum ReportResult
	{
		DontShow,
		Continue,
		Stop,
	};

	


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


	void ReportManager::SendReport(IssueType type, IssueLevel level, IssueCode code, std::string_view view, spdlog::source_loc& loc, Outlogger logger)
	{
		//By having this in interface, I can obscure spdlog, enabling spdlog not being a requirement. But in the future. I won't try too hard to do that now.

		constexpr fmt::format_string<std::string_view> a_fmt{ "{}" };
		
		ReportType to = ReportType::None;
		
		logger::InitializeLogging();

		std::string message{ view };

		bool log = true;

		LogData data{ type, level, std::string { view }, to, loc };

		

		if (MutateReport(data, true) == false, type = data.type) {
			//Should still log if it's fatal
			log = false;
		}
		else {
			HeaderMessage(message, type, code);

			//At a later point I'll prevent type from being edited in post.
			//type = data.type;

			if (MutateReport(data, false) == false) {
				log = false;
			}
		}

		if (log && level != IssueLevel::Critical)
		{
			auto spd_lvl = get_spdlog_level(level);

			switch (to)
			{
			case ReportType::Plugin:
				logger(loc, spd_lvl, message);
				break;
			case ReportType::General:
			case ReportType::Script:
			case ReportType::Main:

			default:
				spdlog::log(loc, spd_lvl, a_fmt, std::string_view{ message });
			}
		}

		TryThrow(type, level, message);
	}

}