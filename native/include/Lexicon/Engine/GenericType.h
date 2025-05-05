#pragma once

#include "Lexicon/Engine/TypeBase.h"
#include "Lexicon/Engine/GenericBase.h"

namespace LEX
{
	struct SpecialBase;

	struct GenericType : public GenericTypeBase, public GenericBase
	{

		GenericType() : GenericTypeBase{} {}

		GenericType(uint32_t i) : GenericTypeBase{ i } {}

		GenericType(std::string_view name, TypeOffset offset) : GenericTypeBase{ name, offset } {}


		GenericBase* AsGenericElement() override { return this; }

		ISpecializable* GetSpecializable() override { return this; }

		bool IsResolved() const override { return false; }

		ITypeInfo* CheckTypePolicy(ITemplatePart* args) override;

		TypeInfo* GetTypePolicy(ITemplateBody* args) override;

		std::vector<TemplateType*> GetTemplateInputs() override
		{
			std::vector<TemplateType*> result{ _templates.size() };

			std::transform(_templates.begin(), _templates.end(), result.begin(), [](TemplateType& it) { return std::addressof(it); });

			return result;
		}

		SpecialBase* ObtainPart(ITemplatePart* args) override
		{
			if (args->GetClient() == this) {
				//TODO: I would like for GenericType::ObtainPart to
				//If this is the client, I'd like to check if each type is something this houses, and if so
				// I'd like to just use the generic base as the object in question.
			}

			return __super::ObtainPart(args);
		}

		ITemplatePart* GetSpecialPart() override
		{
			return this;
		}

		std::unique_ptr<SpecialBase> CreateSpecial(ITemplatePart* args) override;
	};


}