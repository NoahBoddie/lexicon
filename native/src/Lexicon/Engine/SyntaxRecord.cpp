#include "Lexicon/Engine/SyntaxRecord.h"

#include "Lexicon/Engine/Script.h"


namespace LEX
{

	BasicRecord<Syntax, SyntaxBody>* SyntaxBody::GetSelf()
	{
		return reinterpret_cast<Self*>(this);
	}

	Element* SyntaxBody::GetParent()
	{
		return GetSelf()->RecordBase::GetParent<Element*>();
	}

	Syntax& SyntaxBody::GetSyntax()
	{
		return GetSelf()->GetEnumFromRecord();
	}

	bool SyntaxBody::IsPath()
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


	std::string SyntaxBody::GetAffix()
	{
		//Add project
		Script* script = GetParent()->FetchScript();

		std::string_view name = script ? script->GetName() : "<no_name>";
		std::string_view extension = script ? ".lsi" : "";

		auto& syntax = GetSyntax();

		return std::format(" <{}{}: (line: {} / col: {})>", name, extension, syntax.line, syntax.column);
	}

	std::function<LogEditor> SyntaxBody::Mutator()
	{
		//return [this](LogParams& params, LogState state, LogResult&) -> void { if (state == LogState::Prep) params.suffix << GetAffix(); };
		return [this](LogParams& params, LogState state, LogResult&) -> void
			{
				if (this)
				{
					if (state == LogState::Prep)
					{
						Script* script = GetParent()->FetchScript();
						Project* project = GetParent()->FetchProject();

						auto& syntax = GetSyntax();

						if (script) {
							params.loc.filename = script->GetName().data();
							params.loc.line = syntax.line;
						}

						//params.suffix << GetAffix();
					}
				}
			};
	}

}