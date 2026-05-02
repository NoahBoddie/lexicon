#pragma once

#include "Lexicon/Engine/SyntaxRecord.h"
#include "Lexicon/Engine/Register.h"

namespace LEX
{
	struct IFunction;
	struct ICallableUnit;

	struct QualifiedType;

	struct ITypeInfo;
	struct TypeInfo;
	class Runtime;
	struct Variable;
	class RuntimeVariable;

	struct Solution;
	struct ExpressionCompiler;



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

	using Converter_ = RuntimeVariable(*)(const RuntimeVariable&, Runtime*);



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

		void Test()
		{
			data <=> data;
			message <=> message;
		}

		operator bool() const
		{
			return !IsFailure();
		}
		constexpr auto operator <=>(const ConvertResult& other) const noexcept
		{
			if (auto res = data <=> other.data; res != std::strong_ordering::equal) {
				return res;
			}

			return message <=> other.message;
		}
		constexpr auto operator <=>(ConversionEnum e) const
		{
			return data <=> e;
		}

		constexpr auto operator ==(ConversionEnum e) const
		{
			return data == e;
		}

		//Fill the rest of these out.
		constexpr auto operator !=(ConversionEnum e) const
		{
			return data != e;
		}
		

		//Want to make an ease of use constructor to make this, all you'd need to do is give it what you'd intend to use with it, ref
		// constness, etc and such.

		std::optional<std::string_view> GetViewFromQType(const QualifiedType& q_type, bool right);

		void PrintError(SyntaxRecord& record, const QualifiedType& lhs, const QualifiedType& rhs, const std::source_location& loc = std::source_location::current());

	};


	struct ConversionResult
	{
		static constexpr ConvertResult Generic{ ConversionEnum::None_None, 0 };
		static constexpr ConvertResult Ineligible{ ConversionEnum::None_None, 60 };
		static constexpr ConvertResult Inaccessible{ ConversionEnum::None_None, 61 };
		static constexpr ConvertResult IneligibleQuals{ ConversionEnum::None_None, 62 };
		static constexpr ConvertResult Unassignable{ ConversionEnum::Const_None, 63 };
	};

	
	//using ConvertFunc = RuntimeVariable(*)(RuntimeVariable);


	struct NewConversion
	{
		using test = std::variant<IFunction*, ITypeInfo*, Converter_>;

		union
		{
			uint64_t		raw = 0;

			IFunction*		userDefined;
			Converter_		implDefined;
			ITypeInfo*		typeDefined;
		};


	};


	struct Conversion
	{
		enum Enum
		{
			None,
			IsType1 = 1 << 0,
			IsType2 = 1 << 1,
		};

		void SetUserImpl(IFunction* func)
		{
			userDefined = func;
			usesType &= ~IsType1;

		}

		void SetUserImpl(Converter_ convert)
		{
			implDefined = convert;
			usesType &= ~IsType1;
		}


		void SetUserImpl(ITypeInfo* type)
		{
			typeDefined = type;
			usesType |= IsType1;
		}

		void SetUserTo(Converter_ convert)
		{
			userToImpl = convert;
			usesType &= ~IsType2;
		}


		void SetUserTo(ITypeInfo* type)
		{
			userToType = type;
			usesType |= IsType2;
		}


		//RuntimeVariable;
		Variable Run(Variable from, ConvertResult result) const;


		bool HandleInstruction(ExpressionCompiler* compiler, SyntaxRecord& target, Solution& value, ConvertResult result, Register reg);

		//Creates an operand based on the conversion type if any.
		/*
		//Operand uses this by proxy, so it needs to be in a source file
		//#include "Lexicon/Engine/Operand.h"
		std::vector<Operand> CreateOperand(ConvertResult res)
		{
			//Steal from compile util more pls
			bool fall = false;

			switch (res.data)
			{
			case ConversionEnum::ImplDefined:
				compiler->EmplaceInstruction(target, InstructionType::Convert, reg, Operand{ out.implDefined, OperandType::Callable }, value);
				break;

			case ConversionEnum::UserDefined:

				compiler->EmplaceInstruction(target, InstructionType::Convert, reg, Operand{ out.userDefined, OperandType::Function }, value);


				if (!fall)
					break;

				[[fallthrough]];

			case ConversionEnum::UserToImplDefined:
				return Operand{ userToImpl, OperandType::Callable };

			default:
				return {};
			}


		}
		//*/
		//Merge convert results with this shit btw.

		//This should tell how you jumped from one value to the next. Approx 3 pointers long.

		operator bool() const
		{
			return raw1 || raw2;
		}

	private:
		struct
		{
			union
			{
				uint64_t		raw1 = 0;
				IFunction*		userDefined;
				Converter_		implDefined;
				ITypeInfo*		typeDefined;
			};

			union
			{
				//When a user defined conversion can be converted this is what is used.
				uint64_t		raw2 = 0;
				Converter_		userToImpl;
				ITypeInfo*		userToType;
			};

			Enum usesType = None;
		};

	};
}