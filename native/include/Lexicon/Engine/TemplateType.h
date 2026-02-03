#pragma once

#include "Lexicon/Engine/ITypeInfoImpl.h"
#include "Lexicon/Engine/HierarchyData.h"

//*src
#include "Lexicon/Interfaces/IdentityManager.h"
#include "Lexicon/Qualifier.h"
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
		TemplateType(const std::string_view& n, size_t i) : name{ n }, index{ i } {};

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

		ObjectPolicy* GetObjectPolicy() const
		{
			return nullptr;
		}

		ConvertResult GetConvertTo(const ITypeInfo* other, const ITypeInfo* scope, Conversion* out, ConversionFlag flags) const override
		{
			if (this == other) {
				return ConversionEnum::Exact;
			}


			//First we check if the entry exists. Based on what its access is, what we do changes.
			//Public: Conversion is allowed.
			//Protected: scope type must other must derive from
			//Private: scope type must have other's distance be 1.

			//Internal: The trail of internals within scope must lead to an internal that has an owner index of 0. Otherwise, it is too seperated to be converted.
			//Will need to make a collection to measure this.
			// Actually, simplier way (though the above is still required) this and scope need to be the same. Actually this isn't right either, internal is about
			// this, not who's scope.


			//Whether it can convert should basically be solved here, this should never return an internal it does not own.
			const InheritData* convert_data = GetInheritData(other);


			//Not gonna worry about function conversions for a while.
			if (!convert_data) {
				//TODO: If this is internal, I might perform some saving throw with the scope type.
				return ConversionResult::Ineligible;
			}

			//To do this saving throw, we'd need to get if scope has that inheritdata from other first (check for internal first)
			// then, if it does, ask if scope is viewable this is convertible to scope.



			Access access = convert_data->access & ~Access::Internal;
			//Should this ever actually be internal?
			bool is_internal = convert_data->IsInternal();//convert_data->access & Access::Internal;

			if (is_internal) {
				logger::info("Owner should be 0 => {}", convert_data->ownerIndex);
			}


			switch (access)
			{
				//No access is given, thus conversion is not possible from here.
				//*This has to be tested later.
				//case Access::None:
				//	return false;//No access between types

			case Access::Public:
				return ConversionEnum::TypeDefined;//Type converted
			}

			if (!scope) {
				return ConversionResult::Inaccessible;//No access from here
			}

			if (scope == other) {
				return ConversionEnum::TypeDefined;
			}

			//By this point, please note that internal should not even be a thought here.
			//Being able to get someone's inheritdata that belongs to a specific class might be valuable.
			//*Might need to recant this.
			const InheritData* access_data = scope->GetHierarchyData()->GetInheritData(other);



			if (!access_data || access_data->GetAccess() == Access::None) {
				return ConversionResult::Inaccessible;//No access from here
			}

			//if (access_data->GetAccess())



			return ConversionEnum::TypeDefined;//Should have access
		}


		//static_assert(false, "I need a IsConvertibleFrom, which means I need a conversion function that isn't virtual.");
		ConvertResult GetConvertFrom(const ITypeInfo* other, const ITypeInfo* scope, Conversion* = nullptr, ConversionFlag flags = ConversionFlag::None) const override
		{
			auto result = __super::GetConvertFrom(other, scope, nullptr, flags);

			if (result.data <= ConversionEnum::Failure)
			{
				if (flags & ConversionFlag::Template)//This requires compliance from all
				{

					//TODO:This doesn't work as expected because it's empty and thus will not fail
					
					//If this inherits nothing it literally cannot be used for anything
					for (auto& inherit : inheritance)
					{
						if (other->IsConvertibleTo(inherit.type, scope) != ConversionEnum::TypeDefined) {
							return ConversionResult::Ineligible;
						}
					}

					result = ConversionEnum::TempConvert;
				}
				else if (0)//This requires compliance from some
				{
					for (auto& inherit : inheritance)
					{
						if (other->IsConvertibleTo(inherit.type, scope) != ConversionEnum::TypeDefined) {
							return ConversionResult::Ineligible;
						}
					}
				}
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


		void SetSelfQualifiers(Qualifier& qualifiers) const override
		{
			qualifiers.MakeReadonly(true);
		}


	};


}