#pragma once

//#include "BasicQualifier.h"
//#include "RuntimeQualifier.h"

#include "ParamInput.h"
#include "GenericInput.h"

//Seperate.
#include "QualifiedType.h"

namespace LEX
{
	struct ITypePolicy;
	struct OverloadClause;


	struct _DefInput : public QualifiedType
	{
		std::string name;

		_DefInput()
		{

		}
	};



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

		virtual Qualifier GetQualifiers() const = 0;




		virtual std::vector<ITypePolicy*> GetGenericInput() = 0;
		virtual std::vector<DefaultGenericInput> GetDefaultGenericInput() = 0;

		virtual std::vector<ParamInput> GetParamInput() = 0;
		virtual std::vector<DefaultParamInput> GetDefaultParamInput() = 0;

		virtual OverloadClause* AsClause() { return nullptr; }





		bool IsVolatility(Qualifier v) const
		{
			return FilterEquals<Qualifier::Volatility_>(GetQualifiers(), v);
		}

		bool IsConst() const
		{
			return IsVolatility(Qualifier::Const);
		}

		bool IsModable() const
		{
			return IsVolatility(Qualifier::Modable);
		}


		bool IsMutable() const
		{
			return IsVolatility(Qualifier::Mutable);
		}

		Qualifier GetVolatility() const
		{
			return GetQualifiers() & std::to_underlying(Qualifier::Volatility_);
		}





		//Maybe make an enum so I can use one function for all.
		virtual std::vector<QualifiedType> _GetInput(size_t group)
		{
			//this should only get the primary input, and exclude 
			return {};
		}
		
		virtual std::vector<_DefInput> _GetOptInput(size_t group)
		{
			return {};
		}

		size_t _NumOfParamGroups() const
		{
			return 0;
		}

		size_t _NumOfOptGroups() const
		{
			return 0;
		}

		size_t _NumOfEntries() const
		{
			return 0;
		}
	};


}