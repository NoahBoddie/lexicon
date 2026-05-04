#include "Lexicon/Engine/SyntaxRecord.h"

#include "Lexicon/Engine/Script.h"


namespace LEX
{

	//BasicRecord<Syntax, SyntaxRecord>* SyntaxRecord::GetSelf()
	//{
	//	return reinterpret_cast<Self*>(this);
	//}

	Element* SyntaxRecord::GetParent()
	{
		return RecordBase::GetParent<Element*>();
	}

	Syntax& SyntaxRecord::GetSyntax()
	{
		return GetEnumFromRecord();
	}

	bool SyntaxRecord::IsPath()
	{
		switch (GetSyntax().type)
		{
		case SyntaxType::Path:
		case SyntaxType::SpecifyShared:
		case SyntaxType::SpecifyCommons:
		case SyntaxType::SpecifyScript:
		case SyntaxType::SpecifyProject:
		case SyntaxType::SpecifyGlobal:
		case SyntaxType::SpecifyType:

			return true;

		default:
			return false;
		}
	}



	std::string SyntaxRecord::GetAffix()
	{
		//Add project
		Script* script = NULL_OP(NULL_Q(GetParent())->GetScript());

		std::string_view name = NULL_OP(NULL_Q(script)->GetName(), "<no_name>");
		std::string_view extension = script ? ".lsi" : "";

		auto& syntax = GetSyntax();

		return std::format(" <{}{}: (line: {} / col: {})>", name, extension, syntax.line, syntax.column);
	}

	std::function<LogEditor> SyntaxRecord::Mutator()
	{
		//return [this](LogParams& params, LogState state, LogResult&) -> void { if (state == LogState::Prep) params.suffix << GetAffix(); };
		return [this](LogParams& params, LogState state, LogResult&) -> void
			{
				if (this)
				{
					if (state == LogState::Prep)
					{
						Script* script = NULL_OP(NULL_Q(GetParent())->GetScript());
						Project* project = NULL_OP(NULL_Q(GetParent())->GetProject());

						auto& syntax = GetSyntax();

						if (script) {
							params.loc.filename = script->GetName().data();
						}
						else {
							params.loc.filename = "";
						}
						params.loc.line = syntax.line;

						//params.suffix << GetAffix();
					}
				}
			};
	}

}