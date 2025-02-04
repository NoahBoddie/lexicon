#include "Lexicon/Report.h"

#include "Lexicon/Interfaces/ReportManager.h"

namespace LEX
{
	void report::LogBase(IssueCode code, std::string_view main, std::string_view trans, IssueType type, IssueLevel level, const std::source_location& loc)
	{
		//HeaderMessage(message, type, level, code);
		
		spdlog::source_loc a_loc{ loc.file_name(), static_cast<int>(loc.line()), loc.function_name() };
		
		//This is temporary, eventually I want some control of this in the plugin, via the same report method.
		
		ReportType to = ReportType::Main;

#ifndef LEX_SOURCE
		to |= ReportType::Return;
#endif


		ReportManager::instance->SendReport(type, level, to, code, main, trans, a_loc);

	}

	std::string_view report::GetIssueMessage(IssueCode code, bool translation)
	{
		return ReportManager::instance->GetIssueMessage(code, translation);
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