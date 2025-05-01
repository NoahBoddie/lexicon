#pragma once

namespace LEX
{
	struct ITemplatePart;
	struct ITemplateBody;

	struct SpecialBase;

	struct GenericBase;

	//TODO: This seems close to being completely unnecessary in my opinion especially because i's in engine. 
	// Might as well make an upper version of ISpecial that handles this transformation to GenericBase, and move
	// the handling of all this to GenericBase. I'll hold off for now though.
	struct ISpecializable
	{
		//I don't wanna do this but there's no dynamic cast without polymorphism so +8
		virtual ~ISpecializable() = default;

		//TODO: make non-const version of GetGeneric
		virtual GenericBase* GetGeneric() const = 0;

		GenericBase* FetchBase() const
		{
			return this ? GetGeneric() : nullptr;
		}

		//virtual SpecialBase* GetPart(ITemplatePart* args) = 0;

		//This should just be obtain special. SpecialBase is both part and body
		//virtual SpecialBase* ObtainBody(ITemplateBody* args) = 0;
		virtual SpecialBase* ObtainSpecial(ITemplatePart* args) = 0;

		virtual bool TemplateMatches(ITemplatePart* args) = 0;


	};

}