#pragma once

#include "Lexicon/GenericArray.h"


namespace LEX
{
	struct Environment;

	struct QualifiedName : public GenericArray
	{
		QualifiedName(Environment* e) : env{ e } {}
		QualifiedName(Environment* e, const GenericArray& array) : env{ e }, GenericArray{ array } {}
		QualifiedName(Environment* e, GenericArray&& array) : env{ e }, GenericArray{ std::move(array) } {}

		//operator Environment* () { return env; }
		Environment* operator-> () { return env; }




		Environment* env = nullptr;

	};
}