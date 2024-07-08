#pragma once


#include "Lexicon/Exception.h"

namespace LEX
{
	//TODO: Please relocate "is_not"
	template <typename This, typename Not>
	concept is_not = !std::is_same_v<std::remove_reference_t<std::remove_pointer_t<This>>, Not>;


	ENUM(IssueType, uint8_t)
	{
		Parse,    //A fundemental syntax issue possibly preventing the rest from being read properly.
		Compile,  //An error that occurs trying to use data as provided
		Link,     //An error that occurs trying to connect or look up elements
		Runtime,  //An issue occuring at runtime, undetectable at compile time.
		Fault,    //An issue in C++ implementation, often fatal.
		Apply,    //An issue occuring pre runtime, causing issues starting a routine or call.
		Message,
		Total
	};

	ENUM(IssueLevel, uint8_t){
		Trace,
		Info,
		Debug,
		Warning,
		Error,
		Critical,  //Functionally the same as error, but in some situations where error won't send n exception this will
		Total
	};
	using IssueCode = uint16_t;  //lynchpin this by removing it.

	using LChar = char;
	using LString = std::basic_string<LChar>;

	template <typename... Ts>
	using LFormatString = std::basic_format_string<LChar, Ts...>;
	//IssueCodes will come with names, at a later point, needing to be picked from a list somewhere, that
	// isn't the table for codes. That table only cares about the numbers.
	// But if one tries to print a code it will tell it's name.

	struct IssueTable
	{
		constexpr static const LChar* npos = "";

		using _Table = std::unordered_map<size_t, LChar*>;

		inline static _Table _table;

		static const LChar* GetIssueMessage(IssueCode code)
		{
			auto it = _table.find(code);
			return it != _table.end() ? it->second : nullptr;
		}
	};

	struct Issue
	{
		std::string_view name = "";  //debug name for when the path is not found.
		size_t code = 0;

		constexpr Issue(std::string_view _n, uint16_t _c) :
			name{ _n }, code{ _c } {}
	};

	template <typename T>
	struct SourceAndProxy
	{
		T prox;
		std::source_location src;
		template <std::convertible_to<T> To>
		SourceAndProxy(To&& p, std::source_location s = std::source_location::current()) :
			src{ s },
			prox{ static_cast<T>(p) }
		{}
	};

	struct report
	{
	protected:
		static inline LString CreateArgSlots(size_t size)
		{
			if (!size)
				return "";

			constexpr const LChar* genericSlot = "{}";

			LString result = genericSlot;

			for (auto i = 1; i < size; i++) {
				result += ", "s + genericSlot;
			}

			return result;
		}

		template <IssueLevel Level>
		static inline void TryThrow(IssueType type, LString& message)
		{
			//This should through differently depending on what type it is, and if it's fatal it should be
			// a fault regardless.

			//This might take a string as well.

			if constexpr (Level == IssueLevel::Error) {
				//specific crash
				if (Type == IssueType::Compile) {
					throw CompileError("Not implemented");
				} else if (Type == IssueType::Link) {
					throw LinkError("Not implemented");
				} else if (Type == IssueType::Parse) {
					throw ParseError("Not implemented");
				} else if (Type == IssueType::Runtime) {
					throw RuntimeError("Not implemented");
				} else {
					throw Error("Not implemented");
				}
			} else if constexpr (Level == IssueLevel::Critical) {
				//Just the fatal crash, doesn't matter what type it is.
				throw FatalError("Not implemented");
			}
		}

		template <IssueLevel Level>
		static constexpr spdlog::level::level_enum get_spdlog_level()
		{
			if constexpr (Level == IssueLevel::Critical)
				return spdlog::level::level_enum::critical;
			else if constexpr (Level == IssueLevel::Debug)
				return spdlog::level::level_enum::debug;
			else if constexpr (Level == IssueLevel::Error)
				return spdlog::level::level_enum::err;
			else if constexpr (Level == IssueLevel::Info)
				return spdlog::level::level_enum::info;
			else if constexpr (Level == IssueLevel::Warning)
				return spdlog::level::level_enum::warn;
			else if constexpr (Level == IssueLevel::Trace)
				return spdlog::level::level_enum::trace;
			else
				return spdlog::level::level_enum::trace;
		}

