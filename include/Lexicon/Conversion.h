#pragma once

#include "Lexicon/Engine/SyntaxRecord.h"

namespace LEX
{
	struct IFunction;
	struct ICallableUnit;

	struct QualifiedType;


	enum struct ConversionEnum
	{
		None_None = -16,
		None_Type,
		None_Ref,
		None_Const,

		Type_Ref,
		Type_None,
		Type_Type,
		Type_Const,

		Ref_Ref,
		Ref_None,
		Ref_Type,
		Ref_Const,

		Const_Ref,
		Const_None,
		Const_Type,
		Const_Const,
		//Might rearrange these to be greater
		Exact = 0,
		RefConvert,
		ConstConvert,
		TempConvert,	//Nearly exact, but via template. Loses to exactness of other kinds
		TypeDefined,

		ImplDefined,
		UserDefined,
		UserToImplDefined,

		Failure = -1,
		Transformative = ImplDefined,//Anything equal or greater than transformative is not valid to be used against something under said value.
	};


	struct ConvertResult
	{
		constexpr ConvertResult() = default;
		constexpr ConvertResult(ConversionEnum e, IssueCode msg = 0) : data{ e }, message{ msg } {}

		ConversionEnum data = ConversionEnum::Failure;

		//I'll make common versions of these that classes will use

		IssueCode message{};

		bool IsFailure() const
		{
			return data <= ConversionEnum::Failure;
		}

		operator bool() const
		{
			return !IsFailure();
		}

		constexpr auto operator <=>(ConversionEnum e) const
		{
			return data <=> e;
		}

		//Fill the rest of these out.
		constexpr auto operator !=(ConversionEnum e) const
		{
			return data != e;
		}


		//Want to make an ease of use constructor to make this, all you'd need to do is give it what you'd intend to use with it, ref
		// constness, etc and such.

		std::optional<std::string_view> GetViewFromQType(const QualifiedType& q_type, bool right);

		void PrintError(const SyntaxRecord& record, const QualifiedType& lhs, const QualifiedType& rhs, const std::source_location& loc = std::source_location::current());

	};


	struct ConversionResult
	{
		static constexpr ConvertResult Generic{ ConversionEnum::None_None, 0 };
		static constexpr ConvertResult Ineligible{ ConversionEnum::None_None, 60 };
		static constexpr ConvertResult Inaccessible{ ConversionEnum::None_None, 61 };
		static constexpr ConvertResult IneligibleQuals{ ConversionEnum::None_None, 62 };
	};

	



	struct Conversion
	{
		struct
		{
			union
			{
				uint64_t		raw = 0;
				IFunction*		userDefined;
				ICallableUnit*	implDefined;
			};

			//When a user defined conversion can be converted this is what is used.
			ICallableUnit* userToImpl = nullptr;

		};

		//Merge convert results with this shit btw.

		//This should tell how you jumped from one value to the next. Approx 3 pointers long.

		operator bool() const
		{
			return raw;
		}
	};
}