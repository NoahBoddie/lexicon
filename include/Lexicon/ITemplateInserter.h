#pragma once

namespace LEX
{
	struct ITemplateInserter
	{

		virtual void InsertType(BasicType* part) = 0;

		//void AcceptTypes(std::span<IType*> types);
	};

	/*
	void InsertType(IType* part) override
	{
		auto back = part->GetTypePolicy((ITemplateBody*)nullptr);

		assert(back);

		return InsertType(back);
	}
	//*/
}