		template <IssueLevel Level>
		static void RaiseMessage(LString& message, std::source_location& loc)
		{
			constexpr fmt::format_string<std::string> a_fmt{ "{}" };

			logger::InitializeLogging();

			//logging level should depend on what this is.

			constexpr spdlog::level::level_enum spdlog_level = get_spdlog_level<Level>();

			spdlog::log(spdlog::source_loc{
							loc.file_name(),
							static_cast<int>(loc.line()),
							loc.function_name() },
				spdlog_level, a_fmt, std::forward<LString>(message));
		}

		template <typename... Ts>
		static void ValidateMessage(LString& message, Ts&... args)
		{
			try {
				message = std::vformat(message, std::make_format_args(args...));
			} catch (std::format_error& f_error) {
				constexpr auto size = sizeof...(Ts);

				message = "INVALID_FORMAT({})"s;

				if constexpr (size != 0) {
					message += " Args("s + CreateArgSlots(size) + ")";
				}

				auto what = f_error.what();
				//This should include what was going to be used maybe?
				message = std::vformat(message, std::make_format_args(what, args...));
			}
		}

		static void HeaderMessage(LString& message, IssueType type, IssueLevel level, IssueCode code)
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

			} else {
				LChar str[5];
				snprintf(str, 5, "%04d", number);
				result += str;
			}
			result += ": ";

			message.insert(0, result);
		}

		template <IssueLevel Level, is_not<std::source_location>... Ts>
		static void LogBase(LString& message, IssueCode code, IssueType type, std::source_location& loc, Ts&... args)
		{
		}

		template <IssueLevel Level, is_not<std::source_location>... Ts>
		static void Log(LString& message, std::source_location& loc, IssueType type, Ts&... args)
		{
			ValidateMessage(message, args...);

			HeaderMessage(message, type, Level, 0);

			RaiseMessage<Level>(message, loc);

			TryThrow<Level>(type, message);
		}

		template <IssueLevel Level, is_not<std::source_location>... Ts>
		static void Log(IssueCode code, std::source_location& loc, IssueType type, Ts&... args)
		{
			LString result;

			const LChar* message = IssueTable::GetIssueMessage(code);

			//LString argSlots;

			if (!message) {
				result = "MISSING_ISSUE";

				constexpr auto size = sizeof...(Ts);

				if constexpr (size != 0) {
					result += " Args("s + CreateArgSlots(size) + ").";
				}

			} else {
				result = message;
			}

			ValidateMessage(result, args...);

			HeaderMessage(result, type, Level, code);

			RaiseMessage<Level>(result, loc);

			TryThrow<Level>(type, result);
		}

	public:

#define DECLARE_LOGGER_LEVEL(mc_name, mc_level,...)													\
	template <is_not<std::source_location>... Ts>													\
	__VA_ARGS__ static void mc_name(SourceAndProxy<std::string> message, Ts&&... args)              \
	{																								\
		return Log<IssueLevel::mc_level>(message.prox, message.src, GetIssueType(), args...);		\
	}																								\
	template <is_not<std::source_location>... Ts>													\
	__VA_ARGS__ static void mc_name(std::string& message, std::source_location loc, Ts&&... args)   \
	{																								\
		return Log<IssueLevel::mc_level>(message, loc, GetIssueType(), args...);					\
	}																								\
	template <is_not<std::source_location>... Ts>													\
	__VA_ARGS__ static void mc_name(std::string&& message, std::source_location loc, Ts&&... args)  \
	{																								\
		return mc_name(message, loc, args...);														\
	}																								\
	template <is_not<std::source_location>... Ts>													\
	__VA_ARGS__ static void mc_name(SourceAndProxy<IssueCode> code, Ts&&... args)					\
	{																								\
		return Log<IssueLevel::mc_level>(code.prox, code.src, GetIssueType(), args...);				\
	}																								\
	template <is_not<std::source_location>... Ts>													\
	__VA_ARGS__ static void mc_name(IssueCode code, std::source_location loc, Ts&&... args)			\
	{																								\
		return Log<IssueLevel::mc_level>(code, loc, GetIssueType(), args...);						\
	}

