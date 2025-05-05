#pragma once

#include "Lexicon/GenericArray.h"


namespace LEX
{
	struct Environment;

	struct QualifiedName : public GenericArray
	{
		QualifiedName(Environment* e) : env{ e } {}

		//operator Environment* () { return env; }
		Environment* operator-> () { return env; }




		Environment* env = nullptr;

	};
}