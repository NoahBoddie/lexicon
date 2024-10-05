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

	void SyntaxBody::SetParent(Element* parent)
	{
		return GetSelf()->RecordBase::SetParent(parent);
	}

	Syntax& SyntaxBody::GetSyntax()
	{
		return GetSelf()->GetEnumFromRecord();
	}

	bool SyntaxBody::IsPath()
	{
		return GetSyntax().type == SyntaxType::Path;
	}


	std::string SyntaxBody::GetAffix()
	{
		Script* script = GetParent()->FetchScript();

		std::string_view name = script ? script->GetName() : "<no_name>";
		std::string_view extension = script ? ".lsi" : "";

		auto& syntax = GetSyntax();

		return std::format(" <{}{}: (line: {} / col: {})>", name, extension, syntax.line, syntax.column);
	}


}