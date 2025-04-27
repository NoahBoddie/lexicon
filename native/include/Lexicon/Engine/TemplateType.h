#pragma once

#include "Lexicon/Engine/ITypeInfo.h"
#include "Lexicon/Engine/HierarchyData.h"

//*src
#include "Lexicon/Interfaces/IdentityManager.h"
namespace LEX
{
	constexpr TypeID Trival = 0xDEADBEEF;
	constexpr TypeID Tuple = 0xDEADBAAD;


	struct TemplateTuple;

	struct TemplateType : public ITypeInfo, public HierarchyData
	{
		//GenericType is an ITypeInfo that largely should not exist with any HierarchyData. It's from this fact
		// plus the fact HierarchyData is a lot that I think I should split the function between 2 parts.
		//This type will use a seperate HierarchyData having type in order to answer questions it's questions about it.

		//But until I have a proper design for that I think having hierarchy data will be fine.
		void Test()
		{
			IdentityManager::instance->GetIDFromName("TRIVAL");


			ITypeInfo* test = nullptr;

			HierarchyData* other = dynamic_cast<HierarchyData*>(test);
		}

		std::string name;
		size_t index = -1;
		TemplateTuple* _tupleData = nullptr;
		TemplateType(std::string n, size_t i) : name{ n }, index{ i } {};

		//TODO: TemplateType needs it's fucking specializable I'm fucking off
		ISpecializable* GetSpecializable() override { return nullptr; }


		ITypeInfo* CheckTypePolicy(ITemplatePart* args) override;

		TypeInfo* GetTypePolicy(ITemplateBody* args) override;


		TemplateType* AsTemplate() override { return this; }
		std::vector<TemplateType*> GetTemplateInputs() override { return { this }; }

		virtual bool IsResolved() const { return false; }

		virtual TypeID GetTypeID() const { return _tupleData ? Tuple : Trival; }

		virtual DataType GetDataType() const { return DataType::Interface; }



		virtual std::string_view GetName() const { return name; }


		//static_assert(false, "I need a IsConvertibleFrom, which means I need a conversion function that isn't virtual.");
		ConvertResult GetConvertFrom(const ITypeInfo* other, const ITypeInfo* scope, Conversion* = nullptr, ConversionFlag flags = ConversionFlag::None) const override
		{
			auto result = __super::GetConvertFrom(other, scope, nullptr, flags);

			if (flags & ConversionFlag::Template && result.data <= ConversionEnum::Failure)
			{
				//If this inherits nothing it literally cannot be used for anything
				for (auto& inherit : inheritance)
				{
					if (other->IsConvertibleTo(inherit.type, scope) != ConversionEnum::TypeDefined) {
						return ConversionResult::Ineligible;
					}
				}

				result = ConversionEnum::TempConvert;
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

		ITypeInfo* GetHierarchyType() override
		{
			return this;
		}

	};


}