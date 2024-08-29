#pragma once


#include "Lexicon/Engine/TemplateType.h"
#include "Lexicon/Engine/ISpecializable.h"

#include "Lexicon/Engine/SpecialPart.h"
#include "Lexicon/Engine/SpecialBody.h"


//*src
#include "Lexicon/ITemplateBody.h"
#include "Lexicon/Engine/Element.h"
#include "Lexicon/Engine/Environment.h"
namespace LEX
{

	struct SpecialPart;
	struct SpecialBody;



	struct GenericBase : public ISpecializable
	{
		//This is the class that handles Generic stuff, old name ISpecializable



		GenericBase* GetGeneric() const override
		{
			return const_cast<GenericBase*>(this);
		}

		SpecialPart* FindPart(GenericBase* tar, ITemplatePart* args);

		SpecialBody* FindBody(ITemplateBody* args);



		virtual std::unique_ptr<SpecialPart> CreatePart(ITemplatePart* args) = 0;
		virtual std::unique_ptr<SpecialBody> CreateBody(ITemplateBody* args) = 0;

		//Note that validation needs to happen around here for multiple parts.


		//Might need to be override for this
		virtual SpecialPart* ObtainPart(GenericBase* client, ITemplatePart* args)
		{
			auto result = client->FindPart(this, args);

			if (!result) {
				result = client->incomplete.emplace_back(CreatePart(args)).get();
			}

			return result;
		}

		SpecialBody* ObtainBody(ITemplateBody* args) override
		{
			if (TemplateMatches(args) == false)
			{
				report::fault::error("cant handle args");
			}

			auto result = FindBody(args);

			if (!result) {
				result = Specialize(args);
			}

			return result;
		}

		SpecialBody* Specialize(ITemplateBody* args)
		{
			SpecializeParts(args);

			return complete.emplace_back(CreateBody(args)).get();

		}

		void SpecializeParts(ITemplateBody* args);

		SpecialBase* ObtainSpecial(GenericBase* client, ITemplatePart* args);

		bool TemplateMatches(ITemplatePart* args) override
		{
			//This expects a completed template btw.

			auto size = _template.size();



			if (args->GetSize() != size)
				return false;

			for (int i = 0; i < size; i++)
			{
				auto& param = _template[i];

				//This would be the thing it should be trying to turn into
				ITypePolicy* _param = &param;

				ITypePolicy* arg = args->GetPartArgument(i);

				Element* element = dynamic_cast<Element*>(this);

				ITypePolicy* scope = element->FetchEnvironment()->ToType();

				if constexpr (0)
				{//For now it accepts all, so no real reason to do this.
					auto conv = arg->IsConvertibleTo(_param, scope, nullptr);

					if (conv <= ConvertResult::Failure) {
						return false;
					}
				}
			}

			return true;
		}

		//So try specialize is what you'll get when you try to specialize I guess?
		//SpecialBase* TrySpecialize()

		//SpecialBody* ObtainBody()


		//I'm unsure if this would even bee needed honestly.
		//void ResolveIncomplete(IGenericArgument*) {}




		std::vector<TemplateType> _template;

		std::vector<std::unique_ptr<SpecialPart>> incomplete;
		std::vector<std::unique_ptr<SpecialBody>> complete;
	};


}