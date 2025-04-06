#pragma once

#include "Lexicon/Engine/AbstractType.h"
#include "Lexicon/Engine/HierarchyData.h"

//*src
#include "Lexicon/Interfaces/IdentityManager.h"
namespace LEX
{
	constexpr TypeID Trival = 0xDEADBEEF;
	constexpr TypeID Tuple = 0xDEADBAAD;


	struct TemplateTuple;

	struct TemplateType : public AbstractType, public HierarchyData
	{
		//GenericType is an IType that largely should not exist with any HierarchyData. It's from this fact
		// plus the fact HierarchyData is a lot that I think I should split the function between 2 parts.
		//This type will use a seperate HierarchyData having type in order to answer questions it's questions about it.

		//But until I have a proper design for that I think having hierarchy data will be fine.
		void Test()
		{
			IdentityManager::instance->GetIDFromName("TRIVAL");


			AbstractType* test = nullptr;

			HierarchyData* other = dynamic_cast<HierarchyData*>(test);
		}

		std::string name;
		size_t index = -1;
		TemplateTuple* _tupleData = nullptr;
		TemplateType(std::string n, size_t i) : name{ n }, index{ i } {};

		//TODO: TemplateType needs it's fucking specializable I'm fucking off
		ISpecializable* GetSpecializable() override { return nullptr; }


		AbstractType* CheckTypePolicy(ITemplatePart* args) override;

		Type* GetTypePolicy(ITemplateBody* args) override;

		std::vector<TemplateType*> GetTemplateInputs() override { return { this }; }

		virtual bool IsResolved() const { return false; }

		virtual TypeID GetTypeID() const { return _tupleData ? Tuple : Trival; }

		virtual DataType GetDataType() const { return DataType::Interface; }



		virtual std::string_view GetName() const { return name; }


		//static_assert(false, "I need a IsConvertibleFrom, which means I need a conversion function that isn't virtual.");
		ConvertResult GetConvertFrom(const BasicType* other, const BasicType* scope, Conversion* = nullptr, ConversionFlag flags = ConversionFlag::None) const override
		{
			auto result = __super::GetConvertFrom(other, scope, nullptr, flags);

			if (flags & ConversionFlag::Template && result <= ConvertResult::Failure)
			{
				for (auto& inherit : inheritance)
				{
					if (other->IsConvertibleTo(inherit.type, scope) != ConvertResult::TypeDefined) {
						return ConvertResult::Ineligible;
					}
				}

				result = ConvertResult::TempConvert;
			}

			return result;

		}
	
		//I want one able to handle possible tuple typing which doesn't currently exist.
		//bool CanSpecializeTo(TemplateType* other) const;
		


		HierarchyData* GetHierarchyData() const override
		{
			const HierarchyData* out = this;
			return const_cast<HierarchyData*>(out);
		}

		AbstractType* GetHierarchyType() override
		{
			return this;
		}

	};


}