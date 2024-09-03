#include "Lexicon/Report.h"

#include "Lexicon/Interfaces/ReportManager.h"


namespace LEX
{
	void report::LogBase(IssueCode code, std::string& message, IssueType type, IssueLevel level, std::source_location& loc)
	{
		//HeaderMessage(message, type, level, code);
		
		spdlog::source_loc a_loc{ loc.file_name(), static_cast<int>(loc.line()), loc.function_name() };

		ReportManager::instance->SendReport(type, level, code, message, a_loc);

	}

	std::string_view report::GetIssueMessage(IssueCode code)
	{
		return ReportManager::instance->GetIssueMessage(code);
	}

	IssueType report::GetIssueType() 
	{ 
		return ReportManager::instance->GetIssueType(); 
	}
}