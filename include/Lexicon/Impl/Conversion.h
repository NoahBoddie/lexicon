#pragma once


namespace LEX
{
	struct IFunction;
	struct ICallableUnit;


	enum struct ConvertResult
	{

		QualError4 = -7,
		QualError3 = -6,
		QualError2 = -5,
		QualError1 = -4,
		//This shit needs failures brought about due to constness or references or some shit like that.
		Inaccessible = -3,
		Ineligible = -2,
		IllegalIntern = -1,
		//Might rearrange these to be greater
		Exact = 0,
		TypeDefined,
		ImplDefined,
		UserDefined,
		UserToImplDefined,



		Failure = -1,//Anything under or equal to failure doesn't need it's value recorded for anything, it's just a conversion error code.
		Transformative = 2,//Anything equal or greater than transformative is not valid to be used against something under said value.
	};

	constexpr ConvertResult convertFailure = ConvertResult::Failure;


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