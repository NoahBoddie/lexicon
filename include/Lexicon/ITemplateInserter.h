#pragma once

namespace LEX
{
	struct ITypePolicy;

	struct ITemplateInserter
	{

		virtual void InsertType(ITypePolicy* part) = 0;

		//void AcceptTypes(std::span<ITypePolicy*> types);
	};

	/*
	void InsertType(ITypePolicy* part) override
	{
		auto back = part->GetTypePolicy((ITemplateBody*)nullptr);

		assert(back);

		return InsertType(back);
	}
	//*/
}