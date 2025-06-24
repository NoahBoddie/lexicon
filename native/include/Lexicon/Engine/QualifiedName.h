#pragma once

#include "Lexicon/GenericArray.h"


namespace LEX
{
	struct Element;
	struct Environment;

	struct QualifiedName : public GenericArray
	{
		QualifiedName(Environment* e) : env{ e } {}
		QualifiedName(Environment* e, const GenericArray& array) : env{ e }, GenericArray{ array } {}
		QualifiedName(Environment* e, GenericArray&& array) : env{ e }, GenericArray{ std::move(array) } {}

		QualifiedName(Element* e) : elem{ e } {}
		QualifiedName(Element* e, const GenericArray& array) : elem{ e }, GenericArray{ array } {}
		QualifiedName(Element* e, GenericArray&& array) : elem{ e }, GenericArray{ std::move(array) } {}

		//operator Environment* () { return env; }
		Environment* operator-> () { return env; }



		Element* elem = nullptr;
		Environment* env = nullptr;

	};


	struct QualifiedElement : public QualifiedName
	{
		using QualifiedName::QualifiedName;
		Element* operator-> () { return elem; }
	};
}