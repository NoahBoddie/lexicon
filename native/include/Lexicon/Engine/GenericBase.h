#pragma once


#include "Lexicon/Engine/TemplateType.h"
#include "Lexicon/Engine/ISpecializable.h"

#include "Lexicon/Engine/SpecialBase.h"


//*src
#include "Lexicon/Engine/ITypeInfoImpl.h"
#include "Lexicon/ITemplateBody.h"
#include "Lexicon/Engine/Element.h"
#include "Lexicon/Engine/Environment.h"
namespace LEX
{

	struct SpecialBase;
	struct SpecialBase;



	struct GenericBase : public ISpecializable, public ITemplatePart
	{
		//This is the class that handles Generic stuff, old name ISpecializable



		GenericBase* GetGeneric() const override
		{
			return const_cast<GenericBase*>(this);
		}



		GenericBase* GetClient() const override { return GetGeneric(); }


		size_t GetSize() const override
		{
			return _templates.size();
		}

		ITypeInfo* GetPartArgument(size_t i) const override
		{
			return const_cast<TemplateType*>(std::addressof(_templates.at(i)));
		}


		SpecialBase* FindPart(GenericBase* tar, ITemplatePart* args);

		SpecialBase* FindBody(ITemplateBody* args);



	private:

		virtual std::unique_ptr<SpecialBase> CreateSpecial(ITemplatePart* args) = 0;
	
	public:

		//Note that validation needs to happen around here for multiple parts.

		//Hide the below

		//Might need to be override for this
		virtual SpecialBase* ObtainPart(ITemplatePart* args)
		{
			auto client = args->GetClient();


			auto result = client->FindPart(this, args);

			if (!result) {
				result = client->incomplete.emplace_back(CreateSpecial(args)).get();
			}

			return result;
		}

		SpecialBase* ObtainBody(ITemplateBody* args)
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
			auto spec = CreateSpecial(args);

			if (spec->IsResolved() == false) {
				report::critical("Created specialization isn't resolved");
			}

			return complete.emplace_back(std::move(spec)).get();

		}

		void SpecializeParts(ITemplateBody* args);

		SpecialBase* ObtainSpecial(ITemplatePart* args) override;

		virtual SpecialBase* TryObtainTemporary(ITemplatePart* args)
		{
			return ObtainSpecial(args);
		}

		virtual void ClearTemporary() {}

		bool TemplateMatches(ITemplatePart* args) override
		{
			//This expects a completed template btw.

			auto size = _templates.size();



			if (args->GetSize() != size)
				return false;

			for (int i = 0; i < size; i++)
			{
				TemplateType& param = _templates[i];
				
				//This would be the thing it should be trying to turn into
				ITypeInfo* _param = &param;

				ITypeInfo* arg = args->GetPartArgument(i);

				Element* element = dynamic_cast<Element*>(this);

				ITypeInfo* scope = element->FetchEnvironment()->As<ITypeInfo>();

				if constexpr (0)
				{//For now it accepts all, so no real reason to do this.
					auto conv = arg->IsConvertibleTo(_param, scope, nullptr);

					if (conv <= ConversionEnum::Failure) {
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

		TemplateType* GetTemplateAt(size_t i)
		{
			return &_templates[i];
		}


		TemplateType* GetTemplateByName(std::string_view name)
		{
			for (auto& temp_type : _templates)
			{
				if (temp_type.name == name) {
					return &temp_type;
				}
			}

			return nullptr;
		}



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