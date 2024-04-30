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


	using RequiredArg = QualifiedType;


	struct OptionalArg : public RequiredArg
	{
		std::string name;

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



		//I have no idea how I'd actually use generics quite yet, so I'll prefer to not use them.
		//virtual std::vector<ITypePolicy*> GetGenericInput() = 0;
		//virtual std::vector<DefaultGenericInput> GetDefaultGenericInput() = 0;

		virtual OverloadClause* AsClause() { return nullptr; }





		bool IsVolatility(Qualifier v) const
		{
			return FilterEquals<Qualifier::Constness_>(GetQualifiers(), v);
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
			return GetQualifiers() & std::to_underlying(Qualifier::Constness_);
		}




		//I get why I need all of these, but seriously, compress this shit dog.

		//virtual size_t GetReqInputGroupSize() const = 0;

		//virtual size_t GetOptInputGroupSize() const = 0;

		//virtual size_t GetReqInputSize() const = 0;

		//virtual size_t GetOptInputSize() const = 0;
		
		//Ok so sorry to do this, but all of these can go back to being arguments. Because the only time they'll be used is as arguments.

		virtual std::pair<size_t, size_t> GetNumOfInputs() const = 0;

		virtual std::pair<size_t, size_t> GetNumOfInputGroups() const = 0;

		virtual std::vector<RequiredArg> GetRequiredInput(size_t offset) const = 0;


		virtual std::vector<OptionalArg> GetOptionalInput(size_t offset) const = 0;



		size_t GetInputSize() const
		{
			auto entries = GetNumOfInputs();

			return entries.first + entries.second;
		}
		//Param/Template
		//Required/Optional
		//All of these are inputs.
		//params and args are inputs, generic params and args are templates. This is just to resolve the names so I know what's what.




	};


}