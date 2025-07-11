#pragma once


#include "Lexicon/Engine/Expression.h"


#include "Lexicon/Report.h"
#include "Lexicon/Interfaces/ReportManager.h"

namespace LEX
{
	class Script;
	struct Element;


	struct SyntaxBody
	{
		//TODO: I would like to make make access functions for certain common ground stuff like return types and such.

		using Self = BasicRecord<Syntax, SyntaxBody>;

		//I would like to make an over version of syntax record that basically copies all the functions and makes it into a new version, that way find
		// and the rest of that stuff doesn't have so many issues.

		//Actually, pause, I think I'll make a "basic_record" that takes what type of record it is. Largely, the concept would be there'd be no difference
		// but it would help determine what returns there are. The template record is what would do all the accessing and such maybe?



		//What I basically want of this it accumulate messages, and their source locations and such. So it will handle that sort of this.

		//inline static std::unordered_map<size_t, Script*> selfList;

		inline static size_t hash = 0;


		void Tester()
		{
			std::hash<std::thread::id> hasher{};

			size_t hash = hasher(std::this_thread::get_id());

		}
	private:
		Self* GetSelf();

	public:

		Element* GetParent();
		
		Syntax& GetSyntax();

		bool IsPath();


		std::string GetAffix();
		
		std::function<LogEditor> Mutator();
		

		template <IssueType Issue = IssueType::Total, is_not<std::source_location>... Ts> void Log(const std::string& message, std::source_location& src, IssueLevel level, Ts&&... args)
		{
			auto handle = ReportManager::instance->AddEditor(Mutator());

			return report::log(message, src, ReportManager::instance->GetIssueType(), level, args...);
			//return report::log(message.prox, GetAffix(), message.src, ReportManager::instance->GetIssueType(), IssueLevel::Debug, args...);
		}

		template <IssueType Issue = IssueType::Total, is_not<std::source_location>... Ts> void Log(IssueCode code, std::source_location& src, IssueLevel level, Ts&&... args)
		{
			auto handle = ReportManager::instance->AddEditor(Mutator());

			return report::log(code, src, ReportManager::instance->GetIssueType(), level, args...);
			//return report::log(message.prox, GetAffix(), message.src, ReportManager::instance->GetIssueType(), IssueLevel::Debug, args...);
		}

		//scoped_logger log(Mutator(), LogState::Prep); 

#define DECLARE_SYNTAX_LOGGER(mc_name, mc_level,...)\
		template <IssueType Issue = IssueType::Total, is_not<std::source_location>... Ts>\
		void mc_name(SourceAndProxy<std::string> message, Ts&&... args)\
		{\
			return Log<Issue>(message.prox, message.src, IssueLevel::mc_level, args...);\
		}\
		template <IssueType Issue = IssueType::Total, is_not<std::source_location>... Ts>\
		void mc_name(std::string& message, std::source_location loc, Ts&&... args)\
		{\
			return Log<Issue>(message, loc, IssueLevel::mc_level, args...);\
		}\
		template <IssueType Issue = IssueType::Total, is_not<std::source_location>... Ts>\
		void mc_name(IssueCode code, Ts&&... args)\
		{\
			return Log<Issue>(code, IssueLevel::mc_level, args...);\
		}\
		template <IssueType Issue = IssueType::Total, is_not<std::source_location>... Ts>\
		void mc_name(IssueCode code, std::source_location loc, Ts&&... args)\
		{\
			return Log<Issue>(code, loc, IssueLevel::mc_level, args...);\
		}


		/*

#define DECLARE_SYNTAX_LOGGER(mc_name, mc_level,...)\
		template <is_not<std::source_location>... Ts>\
		void mc_name(SourceAndProxy<std::string> message, Ts&&... args)\
		{\
			return report::log(message.prox, message.src, ReportManager::instance->GetIssueType(), IssueLevel::mc_level, args...);\
		}\
		template <is_not<std::source_location>... Ts>\
		void mc_name(std::string& message, std::source_location loc, Ts&&... args)\
		{\
			return report::log(message, loc, ReportManager::instance->GetIssueType(), IssueLevel::mc_level, args...);\
		}\
		template <is_not<std::source_location>... Ts>\
		static void mc_name(IssueCode code, Ts&&... args)\
		{\
			return report::log(code, ReportManager::instance->GetIssueType(), IssueLevel::mc_level, args...);\
		}\
		template <is_not<std::source_location>... Ts>\
		static void mc_name(IssueCode code, std::source_location loc, Ts&&... args)\
		{\
			return report::log(code, loc, ReportManager::instance->GetIssueType(), IssueLevel::mc_level, args...);\
		}


		//*/


