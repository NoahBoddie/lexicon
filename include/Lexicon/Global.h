#pragma once


#include "Lexicon/IGlobal.hpp"


#include "Lexicon/RuntimeVariable.h"

namespace LEX
{
	struct RuntimeVariable;

	struct Global : public IGlobal
	{
		//bool IsResolved() const override final { return true; }

		//Global* GetGlobal(ITemplateBody* args) override { return this; };

		//TODO: Restore these
		
		//Gets the value of the global. Attempts to send it by reference, but qualifiers
		virtual RuntimeVariable GetValue() = 0;
		virtual RuntimeVariable GetReference() = 0;//Move this to implementation
		virtual const RuntimeVariable GetValue() const { return const_cast<Global*>(this)->GetValue(); }
		virtual const RuntimeVariable GetReference() const { return const_cast<Global*>(this)->GetReference(); }



		//Allows for the setting of the actual global space. If the setting defies the qualifiers it may fail.
		virtual bool Set(const RuntimeVariable& value) = 0;

		
	};
} 