#define DECLARE_ALL_LOGGER_LEVELS()							\
	DECLARE_LOGGER_LEVEL(trace, Trace)						\
	DECLARE_LOGGER_LEVEL(info, Info)						\
	DECLARE_LOGGER_LEVEL(debug, Debug)						\
	DECLARE_LOGGER_LEVEL(warn, Warning)						\
	DECLARE_LOGGER_LEVEL(error, Error, [[noreturn]])		\
	DECLARE_LOGGER_LEVEL(critical, Critical, [[noreturn]])

#define DECLARE_LOGGER_TYPE(mc_name, mc_type)                                             \
	struct mc_name                                                                        \
	{                                                                                     \
		constexpr static IssueType GetIssueType() noexcept { return IssueType::mc_type; } \
		DECLARE_ALL_LOGGER_LEVELS()                                                       \
	};

		DECLARE_ALL_LOGGER_LEVELS()

		DECLARE_LOGGER_TYPE(apply, Apply)
		DECLARE_LOGGER_TYPE(compile, Compile)
		DECLARE_LOGGER_TYPE(fault, Fault)
		DECLARE_LOGGER_TYPE(link, Link)
		DECLARE_LOGGER_TYPE(message, Message)
		DECLARE_LOGGER_TYPE(runtime, Runtime)
		DECLARE_LOGGER_TYPE(parse, Parse)

#undef DECLARE_LOGGER_LEVEL
#undef DECLARE_LOGGER_TYPE
#undef DECLARE_ALL_LOGGER_LEVELS
		
#define BREAK_LOGGER(mc_name, ...) mc_name(__VA_ARGS__), (IsDebuggerPresent() ? __debugbreak() : void())

//Need to ensure this only happens if a debugger is active.
#define break_trace(...) BREAK_LOGGER(trace, __VA_ARGS__)
#define break_debug(...) BREAK_LOGGER(debug, __VA_ARGS__)
#define break_info(...) BREAK_LOGGER(info, __VA_ARGS__)
#define break_warn(...) BREAK_LOGGER(warn, __VA_ARGS__)
#define break_error(...) BREAK_LOGGER(error, __VA_ARGS__)
#define break_critical(...) BREAK_LOGGER(critical, __VA_ARGS__)

	private:
		//loose type should be named some common ground that will make it easier to use for macros.
		static inline thread_local std::optional<IssueType> Type = std::nullopt;

		static IssueType GetIssueType() { return Type.value_or(IssueType::Message); }

		std::optional<IssueType> _prev;

	public:
		//Report itself serves as class that sets what loose type is currently being used, and then undoes it once when it's over.
		// Optionally, it should maybe come with an optional state so it can turn itself off if invalid.
		report(IssueType set) :
			_prev{ Type }
		{
			Type = set;
		}

		~report()
		{
			Type = _prev;
		}
	};


	
	template <is_not<std::source_location>... Ts>
	void a_oldreporttest(SourceAndProxy<std::string> message, Ts&&... args)
	{
		//LEX::report::runtime::debug(message.prox, message.src, args...);
	}

	template <is_not<std::source_location>... Ts>
	void a_oldreporttest(std::string message, std::source_location loc, Ts&&... args)
	{
		//LEX::report::runtime::debug(message, loc, args...);
	}


}