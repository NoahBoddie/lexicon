#pragma once


#include "Lexicon/Engine/TemplateType.h"
#include "Lexicon/Engine/ISpecializable.h"

#include "Lexicon/Engine/SpecialBase.h"


//*src
#include "Lexicon/ITemplateBody.h"
#include "Lexicon/Engine/Element.h"
#include "Lexicon/Engine/Environment.h"
namespace LEX
{

	struct SpecialBase;
	struct SpecialBase;



	struct GenericBase : public ISpecializable
	{
		//This is the class that handles Generic stuff, old name ISpecializable



		GenericBase* GetGeneric() const override
		{
			return const_cast<GenericBase*>(this);
		}

		SpecialBase* FindPart(GenericBase* tar, ITemplatePart* args);

		SpecialBase* FindBody(ITemplateBody* args);



		virtual std::unique_ptr<SpecialBase> CreatePart(ITemplatePart* args) = 0;
		virtual std::unique_ptr<SpecialBase> CreateBody(ITemplateBody* args) = 0;


	private:

		//Should be pure. The use of this needs to manually confirm that it is resolved.
		virtual std::unique_ptr<SpecialBase> CreateSpecial(ITemplatePart* args) { return {}; }
	
	public:

		//Note that validation needs to happen around here for multiple parts.


		//Might need to be override for this
		virtual SpecialBase* ObtainPart(GenericBase* client, ITemplatePart* args)
		{
			auto result = client->FindPart(this, args);

			if (!result) {
				result = client->incomplete.emplace_back(CreatePart(args)).get();
			}

			return result;
		}

		SpecialBase* ObtainBody(ITemplateBody* args) override
		{
			if (TemplateMatches(args) == false)
			{
				report::fault::error("cant handle args");
			}

			//Move this into specialize, it only makes sense.
			auto result = FindBody(args);

			if (!result) {
				result = Specialize(args);
			}

			return result;
		}

		SpecialBase* Specialize(ITemplateBody* args)
		{
			SpecializeParts(args);

			return complete.emplace_back(CreateBody(args)).get();

		}

		void SpecializeParts(ITemplateBody* args);

		SpecialBase* ObtainSpecial(GenericBase* client, ITemplatePart* args);

		bool TemplateMatches(ITemplatePart* args) override
		{
			//This expects a completed template btw.

			auto size = _templates.size();



			if (args->GetSize() != size)
				return false;

			for (int i = 0; i < size; i++)
			{
				auto& param = _templates[i];

				//This would be the thing it should be trying to turn into
				ITypePolicy* _param = &param;

				ITypePolicy* arg = args->GetPartArgument(i);

				Element* element = dynamic_cast<Element*>(this);

				ITypePolicy* scope = element->FetchEnvironment()->As<ITypePolicy>();

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

		//SpecialBase* ObtainBody()


		//I'm unsure if this would even bee needed honestly.
		//void ResolveIncomplete(IGenericArgument*) {}

		const std::vector<TemplateType>& templates() const
		{
			return _templates;
		}


		//For when something inherits from one and falls into the other?
		//std::vector<uint32_t> inheritGroups{};

		std::vector<TemplateType> _templates;
		
		//I'm actually unsure how needed incomplete is. We have to specialize stuff as it's getting used,
		// so it would likely be specialized in the moment
		std::vector<std::unique_ptr<SpecialBase>> incomplete;
		std::vector<std::unique_ptr<SpecialBase>> complete;
	};


}