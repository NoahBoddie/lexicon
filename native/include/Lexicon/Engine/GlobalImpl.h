#pragma once

#include "Lexicon/Global.h"

namespace LEX
{
	struct Global : public GlobalAbstract
	{
		virtual const RuntimeVariable GetReference() const = 0;//Move this to implementation
		RuntimeVariable GetReference() { return make_const(this)->GetReference(); }
	};
}