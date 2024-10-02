#pragma once

#include "Lexicon/Report.h"
#include "Lexicon/Interfaces/ReportManager.h"

#include "Lexicon/Engine/Script.h"

#include "Lexicon/Engine/Expression.h"

namespace LEX
{

	struct SyntaxBody
	{
		using Self = BasicRecord<Syntax, SyntaxBody>;

		//I would like to make an over version of syntax record that basically copies all the functions and makes it into a new version, that way find
		// and the rest of that stuff doesn't have so many issues.

		//Actually, pause, I think I'll make a "basic_record" that takes what type of record it is. Largely, the concept would be there'd be no difference
		// but it would help determine what returns there are. The template record is what would do all the accessing and such maybe?



		//What I basically want of this it accumulate messages, and their source locations and such. So it will handle that sort of this.

		inline static std::unordered_map<size_t, Script*> selfList;

		inline static size_t hash = 0;


		void Tester()
		{
			std::hash<std::thread::id> hasher{};

			size_t hash = hasher(std::this_thread::get_id());

		}
	private:
		Self* GetSelf()
		{
			return reinterpret_cast<Self*>(this);
		}
	public:

		Element* GetParent()
		{
			return GetSelf()->RecordBase::GetParent<Element*>();
		}
		
		void SetParent(Element* parent)
		{
			return GetSelf()->RecordBase::SetParent(parent);
		}

		Syntax& GetSyntax()
		{
			return GetSelf()->GetEnumFromRecord();
		}

		bool IsPath()
		{
			return GetSyntax().type == SyntaxType::Path;
		}


		std::string GetAffix()
		{
			Script* script = GetParent()->FetchScript();

			std::string_view name = script ? script->GetName() : "<no_name>";
			std::string_view extension = script ? ".lsi" : "";

			auto& syntax = GetSyntax();

			return std::format(" <{}{}: (line: {} / col: {})>", name, extension, syntax.line, syntax.column);
		}
		
		auto Mutator()
		{
			return [this](LogData& data) -> bool { data.message += GetAffix(); return true;	};
		}


#define DECLARE_SYNTAX_LOGGER(mc_name, mc_level,...)\
		template <is_not<std::source_location>... Ts>\
		void mc_name(SourceAndProxy<std::string> message, Ts&&... args)\
		{\
			ScopedLogger log(Mutator(), true);\
			return report::Log_(message.prox, message.src, ReportManager::instance->GetIssueType(), IssueLevel::mc_level, args...);\
		}\
		template <is_not<std::source_location>... Ts>\
		void mc_name(std::string& message, std::source_location loc, Ts&&... args)\
		{\
			ScopedLogger log(Mutator(), true);\
			return report::Log_(message, loc, ReportManager::instance->GetIssueType(), IssueLevel::mc_level, args...);\
		}\
		template <is_not<std::source_location>... Ts>\
		static void mc_name(IssueCode code, Ts&&... args)\
		{\
			ScopedLogger log(Mutator(), true);\
			return report::Log_(message, loc, ReportManager::instance->GetIssueType(), IssueLevel::mc_level, args...);\
		}\
		template <is_not<std::source_location>... Ts>\
		static void mc_name(IssueCode code, std::source_location loc, Ts&&... args)\
		{\
			ScopedLogger log(Mutator(), true);\
			return report::Log_(message, loc, ReportManager::instance->GetIssueType(), IssueLevel::mc_level, args...);\
		}

		template <is_not<std::source_location>... Ts> void Note(SourceAndProxy<std::string> message, Ts&&... args) {
			ScopedLogger log(Mutator(), true); return report::Log_(message.prox, message.src, ReportManager::instance->GetIssueType(), IssueLevel::Debug, args...);
		} template <is_not<std::source_location>... Ts> void Note(std::string& message, std::source_location loc, Ts&&... args) {
			ScopedLogger log(Mutator(), true); return report::Log_(message, loc, ReportManager::instance->GetIssueType(), IssueLevel::Debug, args...);
		} template <is_not<std::source_location>... Ts> static void Note(IssueCode code, Ts&&... args) {
			ScopedLogger log(Mutator(), true); return report::Log_(code, ReportManager::instance->GetIssueType(), IssueLevel::Debug, args...);
		} template <is_not<std::source_location>... Ts> static void Note(IssueCode code, std::source_location loc, Ts&&... args) {
			ScopedLogger log(Mutator(), true); return report::Log_(code, loc, ReportManager::instance->GetIssueType(), IssueLevel::Debug, args...);
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