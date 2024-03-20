#pragma once


#include "Exception.h"

namespace LEX
{

	ENUM(IssueType, uint8_t)
	{
		Parse,				//A fundemental syntax issue possibly preventing the rest from being read properly.
		Compile,			//An error that occurs trying to use data as provided
		Link,				//An error that occurs trying to connect or look up elements
		Runtime,			//An issue occuring at runtime, undetectable at compile time.
		Fault,				//An issue in C++ implementation, often fatal.
		Apply,				//An issue occuring pre runtime, causing issues starting a routine or call.
		Total
	};

	ENUM(IssueLevel, uint8_t)
	{
		Info,
		Debug,
		Warning,
		Error,
		Critical,	//Functionally the same as error, but in some situations where error won't send n exception this will
		Fatal,
		Total
	};


	using IssueCode = uint16_t;//lynchpin this by removing it.

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

		using _Table = std::array<std::unordered_map<size_t, LChar*>, IssueType::Total>;

		inline static _Table _table;


		static const LChar* GetIssueMessage(IssueType type, IssueCode code)
		{
			//use find
			return _table[type][code];
		}
	};

	struct Issue
	{
		std::string_view	name = "";//debug name for when the path is not found.
		size_t				code = 0;

		constexpr Issue(std::string_view _n, uint16_t _c) :
			name{ _n }, code{ _c } {}
	};

#define DECLARE_ISSUE(mc_name, mc_code) Issue mc_name(#mc_name, mc_code)

	//ex. //DECLARE_ISSUE(Test, 49);



	//where would you store these?



	//Turns the issue data into a header for the message.
	inline LString IssueToString(IssueType type, IssueCode code, IssueLevel level)
	{
		//might do format instead.
		LString result;

		switch (level)
		{
		case IssueLevel::Debug:
			result = "Debug "; break;

		case IssueLevel::Error:
			result = "Error "; break;

		case IssueLevel::Fatal:
			result = "Fatal "; break;

		case IssueLevel::Warning:
			result = "Warn "; break;
		}

		switch (type)
		{
		case IssueType::Compile:
			result += "C"; break;

		case IssueType::Parse:
			result += "P"; break;

		case IssueType::Link:
			result += "L"; break;

		case IssueType::Runtime:
			result += "R"; break;

		case IssueType::Fault:
			result += "F"; break;

		case IssueType::Apply:
			result += "A"; break;
		}

		uint16_t number = code;

		//"Severity X0000: "

		if (!code)
		{
			result += "0000";

		}
		else
		{

			char str[5];
			snprintf(str, 5, "%04d", number);
			result += str;
		}



		return result + ": ";
	}

	//I think there shouldn't be a set place where issues are stored, at least not as an object like this.
	//Rather, I think issues like this shouldn't be the focus, and the result

	inline LString _CreateArgSlots(size_t size)
	{
		if (!size)
			return "";

		constexpr const LChar* genericSlot = "{}";

		LString result = genericSlot;

		for (auto i = 1; i < size; i++)
		{
			result += ", "s + genericSlot;
		}

		return result;
	}




	namespace detail
	{

		inline void _ThrowReport(LEX::IssueLevel Level, LEX::IssueType Type, LString message)
		{
			//This should through differently depending on what type it is, and if it's fatal it should be
			// a fault regardless.

			//This might take a string as well.

			if (Level == LEX::IssueLevel::Error || Level == LEX::IssueLevel::Critical)
			{
				if (Type == LEX::IssueType::Compile && Level != LEX::IssueLevel::Error)
				{
					throw CompileError("Not implemented");
				}
				else if (Type == LEX::IssueType::Link)
				{
					throw LinkError("Not implemented");
				}
				else if (Type == LEX::IssueType::Parse)
				{
					throw ParseError("Not implemented");
				}
				else if (Type == LEX::IssueType::Runtime)
				{
					throw RuntimeError("Not implemented");
				}
				//No exception found.

				//unhandled exception type.
			}
			else if (Level == LEX::IssueLevel::Fatal)
			{
				//throw fatal error
				throw nullptr;
			}

		}



		//this shouldn't be in detail
		template <typename... Ts>
		void _raise_message(LEX::IssueLevel level, LEX::IssueType type, LString& message, bool has_header, std::source_location loc, Ts&... ts)
		{
			//insist that ts... can be formated.
			constexpr auto size = sizeof...(Ts);


			LString base = has_header ? "" : IssueToString(type, 0, level);
			LString body = base + message;


			LString result;


			try
			{
				result = std::vformat(body, std::make_format_args(ts...));
			}
			catch (std::format_error& f_error)
			{
					
				body = base + "Invalid format string(\'{}\')"s + " Args(" + _CreateArgSlots(size) + ")";

				result = std::vformat(body, std::make_format_args(f_error.what(), ts...));
			}

			body += " Args("s + _CreateArgSlots(size) + ")";

			//needs fix

			constexpr fmt::format_string<std::string> a_fmt{ "{}" };
			
			logger::InitializeLogging();

			//logging level should depend on what this is.

			spdlog::level::level_enum spdlog_level;


			switch (level)
			{
			case IssueLevel::Debug:
				spdlog_level = type == IssueType::Runtime ? spdlog::level::info : spdlog::level::debug; break;

			case IssueLevel::Error:
			case IssueLevel::Critical:
				spdlog_level = spdlog::level::err; break;

			case IssueLevel::Warning:
				spdlog_level = spdlog::level::warn; break;

			case IssueLevel::Fatal:
				spdlog_level = spdlog::level::critical; break;

			default:
				spdlog_level = spdlog::level::trace; break;
			}


			spdlog::log(spdlog::source_loc{
						loc.file_name(),
						static_cast<int>(loc.line()),
						loc.function_name() },
						spdlog_level, a_fmt, std::forward<LString>(result));


			_ThrowReport(level, type, result);
		}

		template <typename... Ts>
		void _raise_code(LEX::IssueLevel level, LEX::IssueType type, IssueCode code, std::source_location loc, Ts&... ts)
		{
			//insist that ts... can be formated.
			constexpr auto size = sizeof...(Ts);


			LString base = IssueToString(type, code, level);
			LString body;

			const LChar* message = IssueTable::GetIssueMessage(type, code);

			//LString argSlots;

			if (!message)
			{
				//If there is no message, send the "no message" message. Gives code

				body = base + "No message for issue code.";

				if constexpr (size != 0)
				{
					body += " Args("s + _CreateArgSlots(size) + ").";
				}
			}
			else
			{
				body = base + message;
			}

			return _raise_message(level, type, body, true, loc, ts...);
		}



	}
	//Want to make a default set in each of these. Like report::compile

