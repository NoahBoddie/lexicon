#include "Lexicon/Engine/ITypeInfoImpl.h"


#include "Lexicon/Variable.h"

#include "Lexicon/Engine/HierarchyData.h"
#include "Lexicon/Engine/Convert.h"

namespace LEX
{
	ConvertResult ITypeInfo::GetConvertTo_Hierarchy(const ITypeInfo* other, const ITypeInfo* scope, Conversion* out, ConversionFlag flags) const
	{
		if (this == other) {
			return ConversionEnum::Exact;
		}

		auto hierarchy = GetHierarchyData();

		if (!hierarchy)
			return ConversionResult::Ineligible;

		//First we check if the entry exists. Based on what its access is, what we do changes.
		//Public: Conversion is allowed.
		//Protected: scope type must other must derive from
		//Private: scope type must have other's distance be 1.

		//Internal: The trail of internals within scope must lead to an internal that has an owner index of 0. Otherwise, it is too seperated to be converted.
		//Will need to make a collection to measure this.
		// Actually, simplier way (though the above is still required) this and scope need to be the same. Actually this isn't right either, internal is about
		// this, not who's scope.


		//Whether it can convert should basically be solved here, this should never return an internal it does not own.
		const InheritData* convert_data = hierarchy->GetInheritData(other);


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
			logger::debug("Owner should be 0 => {}", convert_data->ownerIndex);
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


	bool ITypeInfo::GetConvertTo_Intrinsic(const ITypeInfo* other, Conversion* out, bool exp) const
	{
		//Handles the conversions of strings and bool

		if (out && exp) {
			if (common_type::string() == other) {
				constexpr auto func = [](const RuntimeVariable& var, Runtime*) -> RuntimeVariable {return var->PrintString(); };
				out->SetUserImpl(func);
				return true;
			}

			else if (common_type::boolean() == other) {
				constexpr auto func = [](const RuntimeVariable& var, Runtime*) -> RuntimeVariable {return !var->IsValueZero(); };
				out->SetUserImpl(func);
				return true;
			}

		}
		return false;
	}
}