		DECLARE_SYNTAX_LOGGER(critical, Critical);

		DECLARE_SYNTAX_LOGGER(info, Info);
		DECLARE_SYNTAX_LOGGER(debug, Debug);
		DECLARE_SYNTAX_LOGGER(error, Error);

		template <is_not<std::source_location>... Ts> void Note(SourceAndProxy<std::string> message, Ts&&... args) 
		{
			auto handle = ReportManager::instance->AddEditor(Mutator());
		
			return report::log(message.prox, message.src, ReportManager::instance->GetIssueType(), IssueLevel::Debug, args...);
			//return report::log(message.prox, GetAffix(), message.src, ReportManager::instance->GetIssueType(), IssueLevel::Debug, args...);
		} 
		

		/*
		template <is_not<std::source_location>... Ts>
		static void Warn(SourceAndProxy<std::string> message, Syntax syntax, Ts&&... args)
		{
			return report::Log_(message.prox, message.src, ReportManager::instance->GetIssueType(), IssueLevel::Debug, std::forward(args)...);
		}

		template <is_not<std::source_location>... Ts>
		static void Warn(std::string& message, std::source_location loc, Syntax syntax, Ts&&... args)
		{

			return report::Log_(message, loc, ReportManager::instance->GetIssueType(), IssueLevel::Debug, std::forward(args)...);
		}

		template <is_not<std::source_location>... Ts>
		static void Croak(IssueCode code, Syntax syntax, Ts&&... args)
		{

			return report::Log_(message, loc, ReportManager::instance->GetIssueType(), IssueLevel::Debug, std::forward(args)...);
		}

		template <is_not<std::source_location>... Ts>
		static void Croak(IssueCode code, std::source_location loc, Syntax syntax, Ts&&... args)
		{

			return report::Log_(message, loc, ReportManager::instance->GetIssueType(), IssueLevel::Debug, std::forward(args)...);
		}




		template <is_not<std::source_location>... Ts>
		static void Error(SourceAndProxy<std::string> message, Syntax syntax, Ts&&... args)
		{
			return report::Log_(message.prox, message.src, ReportManager::instance->GetIssueType(), IssueLevel::Debug, std::forward(args)...);
		}

		template <is_not<std::source_location>... Ts>
		static void Error(std::string& message, std::source_location loc, Syntax syntax, Ts&&... args)
		{

			return report::Log_(message, loc, ReportManager::instance->GetIssueType(), IssueLevel::Debug, std::forward(args)...);
		}

		template <is_not<std::source_location>... Ts>
		static void Croak(IssueCode code, Syntax syntax, Ts&&... args)
		{

			return report::Log_(message, loc, ReportManager::instance->GetIssueType(), IssueLevel::Debug, std::forward(args)...);
		}

		template <is_not<std::source_location>... Ts>
		static void Croak(IssueCode code, std::source_location loc, Syntax syntax, Ts&&... args)
		{

			return report::Log_(message, loc, ReportManager::instance->GetIssueType(), IssueLevel::Debug, std::forward(args)...);
		}





		template <is_not<std::source_location>... Ts>
		static void Croak(SourceAndProxy<std::string> message, Syntax syntax, Ts&&... args)
		{
			return report::Log_(message.prox, message.src, ReportManager::instance->GetIssueType(), IssueLevel::Debug, std::forward(args)...);
		}

		template <is_not<std::source_location>... Ts>
		static void Croak(std::string& message, std::source_location loc, Syntax syntax, Ts&&... args)
		{

			return report::Log_(message, loc, ReportManager::instance->GetIssueType(), IssueLevel::Debug, std::forward(args)...);
		}

		template <is_not<std::source_location>... Ts>
		static void Croak(IssueCode code, Syntax syntax, Ts&&... args)
		{

			return report::Log_(message, loc, ReportManager::instance->GetIssueType(), IssueLevel::Debug, std::forward(args)...);
		}

		template <is_not<std::source_location>... Ts>
		static void Croak(IssueCode code, std::source_location loc, Syntax syntax, Ts&&... args)
		{

			return report::Log_(message, loc, ReportManager::instance->GetIssueType(), IssueLevel::Debug, std::forward(args)...);
		}

		//*/


	};

	using SyntaxRecord = BasicRecord<Syntax, SyntaxBody>;
}