#define ISSUE_CLASS(mc_name, mc_issue_type, ...) struct mc_name { static constexpr auto Type = mc_issue_type; __VA_ARGS__ }

#define LEVEL_CLASS(mc_name, mc_level_type) template <typename... Ts>requires(!std::same_as<std::source_location, Ts> && ...)\
	using mc_name = _log_base<Type, mc_level_type, Ts...>

#define LEVEL_CLASSES()								\
	LEVEL_CLASS(info, IssueLevel::Info);			\
	LEVEL_CLASS(debug, IssueLevel::Debug);			\
	LEVEL_CLASS(warn, IssueLevel::Warning);			\
	LEVEL_CLASS(error, IssueLevel::Error);			\
	LEVEL_CLASS(critical, IssueLevel::Critical);	\
	LEVEL_CLASS(fatal, IssueLevel::Fatal);


	struct report
	{
	private:
		//for report issue, I would actually like to make use of an explicit specialization function probably.
		//TODO: I wish to make a level of error called Failure, which is basically supposed to just jump out of a current cycle, rather than end the whole process.
		// This is preferable to the current way of handling compiler errors that won't continue catching all the errors.

		template <IssueType Type, IssueLevel Level, typename... Ts>//requires(!std::same_as<std::source_location, Ts> && ...)
		struct _log_base
		{

			_log_base() = default;
			_log_base(_log_base&) = delete;
			_log_base(const _log_base&) = delete;



			static void _ThrowReport()
			{
				//This should through differently depending on what type it is, and if it's fatal it should be
				// a fault regardless.

				//This might take a string as well.

				if constexpr (Level == IssueLevel::Error || Level == IssueLevel::Critical)
				{
					if constexpr (Type == IssueType::Compile && Level != IssueLevel::Error)
					{
						throw CompileError("Not implemented");
					}
					else if constexpr (Type == IssueType::Link)
					{
						throw LinkError("Not implemented");
					}
					else if constexpr (Type == IssueType::Parse)
					{
						throw ParseError("Not implemented");
					}
					else if constexpr (Type == IssueType::Runtime)
					{
						throw RuntimeError("Not implemented");
					}
					//No exception found.

					//unhandled exception type.
					throw nullptr;
				}
				else if (Level == IssueLevel::Fatal)
				{

					//throw fatal error
					throw nullptr;
				}

			}

			//TODO: Report::_log functions have no reason to be a template, may locate in detail
			template <typename... Ts>//requires(!std::same_as<std::source_location, Ts> && ...)
			static void _log(IssueCode code, std::source_location loc, Ts&... ts)
			{
				detail::_raise_code(Level, Type, code, loc, ts...);

				//_ThrowReport();
			}

			template <typename... Ts>//requires(!std::same_as<std::source_location, Ts> && ...)
			static void _log_message(LString message, std::source_location loc, Ts&... ts)
			{
				detail::_raise_message(Level, Type, message, false, loc, ts...);

				//_ThrowReport();
			}





			_log_base(IssueCode code, Ts&&... ts, std::source_location loc = std::source_location::current())
			{
				_log(code, loc, ts...);
			}

			_log_base(IssueCode code, std::source_location loc, Ts&&... ts) requires (sizeof...(Ts) != 0)
			{
				_log(code, loc, ts...);
			}

			//TODO: For manual log bases, make it adhere to the formatting that is demanded of a regular format string.
			_log_base(LString message, Ts&&... ts, std::source_location loc = std::source_location::current())
			{
				_log_message(message, loc, ts...);
			}
			
			_log_base(LString message, std::source_location loc, Ts&&... ts) requires (sizeof...(Ts) != 0)
			{
				_log_message(message, loc, ts...);
			}


		};

		template <IssueType Type, IssueLevel Level, typename... Ts>
		_log_base(IssueCode code, Ts&&... ts) -> _log_base<Type, Level, Ts...>;

		template <IssueType Type, IssueLevel Level, typename... Ts>requires(sizeof...(Ts) != 0)
		_log_base(IssueCode code, std::source_location, Ts&&... ts) -> _log_base<Type, Level, Ts...>;


		template <IssueType Type, IssueLevel Level, typename... Ts>
		_log_base(LString message, Ts&&... ts) -> _log_base<Type, Level, Ts...>;

		template <IssueType Type, IssueLevel Level, typename... Ts>requires(sizeof...(Ts) != 0)
		_log_base(LString message, std::source_location, Ts&&... ts) -> _log_base<Type, Level, Ts...>;





	public:


		ISSUE_CLASS(parse, IssueType::Parse, LEVEL_CLASSES());
		ISSUE_CLASS(compile, IssueType::Compile, LEVEL_CLASSES());
		ISSUE_CLASS(link, IssueType::Link, LEVEL_CLASSES());
		ISSUE_CLASS(runtime, IssueType::Runtime, LEVEL_CLASSES());
		ISSUE_CLASS(fault, IssueType::Fault, LEVEL_CLASSES());
		ISSUE_CLASS(apply, IssueType::Apply, LEVEL_CLASSES());


	};
	//template<typename... Ts>
	//using __stupid_intellisense_fix = report::apply::debug<Ts...>;
	//RaiseIssue(IssueLevel level, IssueType type, IssueCode code, std::source_location)->RaiseIssue<>;

	
	


	inline void Test2()
	{
		report::apply::debug("", "test", "test");
		//Report<IssueType::Compile>::_log_base<IssueLevel::Debug>(1);

		//Report<IssueType::Compile>::RaiseIssue(IssueLevel::Debug, IssueType::Compile, 100, std::source_location::current(), 1, 1);

		//TestName<int>::RaiseAlias(IssueLevel::Debug, IssueType::Compile, 100, std::source_location::current(), 1, 1);
		logger::info("f");
		std::string* test = nullptr;


		//_RaiseIssue(IssueType::Compile, 100, IssueLevel::Debug, 1, 23, 4);
		//This shows that theoretically, this should work.

	}

}