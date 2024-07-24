#pragma once

#include "Lexicon/Engine/ISpecializable.h"

namespace LEX
{

	struct ITypePolicy;
	struct SpecialPart;
	struct SpecialBody;
	struct GenericBase;
	struct ITemplatePart;

	struct SpecialBase : public ISpecializable
	{
		SpecialBase(GenericBase* b) : _base{ b } {}

		//I don't wanna do this but there's no dynamic cast without polymorphism so +8


		GenericBase* GetGeneric() const override
		{
			return _base;
		}

		virtual SpecialPart* AsPart() { return nullptr; }
		virtual SpecialBody* AsBody() { return nullptr; }

		virtual ITemplatePart* AsTemplate() = 0;

		virtual bool TemplateMatches(ITemplatePart* args) = 0;


		virtual size_t GetSize() const = 0;

		//Get rid of this, AsTemplate handles this.
		virtual ITypePolicy* GetArgument(size_t i) const = 0;


	private:
		mutable GenericBase* _base = nullptr;
	};


}