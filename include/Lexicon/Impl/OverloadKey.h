#pragma once

#include "BasicQualifier.h"
#include "RuntimeQualifier.h"

#include "ParamInput.h"
#include "GenericInput.h"


namespace LEX
{
	struct ITypePolicy;
	struct OverloadClause;


	struct OverloadKey
	{
	public:
		virtual ~OverloadKey() = default;



		virtual ITypePolicy* GetTarget() = 0;

		//This isn't quite right, the they should share something that tells you if they're const or not.
		// Some way to get these sort of basic/runtime flags

		//Simple way would probably be to have a function do it like Field.
		// Within input, it stores a target object, so no issues getting the flags there.
		// within a function policy, it's pretty easy to just make them on the spot.

		virtual BasicQualifier GetBasicFlags() const = 0;

		virtual RuntimeQualifier GetRuntimeFlags() const = 0;




		virtual std::vector<ITypePolicy*> GetGenericInput() = 0;
		virtual std::vector<DefaultGenericInput> GetDefaultGenericInput() = 0;

		virtual std::vector<ParamInput> GetParamInput() = 0;
		virtual std::vector<DefaultParamInput> GetDefaultParamInput() = 0;

		virtual OverloadClause* AsClause() { return nullptr; }





		bool IsVolatility(BasicQualifier v) const
		{
			return FilterEquals<BasicQualifier::Volatility_>(GetBasicFlags(), v);
		}

		bool IsConst() const
		{
			return IsVolatility(BasicQualifier::Const);
		}

		bool IsModable() const
		{
			return IsVolatility(BasicQualifier::Modable);
		}


		bool IsMutable() const
		{
			return IsVolatility(BasicQualifier::Mutable);
		}

		BasicQualifier GetVolatility() const
		{
			return GetBasicFlags() & std::to_underlying(BasicQualifier::Volatility_);
		}



	};

}