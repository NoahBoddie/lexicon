#pragma once

#include "Lexicon/IGlobal.h"
#include "Lexicon/IVariable.h"
#include "Lexicon/RuntimeVariable.h"

namespace LEX
{
	struct RuntimeVariable;

	struct AbstractGlobal : public virtual IGlobal, ACCESS_OR(public, protected) IVariable
	{
		bool IsResolved() const override final { return true; }

		AbstractGlobal* GetGlobal(IGenericArgument* args) override { return this; };
		AbstractGlobal* GetGlobal(NewGenericV2::ITemplateBody* args) override { return this; };


		virtual RuntimeVariable GetValue()
		{
			//This is what outsiders use, as to not corrupt what goes into the value.
			return *this;
		}
		virtual bool SetValue(Variable& value)
		{
			Assign(value);

			return true;
		}

		
	};
}