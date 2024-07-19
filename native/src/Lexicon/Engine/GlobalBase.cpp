#include "Lexicon/Engine/GlobalBase.h"
#include "Lexicon/Engine/Expression.h"
namespace LEX
{

	void GlobalBase::LoadFromRecord(Record& target)
	{
		if (target.SYNTAX().type != SyntaxType::Function) {
			report::compile::critical("Not a function, cannot load.");
		}

		_name = target.GetTag();
	}

	void GlobalBase::OnAttach()
	{

	}

	LinkResult GlobalBase::OnLink(LinkFlag flags)
	{
		return LinkFlag::Declaration | LinkFlag::Definition | LinkFlag::External;
	}

	LinkFlag GlobalBase::GetLinkFlags()
	{

	}

}