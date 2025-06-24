#pragma once

#include "Lexicon/Engine/QualifiedType.h"
#include "Lexicon/Impl/common_type.h"
#include "Lexicon/Engine/Runtime.h"
#include "Lexicon/Engine/RuntimeUtility.h"


namespace LEX
{

	//I just realized that QualifiedType will have some isuses when it comes to che

	template<Refness Ref, bool IsSigned>
	struct ReferenceCheck final : public ICallableUnit
	{
		using Self = ReferenceCheck<Ref, IsSigned>;

		static constexpr auto Key = std::pair{ Ref, IsSigned };

		static Self* GetSingleton()
		{
			static Self singleton{};

			return std::addressof(singleton);
		}

		virtual RuntimeVariable Execute(std::span<RuntimeVariable> args, Runtime* runtime, RuntimeVariable*)
		{
			auto settings = Number::Settings::CreateFromType<bool>();
			
			if constexpr (IsSigned){
				settings.sign = Signage::Signed;
			}

			Number result = settings;


			result.Assign(RunUtil::CantPromote(args[0], runtime, Ref) == 0);

			return result;
		}


		

	protected:
		constexpr ReferenceCheck() = default;

		ReferenceCheck(const ReferenceCheck&) = delete;
		ReferenceCheck(ReferenceCheck&&) = delete;
		ReferenceCheck& operator=(const ReferenceCheck&) = delete;
		ReferenceCheck& operator=(ReferenceCheck&&) = delete;

	};

	inline std::map<std::pair<Refness, bool>, ICallableUnit*> refCheckList{};

	template <typename T>
	void RegisterRefChecker(T* singleton)
	{
		refCheckList[T::Key] = singleton;
	}
#define TEST_INIT() inline extern void __init_func_8(); volatile inline static Initializer __init_var_8 = { __init_func_8 }; inline extern void __init_func_8()
	TEST_INIT()
	{
		//TODO:This won't work unless I do this I'm unsure why.
		RegisterRefChecker(ReferenceCheck<Refness::Local, true>::GetSingleton());
		RegisterRefChecker(ReferenceCheck<Refness::Local, false>::GetSingleton());
		RegisterRefChecker(ReferenceCheck<Refness::Scoped, true>::GetSingleton());
		RegisterRefChecker(ReferenceCheck<Refness::Scoped, false>::GetSingleton());
		RegisterRefChecker(ReferenceCheck<Refness::Global, true>::GetSingleton());
		RegisterRefChecker(ReferenceCheck<Refness::Global, false>::GetSingleton());

	};
#undef TEST_INIT

