#pragma once

#include "Lexicon/GenericArray.h"

//TODO: Rename QualifiedName

namespace LEX
{
	struct Element;
	struct Environment;
	struct Directory;

	struct SpecialDirectory : public GenericArray
	{
		SpecialDirectory(Directory* e) : dir{ e } {}
		SpecialDirectory(Directory* e, const GenericArray& array) : dir{ e }, GenericArray{ array } {}
		SpecialDirectory(Directory* e, GenericArray&& array) : dir{ e }, GenericArray{ std::move(array) } {}

		//operator Environment* () { return env; }
		Directory* operator-> () { return dir; }

		Directory* dir = nullptr;

	};

}