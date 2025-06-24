#include "Lexicon/Engine/Conversion.h"

#include "Lexicon/Engine/QualifiedType.h"

namespace LEX
{

	std::optional<std::string_view> ConvertResult::GetViewFromQType(const QualifiedType& q_type, bool right)
	{
		if (right)
		{
			switch (data)
			{
			default:
				//case ConversionEnum::None_Type:
				//case ConversionEnum::None_Ref:
				//case ConversionEnum::None_Const:
				return std::nullopt;


			case ConversionEnum::Type_Ref:
			case ConversionEnum::Type_None:
			case ConversionEnum::Type_Type:
			case ConversionEnum::Type_Const:
				return q_type->GetName();


			case ConversionEnum::Ref_Ref:
			case ConversionEnum::Ref_None:
			case ConversionEnum::Ref_Type:
			case ConversionEnum::Ref_Const:
				return magic_enum::enum_name(q_type.reference);

			case ConversionEnum::Const_Ref:
			case ConversionEnum::Const_None:
			case ConversionEnum::Const_Type:
			case ConversionEnum::Const_Const:
				return magic_enum::enum_name(q_type.constness);
			}
		}
		else
		{
			switch (data)
			{
			default:
				//case ConversionEnum::Type_None:
				//case ConversionEnum::Const_None:
				//case ConversionEnum::Ref_None:
				return std::nullopt;

			case ConversionEnum::None_Ref:
			case ConversionEnum::Type_Ref:
			case ConversionEnum::Ref_Ref:
			case ConversionEnum::Const_Ref:
				return magic_enum::enum_name(q_type.reference);

			case ConversionEnum::None_Type:
			case ConversionEnum::Type_Type:
			case ConversionEnum::Ref_Type:
			case ConversionEnum::Const_Type:
				return q_type->GetName();

			case ConversionEnum::None_Const:
			case ConversionEnum::Ref_Const:
			case ConversionEnum::Const_Const:
			case ConversionEnum::Type_Const:
				return magic_enum::enum_name(q_type.constness);

			}
		}
	}

	void ConvertResult::PrintError(SyntaxRecord& record, const QualifiedType& lhs, const QualifiedType& rhs, const std::source_location& loc)
	{
		auto left_view = GetViewFromQType(lhs, false);
		auto right_view = GetViewFromQType(lhs, true);

		std::string_view* first = left_view.has_value() ?
			&left_view.value() : right_view.has_value() ?
			&right_view.value() : nullptr;

		std::string_view* second = right_view.has_value() ?
			&right_view.value() : nullptr;


		if (!first) {
			record.error(message, loc);
		}
		else if (!second) {
			record.error(message, loc, *first);
		}
		else {
			record.error(message, loc, *first, *second);
		}

	}

}