#pragma once

#include "Lexicon/Field.h"
#include "Lexicon/ISpecial.h"
#include "Lexicon/IGenericArgument.h"

namespace LEX
{
	namespace NewGenericV2
	{
		struct GenericBase;
		struct ITemplatePart;
		struct ITemplateBody;
	}
	struct AbstractGlobal;


	struct IGlobal : public ISpecial
	{

	virtual IGlobal* CheckGlobal(NewGenericV2::GenericBase* base, NewGenericV2::ITemplatePart* args) { return this; };

	virtual AbstractGlobal* GetGlobal(IGenericArgument* args) { return nullptr; };
	virtual AbstractGlobal* GetGlobal(NewGenericV2::ITemplateBody* args) { return nullptr; };

	virtual std::string_view GetName() const = 0;

	AbstractGlobal* FetchGlobal(IGenericArgument* args)
	{
		return this ? GetGlobal(args) : nullptr;
	}

	};
}