#include "Lexicon/Report.h"

#include "Lexicon/Interfaces/ReportManager.h"

namespace LEX
{
	void report::LogBase(IssueCode code, std::string& message, IssueType type, IssueLevel level, std::source_location& loc)
	{
		//HeaderMessage(message, type, level, code);
		
		spdlog::source_loc a_loc{ loc.file_name(), static_cast<int>(loc.line()), loc.function_name() };
		
		//This is temporary, eventually I want some control of this in the plugin, via the same report method.
#ifdef LEX_SOURCE
		constexpr auto to = ReportType::Main;
#else
		constexpr auto to = ReportType::Plugin;
#endif


		ReportManager::instance->SendReport(type, level, to, code, message, a_loc);

	}

	std::string_view report::GetIssueMessage(IssueCode code)
	{
		return ReportManager::instance->GetIssueMessage(code);
	}

	IssueType report::GetIssueType()
	{ 
		return ReportManager::instance->GetIssueType(); 
	}



	report::report(IssueType set) : _prev{ ReportManager::instance->GetIssueType() }
	{
		ReportManager::instance->SetIssueType(set);
	}

	report::~report()
	{
		ReportManager::instance->SetIssueType(_prev);
	}
}