#pragma once



namespace LEX
{
	struct ITypePolicy;
	struct ITemplateInsertBody;

	struct ITemplateInsertPart
	{

		virtual void InsertType(ITypePolicy* part) = 0;

		virtual ITemplateInsertBody* TryPromoteInserter() { return nullptr; }

	};
}