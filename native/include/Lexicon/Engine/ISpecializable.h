#pragma once

namespace LEX
{
	struct ITemplatePart;
	struct ITemplateBody;

	struct SpecialBase;
	struct SpecialBase;
	struct SpecialBase;

	struct GenericBase;

	struct ISpecializable
	{
		//I don't wanna do this but there's no dynamic cast without polymorphism so +8
		virtual ~ISpecializable() = default;

		virtual GenericBase* GetGeneric() const = 0;

		GenericBase* FetchBase() const
		{
			return this ? GetGeneric() : nullptr;
		}

		//virtual SpecialBase* GetPart(ITemplatePart* args) = 0;

		virtual SpecialBase* ObtainBody(ITemplateBody* args) = 0;


		virtual bool TemplateMatches(ITemplatePart* args) = 0;


	};

}