	ConvertResult QualifiedType::IsQualified(const QualifiedType& to_left, ConversionFlag flags, Conversion** out) const
	{
		//TODO: I don't know why this shit is reversed like this but fix it. I know it doesn't work the other way around, but fix it.

		auto result = ConversionEnum::Exact;

		if (this->flags & QualifierFlag::Promoted)
		{
			if (to_left.IsReference(true) == true) {
				//Pass?
			}
			//This needs more
			else if (common_type::boolean() == to_left) {//Is a boolean
				(*out)->implDefined = refCheckList[std::make_pair(reference, false)];
				return ConversionEnum::ImplDefined;
			}
			else {
				return ConversionResult::IneligibleQuals;
			}
		}

		/*
		//QualifiedType boolean { common_type::boolean(), Qualifier{}}

		if (!out) {
			return ConversionEnum::QualError1;
		}
		auto* converse = *out;
		//There's got to be a better way to do this.
		if (other.IsConvertToQualified(QualifiedType{ common_type::boolean() }, nullptr, nullptr, flags) <= ConversionEnum::Failure// &&
			)//other.IsConvertToQualified(QualifiedType{ common_type::uboolean() }, nullptr, nullptr, flags) <= ConversionEnum::Failure)
		{
			return ConversionEnum::QualError1;
		}

		//I need to figure out how to do this, even when the boolean type may not be boolean. Perhaps check if
		// it's an integral number type?

		//converse->implDefined = refCheckList[]

		//*/

		//Refness section
		bool init = flags & ConversionFlag::Initialize;
		bool param = flags & ConversionFlag::Parameter;
		bool ret = flags & ConversionFlag::Return;
		bool expl = flags & ConversionFlag::Explicit;
		
		if (init || param || ret)
		{
			
			if (out && IsReference(false) == true) {
				//No conversions allowed with references
				*out = nullptr;
			}


			//We only care about references if they're being initialized
			Refness refl = to_left.reference;
			Refness refr = reference;

			//While maybe ref is technically a reference, it needs to be promoted first
			bool not_ref = refr == Refness::Temp || refr == Refness::Maybe;
			bool equals = refl == refr;

			bool ref_check = true;

			switch (refl)
			{
			case Refness::Global://For global it must be equal, no exceptions
				if (!equals && refr != Refness::Static)
					return ConversionResult::IneligibleQuals;
				break;

			case Refness::Scoped:
			case Refness::Local:
				if (not_ref)
					return ConversionResult::IneligibleQuals;

				if (!param)//If this is a param, anything goes accept for maybe ref stuff.
				{
					if (ret && equals)
						return ConversionResult::IneligibleQuals;

					if (init && refr < refl)
						return ConversionResult::IneligibleQuals;
				}
				break;

			case Refness::Static://These 2 literally should never be a return type, and assign wise, this has no issues.
			case Refness::Auto:
			case Refness::Maybe:
			case Refness::Temp://Is this immutable?
				//Anything can go into these, there's no restrictions
				ref_check = false;
				break;

			case Refness::Generic:
				if (refr == Refness::Temp) {
					return ConversionResult::IneligibleQuals;//I have no idea if this is the right one, I just don't much care right now.
				}

				break;
			}


			if (IsReference(true) && to_left.IsReferential() && !equals) {
				result = ConversionEnum::RefConvert;
			}

			//Auto cannot be accepted by anything at all
		}
		else if (!expl && to_left.IsAssignable() == false) {
			//If this isn't the initialization of something, and isn't assignable,that's a problem
			return ConversionResult::Unassignable;
		}



		//This does a conversion but only on the qualifiers.

		//auto comp = qualifiers & other.qualifiers;

		//These in essence did a switch. need to rename to To and from.
		Constness l_comp = to_left.constness;
		Constness r_comp = constness;

		//if (l_comp != r_comp) {
		if (IsConstnessEqual(to_left) == false) {

			//this  first bit makes no sense, if the left side is const anything can still go into it (as long as we are initializing).
			if (1 != 1) {
				//if (l_comp == Qualifier::Const)
				if (r_comp == Constness::Const)
				{
					//If left is const, it must be a class type.
					if (policy->IsReferType() == false)
						return ConversionResult::IneligibleQuals;
				}
			}




			//This doesn't seem to work, but it will be hit by a forcible transfer issue.
			//if (l_comp == Constness::Const)
			//if (other.IsAssignable(param) == false)
			if (to_left.IsConst() == true)
			{
				//If right is const, it must be a value.
				if (to_left.policy->IsValueType() == false)
					return ConversionResult::IneligibleQuals;
			}

			if (result == ConversionEnum::Exact)
				result = ConversionEnum::ConstConvert;
		}

		//Simple for now.
		return result;
	}


	ConvertResult QualifiedType::IsConvertToQualified(const QualifiedType& to_left, ITypeInfo* scope, Conversion* out, ConversionFlag flags) const
	{

		//TODO: ConvertQualified needs 2 rules, read below
		// first if ref types are used, no conversions are allowed (return to nullptr). Also that Conversion on maybe refs should present a warning
		// due to not actually using a reference if a reference was desired.



#ifdef DISABLE_THIS
		if (auto result = IsQualified(to_left, flags, &out); result != ConversionEnum::Exact || out)
			return result;

		//Simple for now.
		return policy->IsConvertibleTo(to_left.policy, scope, out, flags);
#endif

		//TODO: ConverstResult needs to be comprised of more types.  \/
		// Namely, something to tell what error it should give, as well as a function that takes the qualified types that failed

		ConvertResult qual_result = IsQualified(to_left, flags, &out);

		if (qual_result <= ConversionEnum::Failure || qual_result > ConversionEnum::Transformative)
			return qual_result;

		if (auto type_result = policy->IsConvertibleTo(to_left.policy, scope, out, flags); type_result != ConversionEnum::Exact)
			return type_result;

		return qual_result;
	}
}