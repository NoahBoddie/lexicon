#pragma once

namespace LEX
{
	struct ITemplateInserter
	{

		virtual void InsertType(ITypeInfo* part) = 0;

		//void AcceptTypes(std::span<ITypeInfo*> types);
	};

	/*
	void InsertType(ITypeInfo* part) override
	{
		auto back = part->GetTypePolicy((ITemplateBody*)nullptr);

		assert(back);

		return InsertType(back);
	}
	//*/
}