#pragma once

#include "Lexicon/Field.h"
#include "Lexicon/ISpecial.h"


namespace LEX
{
	struct GenericBase;
	struct ITemplatePart;
	struct ITemplateBody;
	
	struct AbstractGlobal;


	struct IGlobal : public ISpecial, public Field
	{

	virtual IGlobal* CheckGlobal(GenericBase* base, ITemplatePart* args) { return this; };

	virtual AbstractGlobal* GetGlobal(ITemplateBody* args) = 0;

	virtual std::string_view GetName() const = 0;

	//Attempts to revert value. If the global is const or a special part, it will fail to revert.
	// If reverted just with default it will create the default value, if not, it will attempt to use a routine to set
	// information. Will not throw.
	virtual bool Revert(bool just_default) = 0;

	AbstractGlobal* FetchGlobal(ITemplateBody* args)
	{
		return this ? GetGlobal(args) : nullptr;
	}

	};
}